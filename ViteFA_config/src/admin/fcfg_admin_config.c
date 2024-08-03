#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "fastcommon/sockopt.h"
#include "fcfg_proto.h"
#include "fcfg_admin_func.h"
#include "fcfg_types.h"

char *fcfg_config_type_string[6] = {
    "",
    "string",
    "",
    "list",
    "",
    "map",
};

void fcfg_set_admin_set_config(char *buff,const char *env,
        const char *config_name, const char *config_value,
        const unsigned char type, int *body_len)
{
    FCFGProtoSetConfigReq *set_config_req = (FCFGProtoSetConfigReq *)buff;
    unsigned char env_len = strlen(env);
    unsigned char name_len = strlen(config_name);
    int value_len = strlen(config_value);

    set_config_req->env_len = env_len;
    set_config_req->name_len = name_len;
    set_config_req->type = type;
    int2buff(value_len, set_config_req->value_len);
    memcpy(set_config_req->env, env,
           env_len);
    memcpy(set_config_req->env + env_len,
           config_name,
           name_len);
    memcpy(set_config_req->env + env_len + name_len,
           config_value,
           value_len);
    *body_len = sizeof(FCFGProtoSetConfigReq) + env_len + name_len + value_len;
}

int fcfg_admin_set_config (struct fcfg_context *fcfg_context,
        const char *env, const char *config_name, const char *config_value,
        const unsigned char type)
{
    int ret;
    char buff[64 + FCFG_CONFIG_ENV_SIZE + FCFG_CONFIG_NAME_SIZE + FCFG_CONFIG_VALUE_SIZE];
    int body_len;
    int size;
    FCFGResponseInfo resp_info;
    ConnectionInfo *join_conn;
    FCFGProtoHeader *fcfg_header_proto;

    join_conn = fcfg_context->join_conn + fcfg_context->join_index;
    fcfg_header_proto = (FCFGProtoHeader *)buff;
    fcfg_set_admin_set_config(buff + sizeof(FCFGProtoHeader), env, config_name,
            config_value, type, &body_len);
    fcfg_set_admin_header(fcfg_header_proto, FCFG_PROTO_SET_CONFIG_REQ, body_len);
    size = sizeof(FCFGProtoHeader) + body_len;
    ret = send_and_recv_response_header(join_conn, buff, size, &resp_info,
            fcfg_context->network_timeout);
    if (ret) {
        logError("file: "__FILE__", line: %d "
                "send_and_recv_response_header fail. ret:%d, %s",
                __LINE__, ret, strerror(ret));
        return ret;
    }
    ret = fcfg_admin_check_response(join_conn, &resp_info,
            fcfg_context->network_timeout, FCFG_PROTO_ACK);
    if (ret) {
        logError("file: "__FILE__", line: %d "
                "set config fail. error info: %s",
                __LINE__, resp_info.error.message);
    }

    return ret;
}

int fcfg_admin_config_set (struct fcfg_context *fcfg_context,
        const char *env, const char *config_name, const char *config_value,
        const unsigned char type)
{
    int ret;
    ret = fcfg_admin_check_arg(env, config_name, config_value);
    if (ret == 0) {
        ret = fcfg_admin_set_config(fcfg_context, env, config_name,
                config_value, type);
    }

    return ret;
}

void fcfg_set_admin_del_config(char *buff, const char *env,
        const char *config_name, int *body_len)
{
    FCFGProtoDelConfigReq *del_config_req = (FCFGProtoDelConfigReq *)buff;
    unsigned char env_len = strlen(env);
    unsigned char name_len = strlen(config_name);
    
    del_config_req->env_len = env_len;
    del_config_req->name_len = name_len;
    memcpy(del_config_req->env,
           env,
           env_len);
    memcpy(del_config_req->env + env_len, config_name,
           name_len);
    *body_len = sizeof(FCFGProtoDelConfigReq) + env_len + name_len;
}

