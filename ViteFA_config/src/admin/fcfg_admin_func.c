#include "fcfg_proto.h"
#include "fcfg_types.h"
#include "fcfg_admin_func.h"
#include "fastcommon/sockopt.h"
#include "fcfg_admin.h"

void fcfg_set_admin_join_req(struct fcfg_context *fcfg_context, char *buff,
        int *body_len)
{
    FCFGProtoAdminJoinReq *join_req = (FCFGProtoAdminJoinReq *)buff;
    unsigned char username_len = strlen(fcfg_context->username);
    unsigned char secret_key_len = strlen(fcfg_context->secret_key);
    join_req->username_len = username_len;
    join_req->secret_key_len = secret_key_len;
    memcpy(join_req->username,
            fcfg_context->username,
            username_len);
    memcpy(join_req->username + username_len,
           fcfg_context->secret_key,
           secret_key_len);

    *body_len = sizeof(FCFGProtoAdminJoinReq) + secret_key_len + username_len;
}

int fcfg_admin_load_config (struct fcfg_context *fcfg_context,
        const char *config_filename)
{
    IniContext ini_context;
    int result;
    char *pBasePath;
    char *pDataPath;
    char *config_server[FCFG_CONFIG_SERVER_COUNT_MAX];
    int server_count;
    int i;

    memset(&ini_context, 0, sizeof(IniContext));
    if ((result=iniLoadFromFile(config_filename, &ini_context)) != 0) {
        logError("file: "__FILE__", line: %d "
                "load conf file \"%s\" fail, ret code: %d ",
                __LINE__, config_filename, result);
        return result;
    }
    load_log_level(&ini_context);

    server_count = iniGetValues(NULL, "config_server",
                    &ini_context, config_server, FCFG_CONFIG_SERVER_COUNT_MAX);
    if (server_count <= 0) {
        logError("file: "__FILE__", line: %d "
                "get config_server fail %d ", __LINE__, server_count);
        return -1;
    }
    fcfg_context->server_count = server_count;
    fcfg_context->join_conn = (ConnectionInfo *)malloc(server_count *
            sizeof(ConnectionInfo));
    if (fcfg_context->join_conn == NULL) {
        logError("file: "__FILE__", line: %d "
                "malloc fail", __LINE__);
        return 1;
    }
    memset(fcfg_context->join_conn, 0,
            server_count * sizeof(ConnectionInfo));
    for (i = 0; i < server_count; i ++) {
        conn_pool_parse_server_info(config_server[i],
                fcfg_context->join_conn + i, FCFG_SERVER_DEFAULT_INNER_PORT);
        logDebug("file: "__FILE__", line: %d "
                "config_server: %s", __LINE__, config_server[i]);
    }

    fcfg_context->network_timeout = iniGetIntValue(NULL, "network_timeout",
            &ini_context, FCFG_NETWORK_TIMEOUT_DEFAULT);

    fcfg_context->connect_timeout = iniGetIntValue(NULL, "connect_timeout",
            &ini_context, FCFG_CONNECT_TIMEOUT_DEFAULT);

    pDataPath = iniGetStrValue(NULL, "username", &ini_context);
    if (pDataPath == NULL || *pDataPath == '\0') {
        logError("file: "__FILE__", line: %d "
                "get username from file:%s", __LINE__, config_filename);
        return ENOENT;
    }
    snprintf(fcfg_context->username, sizeof(fcfg_context->username), "%s",
            pDataPath);

    pDataPath = iniGetStrValue(NULL, "secret_key", &ini_context);
    if (pDataPath == NULL || *pDataPath == '\0') {
        logError("file: "__FILE__", line: %d "
                "get secret_key from file:%s", __LINE__, config_filename);
        return ENOENT;
    }
    snprintf(fcfg_context->secret_key, sizeof(fcfg_context->secret_key), "%s",
            pDataPath);

    pBasePath = iniGetStrValue(NULL, "base_path", &ini_context);
    if (pBasePath) {
        snprintf(fcfg_context->base_path, sizeof(fcfg_context->base_path),
                "%s", pBasePath);
        chopPath(fcfg_context->base_path);
        if (!fileExists(fcfg_context->base_path)) {
            logError("file: "__FILE__", line: %d, " \
                    "\"%s\" can't be accessed, error info: %s", \
                    __LINE__, fcfg_context->base_path, STRERROR(errno));
            result = errno != 0 ? errno : ENOENT;
            return result;
        }
        if (!isDir(fcfg_context->base_path)) {
            logError("file: "__FILE__", line: %d, " \
                    "\"%s\" is not a directory!", \
                    __LINE__, fcfg_context->base_path);
            result = ENOTDIR;
            return result;
        }
        if ((result=log_set_prefix(fcfg_context->base_path, "fcfg_admin")) != 0) {
            return result;
        }
    }

    iniFreeContext(&ini_context);
    return 0;
}

int fcfg_admin_init_from_file (struct fcfg_context *fcfg_context,
        const char *config_filename)
{
    int ret;

    memset(fcfg_context, 0, sizeof(struct fcfg_context));
    fcfg_context->join_index = -1;
    ret = fcfg_admin_load_config(fcfg_context, config_filename);
    if (ret == 0) {
        ret = fcfg_do_conn_config_server(fcfg_context); 
        if (ret == 0) {
            ret = fcfg_send_admin_join_request(fcfg_context,
                    fcfg_context->network_timeout,
                    fcfg_context->connect_timeout);
        }
    }

    return ret;
}

int fcfg_admin_destroy (struct fcfg_context *fcfg_context)
{
    if ((fcfg_context->join_index > 0) && fcfg_context->join_conn) {
        fcfg_disconn_config_server(fcfg_context->join_conn + fcfg_context->join_index);
    }
    if (fcfg_context->join_conn) {
        free(fcfg_context->join_conn);
    }

    return 0;
}

