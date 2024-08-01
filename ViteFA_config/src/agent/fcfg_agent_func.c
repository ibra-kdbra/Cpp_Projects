
#include <sys/stat.h>
#include "fastcommon/ini_file_reader.h"
#include "fastcommon/shared_func.h"
#include "fastcommon/logger.h"
#include "sf/sf_util.h"
#include "common/fcfg_types.h"
#include "common/fcfg_proto.h"
#include "fcfg_agent_global.h"
#include "fcfg_agent_func.h"

int fcfg_agent_load_config(const char *filename)
{
    IniContext ini_context;
    int result;
    int server_count;
    int i;
    char *pDataPath;
    char *pThreadStackSize;
    int64_t thread_stack_size;
    char *config_server[FCFG_CONFIG_SERVER_COUNT_MAX];

    memset(&ini_context, 0, sizeof(IniContext));
    if ((result=iniLoadFromFile(filename, &ini_context)) != 0) {
        logError("file: "__FILE__", line: %d, "
                "load conf file \"%s\" fail, ret code: %d",
                __LINE__, filename, result);
        return result;
    }

    pDataPath = iniGetStrValue(NULL, "shm_config_file", &ini_context);
    if (pDataPath == NULL || *pDataPath == '\0') {
        lerr("get shm_config_file from file:%s", filename);
        return ENOENT;
    }
    snprintf(g_agent_global_vars.shm_config_file, MAX_PATH_SIZE, "%s",
             pDataPath);


    pDataPath = iniGetStrValue(NULL, "config_env", &ini_context);
    if (pDataPath == NULL || *pDataPath == '\0') {
        lerr("get config_env from file:%s", filename);
        return ENOENT;
    }
    snprintf(g_agent_global_vars.env, sizeof(g_agent_global_vars.env), "%s",
            pDataPath);
    snprintf(g_agent_global_vars.shm_version_key, sizeof(g_agent_global_vars.shm_version_key),
             "__%s_%s__", g_agent_global_vars.env, FCFG_AGENT_SHM_VERSION_KEY_SUFFIX);

    pThreadStackSize = iniGetStrValue(NULL,
            "thread_stack_size", &ini_context);
    if (pThreadStackSize == NULL) {
        thread_stack_size = FCFG_DEF_THREAD_STACK_SIZE;
    } else if ((result=parse_bytes(pThreadStackSize, 1,
                    &thread_stack_size)) != 0) {
        return result;
    }
    g_agent_global_vars.thread_stack_size = (int)thread_stack_size;

    g_agent_global_vars.sync_log_buff_interval = iniGetIntValue(NULL,
            "sync_log_buff_interval", &ini_context,
            FCFG_SYNC_LOG_BUFF_DEF_INTERVAL);
    if (g_agent_global_vars.sync_log_buff_interval <= 0) {
        g_agent_global_vars.sync_log_buff_interval = FCFG_SYNC_LOG_BUFF_DEF_INTERVAL;
    }

    g_agent_global_vars.rotate_error_log = iniGetBoolValue(NULL, "rotate_error_log",
            &ini_context, false);
    g_agent_global_vars.log_file_keep_days = iniGetIntValue(NULL, "log_file_keep_days",
            &ini_context, 0);

    load_log_level(&ini_context);
    if ((result=log_set_prefix(g_agent_global_vars.base_path, "fcfg_agent")) != 0) {
        return result;
    }

    server_count = iniGetValues(NULL, "config_server",
            &ini_context, config_server, FCFG_CONFIG_SERVER_COUNT_MAX);
    if (server_count <= 0) {
        lerr("get config_server fail %d", server_count);
        return -1;
    }
    g_agent_global_vars.server_count = server_count;
    g_agent_global_vars.join_conn = (ConnectionInfo *)malloc(server_count *
            sizeof(ConnectionInfo));
    if (g_agent_global_vars.join_conn == NULL) {
        lerr("malloc fail");
        return 1;
    }
    memset(g_agent_global_vars.join_conn, 0,
            server_count * sizeof(ConnectionInfo));
    for (i = 0; i < server_count; i ++) {
        conn_pool_parse_server_info(config_server[i],
                g_agent_global_vars.join_conn + i, FCFG_SERVER_DEFAULT_INNER_PORT);
        linfo("config_server: %s", config_server[i]);
    }
    g_agent_global_vars.network_timeout = iniGetIntValue(NULL, "network_timeout",
            &ini_context, FCFG_NETWORK_TIMEOUT_DEFAULT);

    g_agent_global_vars.connect_timeout = iniGetIntValue(NULL, "connect_timeout",
            &ini_context, FCFG_CONNECT_TIMEOUT_DEFAULT);
    linfo("base_path: %s, "
          "shm_config_file: %s, "
          "env: %s, "
          "shm_version_key: %s, "
          "network_timeout: %d, "
          "connect_timeout: %d",
          g_agent_global_vars.base_path,
          g_agent_global_vars.shm_config_file,
          g_agent_global_vars.env,
          g_agent_global_vars.shm_version_key,
          g_agent_global_vars.network_timeout,
          g_agent_global_vars.connect_timeout);

    iniFreeContext(&ini_context);
    return 0;
}

int fcfg_proto_set_join_req(char *buff, char *env, int64_t version, int *req_len)
{
    FCFGProtoHeader *fcfg_header_pro;
    FCFGProtoAgentJoinReq *fcfg_join_req_pro;

    fcfg_header_pro = (FCFGProtoHeader *)buff;
    fcfg_header_pro->cmd = FCFG_PROTO_AGENT_JOIN_REQ;
    int2buff(sizeof(FCFGProtoAgentJoinReq), fcfg_header_pro->body_len);

    fcfg_join_req_pro = (FCFGProtoAgentJoinReq *)(buff + sizeof(FCFGProtoHeader));
    memcpy(fcfg_join_req_pro->env, env, sizeof(fcfg_join_req_pro->env));
    long2buff(version, fcfg_join_req_pro->agent_cfg_version);
    *req_len = sizeof(FCFGProtoHeader) + sizeof(FCFGProtoAgentJoinReq);

    return 0;
}