int fcfg_admin_del_config (struct fcfg_context *fcfg_context,
        const char *env, const char *config_name)
{
    int ret;
    char buff[64 + FCFG_CONFIG_ENV_SIZE + FCFG_CONFIG_NAME_SIZE];
    int body_len;
    int size;
    FCFGResponseInfo resp_info;
    FCFGProtoHeader *fcfg_header_proto;
    ConnectionInfo *join_conn;
    join_conn = fcfg_context->join_conn + fcfg_context->join_index;

    fcfg_header_proto = (FCFGProtoHeader *)buff;
    fcfg_set_admin_del_config(buff + sizeof(FCFGProtoHeader), env, config_name, &body_len);
    fcfg_set_admin_header(fcfg_header_proto, FCFG_PROTO_DEL_CONFIG_REQ, body_len);
    size = sizeof(FCFGProtoHeader) + body_len;
    ret = send_and_recv_response_header(join_conn, buff, size, &resp_info,
            fcfg_context->network_timeout);
    if (ret) {
        logError("file: "__FILE__", line: %d "
                "send_and_recv_response_header fail. ret:%d, %s",
                __LINE__, ret, strerror(ret));
        return ret;
    }
    ret = fcfg_admin_check_response(join_conn, &resp_info,
            fcfg_context->network_timeout, FCFG_PROTO_ACK);
    if (ret) {
        logError("file: "__FILE__", line: %d "
                "del config fail. error info: %s",
                __LINE__, resp_info.error.message);
    }

    return ret;
}

int fcfg_admin_config_del (struct fcfg_context *fcfg_context,
        const char *env, const char *config_name)
{
    int ret;

    ret = fcfg_admin_check_arg(env, config_name, NULL);
    if (ret == 0) {
        ret = fcfg_admin_del_config(fcfg_context, env, config_name);
    }

    return ret;
}
void fcfg_set_admin_get_config(char *buff, const char *env,
        const char *config_name, int *body_len)
{
    FCFGProtoGetConfigReq *get_config_req = (FCFGProtoGetConfigReq *)buff;
    unsigned char env_len = strlen(env);
    unsigned char name_len = strlen(config_name);
    get_config_req->env_len = env_len;
    get_config_req->name_len = name_len;
    memcpy(get_config_req->env,
           env,
           env_len);
    memcpy(get_config_req->env + env_len, config_name,
           name_len);
    *body_len = sizeof(FCFGProtoGetConfigReq) + env_len + name_len;
}
static int _extract_to_array (char *buff, int len, FCFGConfigArray *array,
        int offset, int count)
{
    int config_size;
    int size;
    int index;
    FCFGConfigEntry *rows;
    int ret = 0;

    size = offset;
    rows = array->rows + array->count;
    for (index = 0; index < count; index ++) {
        ret = fcfg_admin_config_set_entry((FCFGProtoListConfigRespBodyPart *)(buff + size),
                rows + index, &config_size);
        if (ret) {
            break;
        }
        size += config_size;
        array->count ++;
    }
    if (ret || (size != len)) {
        logError("file: "__FILE__", line: %d "
                "fcfg_admin_config_set_entry fail"
                " ret:%d, count:%d, size:%d, len:%d ", __LINE__, ret, count, size, len);
        return -1;
    }

    return ret;
}
static int fcfg_admin_extract_to_array (char *buff, int len, FCFGConfigArray *array)
{
    array->rows = (FCFGConfigEntry *)malloc(sizeof(FCFGConfigEntry));
    if (array->rows == NULL) {
        logError("file: "__FILE__", line: %d, "
                "malloc %ld bytes fail", __LINE__, sizeof(FCFGConfigEntry));
        return ENOMEM;
    }

    return _extract_to_array(buff, len, array, 0, 1);
}

static int fcfg_admin_extract_list_to_array (char *buff, int len, FCFGConfigArray *array,
        int *resp_count)
{
    int size;
    short count;
    FCFGConfigEntry *tmp;
    FCFGProtoListConfigRespHeader *list_config_resp_header_proto =
        (FCFGProtoListConfigRespHeader *)buff;
    count = buff2short(list_config_resp_header_proto->count);
    *resp_count = count;
    if (count <= 0) {
        return 0;
    }

    size = sizeof(FCFGConfigEntry) * (count + array->count);
    tmp = (FCFGConfigEntry *)malloc(size);
    if (tmp == NULL) {
        logError("file: "__FILE__", line: %d, "
                "malloc %d bytes fail", __LINE__, size);
        return ENOMEM;
    }
    memset(tmp, 0, size);
    if (array->count && array->rows) {
        memcpy(tmp, array->rows, array->count * sizeof(FCFGConfigEntry));
        free(array->rows);
        array->rows = NULL;
    }
    array->rows = tmp;

    return _extract_to_array(buff, len, array,
            sizeof(FCFGProtoListConfigRespHeader), count);
}