int fcfg_send_admin_join_request(struct fcfg_context *fcfg_context, int network_timeout,
        int connect_timeout)
{
    int ret;
    char buff[1024];
    int body_len;
    int size;
    FCFGResponseInfo resp_info;
    FCFGProtoHeader *fcfg_header_proto;
    ConnectionInfo *join_conn = fcfg_context->join_conn +
        fcfg_context->join_index;

    fcfg_header_proto = (FCFGProtoHeader *)buff;
    fcfg_set_admin_join_req(fcfg_context, buff + sizeof(FCFGProtoHeader), &body_len);
    fcfg_set_admin_header(fcfg_header_proto, FCFG_PROTO_ADMIN_JOIN_REQ, body_len);
    size = sizeof(FCFGProtoHeader) + body_len;
    ret = send_and_recv_response_header(join_conn, buff, size, &resp_info,
            network_timeout);
    if (ret) {
        logError("file: "__FILE__", line: %d "
                "send_and_recv_response_header fail. ret:%d, %s",
                __LINE__, ret, strerror(ret));
        return ret;
    }
    ret = fcfg_admin_check_response (join_conn, &resp_info, network_timeout, FCFG_PROTO_ACK);
    if (ret) {
        logError("file: "__FILE__", line: %d "
                "join server fail. %s", __LINE__, resp_info.error.message);
    } else {
        logDebug("file: "__FILE__", line: %d "
                "join server success!", __LINE__);
    }

    return ret;
}

int fcfg_do_conn_config_server (struct fcfg_context *fcfg_context)
{
    int ret;
    int server_index;
    ConnectionInfo *join_conn;
    int i;

    i = 0;
    srand(time(NULL));
    server_index = rand() % fcfg_context->server_count;
    while (i < fcfg_context->server_count) {
        join_conn = fcfg_context->join_conn + server_index;
        if ((ret = conn_pool_connect_server(join_conn, 1000 *
                        fcfg_context->connect_timeout)) != 0)
        {
            logError("file: "__FILE__", line: %d "
                    "conn_pool_connect_server fail. server index[%d] %s:%d, "
                    "ret:%d, %s", __LINE__, server_index, join_conn->ip_addr,
                    join_conn->port, ret, strerror(ret));
            server_index = (server_index + 1) % fcfg_context->server_count;
            i++;
        } else {
            /* connect success */
            fcfg_context->join_index = server_index;
            break;
        }
    }

    return ret;
}

void fcfg_disconn_config_server (ConnectionInfo *conn)
{
    if (conn && conn->sock >= 0) {
        conn_pool_disconnect_server(conn);
    }
}
int fcfg_admin_env_set_entry(FCFGProtoGetEnvResp *get_env_resp,
        FCFGEnvEntry *rows, int *env_size)
{
    int size;
    rows->env.len = get_env_resp->env_len;
    rows->create_time = buff2int(get_env_resp->create_time);
    rows->update_time = buff2int(get_env_resp->update_time);

    size = rows->env.len;
    rows->env.str = (char *)malloc(size + 1);
    if (rows->env.str == NULL) {
        logError("file: "__FILE__", line: %d, "
                "malloc %d bytes fail", __LINE__, size + 1);
        return ENOMEM;
    }
    strncpy(rows->env.str, get_env_resp->env, rows->env.len);
    rows->env.str[rows->env.len] = '\0';

    *env_size = size + sizeof(FCFGProtoGetEnvResp);
    return 0;
}

int fcfg_admin_config_set_entry (FCFGProtoGetConfigResp *get_config_resp,
        FCFGConfigEntry *rows, int *config_len)
{
    int size;
    rows->status = get_config_resp->status;
    rows->name.len = get_config_resp->name_len;
    rows->type = get_config_resp->type;
    rows->value.len = buff2int(get_config_resp->value_len);
    rows->version = buff2long(get_config_resp->version);
    rows->create_time = buff2int(get_config_resp->create_time);
    rows->update_time = buff2int(get_config_resp->update_time);

    size = rows->name.len + rows->value.len;
    rows->name.str = (char *)malloc(size + 2);
    if (rows->name.str == NULL) {
        logError("file: "__FILE__", line: %d, "
                "malloc %d bytes fail", __LINE__, size + 2);
        return ENOMEM;
    }
    strncpy(rows->name.str, get_config_resp->name, rows->name.len);
    rows->name.str[rows->name.len] = '\0';
    rows->value.str = rows->name.str + rows->name.len + 1;
    strncpy(rows->value.str,
           get_config_resp->name + rows->name.len,
           rows->value.len);
    rows->value.str[rows->value.len] = '\0';
    *config_len = sizeof(FCFGProtoGetConfigResp) + size;

    return 0;
}

int fcfg_admin_check_arg(const char *env, const char *name, const char *value)
{
    if (env && (strlen(env) > FCFG_CONFIG_MAX_ENV_LEN)) {
        logError("env len too long %lu > %d", strlen(env), FCFG_CONFIG_MAX_ENV_LEN);
        return -1;
    }

    if (name && (strlen(name) > FCFG_CONFIG_MAX_NAME_LEN)) {
        logError("config name len too long %lu > %d", strlen(name), FCFG_CONFIG_MAX_NAME_LEN);
        return -1;
    }

    if (value && (strlen(value) > FCFG_CONFIG_MAX_VALUE_LEN)) {
        logError("config value len too long %lu > %d", strlen(value), FCFG_CONFIG_MAX_VALUE_LEN);
        return -1;
    }

    return 0;
}