int fcfg_admin_config_response(ConnectionInfo *join_conn,
        FCFGResponseInfo *resp_info, int network_timeout,
        FCFGConfigArray *array, int *resp_count, int is_list)
{
    char *buff;
    int ret;
    if (resp_info->body_len == 0) {
        return -1;
    }
    buff = (char *)malloc(resp_info->body_len);
    if (buff == NULL) {
        logError("file: "__FILE__", line: %d "
                "malloc fail %d ", __LINE__, resp_info->body_len);
        return ENOMEM;
    }
    ret = tcprecvdata_nb_ex(join_conn->sock, buff,
            resp_info->body_len, network_timeout, NULL);
    if (ret) {
        logError("file: "__FILE__", line: %d "
                "tcprecvdata_nb_ex fail %d ", __LINE__, resp_info->body_len);
        free(buff);
        return -1;
    }

    if (is_list) {
        ret = fcfg_admin_extract_list_to_array(buff, resp_info->body_len, array, resp_count);
    } else {
        ret = fcfg_admin_extract_to_array(buff, resp_info->body_len, array);
    }
    free(buff);
    return ret;
}

int fcfg_admin_get_config (struct fcfg_context *fcfg_context,
        const char *env, const char *config_name, FCFGConfigArray *array)
{
    int ret;
    char buff[64 + FCFG_CONFIG_ENV_SIZE + FCFG_CONFIG_NAME_SIZE];
    int body_len;
    int size;
    FCFGResponseInfo resp_info;
    FCFGProtoHeader *fcfg_header_proto;

    ConnectionInfo *join_conn;
    join_conn = fcfg_context->join_conn + fcfg_context->join_index;
    fcfg_header_proto = (FCFGProtoHeader *)buff;
    fcfg_set_admin_get_config(buff + sizeof(FCFGProtoHeader), env, config_name, &body_len);
    fcfg_set_admin_header(fcfg_header_proto, FCFG_PROTO_GET_CONFIG_REQ, body_len);
    size = sizeof(FCFGProtoHeader) + body_len;
    ret = send_and_recv_response_header(join_conn, buff, size, &resp_info,
            fcfg_context->network_timeout);
    if (ret) {
        logError("file: "__FILE__", line: %d "
                "send_and_recv_response_header fail. ret:%d, %s",
                __LINE__, ret, strerror(ret));
        return ret;
    }
    ret = fcfg_admin_check_response(join_conn, &resp_info,
            fcfg_context->network_timeout, FCFG_PROTO_GET_CONFIG_RESP);
    if (ret) {
        logError("file: "__FILE__", line: %d "
                "get config fail. error info: %s",
                __LINE__, resp_info.error.message);
    } else {
        ret = fcfg_admin_config_response(join_conn, &resp_info,
                fcfg_context->network_timeout, array, NULL, 0);
    }

    return ret;
}

int fcfg_admin_config_get (struct fcfg_context *fcfg_context,
        const char *env, const char *config_name, FCFGConfigArray *array)
{
    int ret;
    memset(array, 0, sizeof(FCFGConfigArray));

    ret = fcfg_admin_check_arg(env, config_name, NULL);
    if (ret == 0) {
        ret = fcfg_admin_get_config(fcfg_context, env, config_name, array);
    }
    return ret;
}
void fcfg_set_admin_list_config(char *buff, const char *env,
        const char *config_name,
        int *body_len, int offset, int count)
{
    FCFGProtoListConfigReq *list_config_req = (FCFGProtoListConfigReq *)buff;
    unsigned char env_len = strlen(env);
    unsigned char name_len = 0;

    if (config_name) {
        name_len = strlen(config_name);
    }

    list_config_req->env_len = env_len;
    list_config_req->name_len = name_len;
    short2buff(offset, list_config_req->limit.offset);
    short2buff(count, list_config_req->limit.count);

    memcpy(list_config_req->env, env,
           env_len);
    if (name_len) {
        memcpy(list_config_req->env + env_len,
                config_name,
                name_len);
    }

    *body_len = env_len + name_len + sizeof(FCFGProtoListConfigReq);
}

int fcfg_admin_list_config (struct fcfg_context *fcfg_context,
        const char *env, const char *config_name, int limit, FCFGConfigArray *array)
{
    int ret;
    char buff[64 + FCFG_CONFIG_ENV_SIZE + FCFG_CONFIG_NAME_SIZE];
    int body_len;
    int size;
    int offset;
    int count;
    int resp_count;
    int left_count;
    FCFGResponseInfo resp_info;
    FCFGProtoHeader *fcfg_header_proto;
    ConnectionInfo *join_conn;

    offset = 0;
    count = FCFG_ADMIN_LIST_REQUEST_COUNT;
    left_count = limit;
    join_conn = fcfg_context->join_conn + fcfg_context->join_index;
    while (left_count > 0 || (limit == 0)) {
        if (limit != 0) {
            count = (left_count > count) ? count : left_count;
        }
        fcfg_header_proto = (FCFGProtoHeader *)buff;
        fcfg_set_admin_list_config(buff + sizeof(FCFGProtoHeader), env,
                config_name, &body_len,
                offset, count);
        fcfg_set_admin_header(fcfg_header_proto, FCFG_PROTO_LIST_CONFIG_REQ, body_len);
        size = sizeof(FCFGProtoHeader) + body_len;
        ret = send_and_recv_response_header(join_conn, buff, size, &resp_info,
                fcfg_context->network_timeout);
        if (ret) {
            logError("file: "__FILE__", line: %d "
                    "send_and_recv_response_header fail. ret:%d, %s",
                    __LINE__, ret, strerror(ret));
            return ret;
        }
        ret = fcfg_admin_check_response(join_conn,
                &resp_info, fcfg_context->network_timeout,
                FCFG_PROTO_LIST_CONFIG_RESP);
        if (ret) {
            logError("file: "__FILE__", line: %d "
                    "list config fail. error info: %.*s",
                    __LINE__, resp_info.body_len, resp_info.error.message);
            break;
        } else {
            ret = fcfg_admin_config_response(join_conn, &resp_info,
                    fcfg_context->network_timeout, array, &resp_count, 1);
            if (ret) {
                logError("file: "__FILE__", line: %d "
                        "fcfg_admin_config_response fail", __LINE__);
                break;
            }
            if (resp_count == 0) {
                break;
            }
        }

        offset += resp_count;
        if (limit != 0) {
            left_count -= resp_count;
        }
    }

    return ret;
}

int fcfg_admin_config_list (struct fcfg_context *fcfg_context,
        const char *env, const char *config_name, const int limit, FCFGConfigArray *array)
{
    int ret;
    memset(array, 0, sizeof(FCFGConfigArray));

    ret = fcfg_admin_check_arg(env, config_name, NULL);
    if (ret == 0) {
        ret = fcfg_admin_list_config(fcfg_context, env, config_name, limit, array);
    }

    return ret;
}
void fcfg_print_config_array (FCFGConfigArray *array, int print_detail)
{
    int i;

    for (i = 0; i < array->count; i++) {
        fprintf(stderr,
                "%s = %s\n",
                (array->rows + i)->name.str,
                (array->rows + i)->value.str);
        if (print_detail) {
            char create_time_buff[32];
            char update_time_buff[32];
            strftime(create_time_buff, sizeof(create_time_buff),
                    "%Y-%m-%d %H:%M:%S",
                    localtime(&((array->rows +i)->create_time)));
            strftime(update_time_buff, sizeof(update_time_buff),
                    "%Y-%m-%d %H:%M:%S",
                    localtime(&((array->rows + i)->update_time)));
            fprintf(stderr,
                    "version: %"PRId64"\n"
                    "type: %s\ncreate time: %s\nupdate time: %s\n",
                    (array->rows + i)->version,
                    fcfg_config_type_string[(array->rows + i)->type],
                    create_time_buff, update_time_buff);
        }
    }
}

void fcfg_free_config_info_array(FCFGConfigArray *array)
{
    FCFGConfigEntry *current;
    FCFGConfigEntry *end;

    if (array->rows == NULL) {
        return;
    }

    end = array->rows + array->count;
    for (current=array->rows; current<end; current++) {
        if (current->name.str != NULL) {
            free(current->name.str);
        }
    }

    free(array->rows);
    array->rows = NULL;
    array->count = 0;
}
