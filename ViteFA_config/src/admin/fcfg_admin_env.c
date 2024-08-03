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

void fcfg_set_admin_add_env(char *buff, const char *env,
        int *body_len)
{
    FCFGProtoAddEnvReq *add_env_req = (FCFGProtoAddEnvReq *)buff;
    unsigned char env_len = strlen(env);
    memcpy(add_env_req->env, env,
           env_len);
    *body_len = sizeof(FCFGProtoAddEnvReq) + env_len;
}
int fcfg_admin_add_env (struct fcfg_context *fcfg_context, const char *env)
{
    int ret;
    char buff[64 + FCFG_CONFIG_ENV_SIZE];
    int body_len;
    int size;
    FCFGResponseInfo resp_info;
    ConnectionInfo *join_conn;
    FCFGProtoHeader *fcfg_header_proto;

    join_conn = fcfg_context->join_conn + fcfg_context->join_index;
    fcfg_header_proto = (FCFGProtoHeader *)buff;
    fcfg_set_admin_add_env(buff + sizeof(FCFGProtoHeader), env, &body_len);
    fcfg_set_admin_header(fcfg_header_proto, FCFG_PROTO_ADD_ENV_REQ, body_len);
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
                "add env fail. error info: %s",
                __LINE__, resp_info.error.message);
    }

    return ret;
}

int fcfg_admin_env_add (struct fcfg_context *fcfg_context, const char *env)
{
    int ret;

    ret = fcfg_admin_check_arg(env, NULL, NULL);
    if (ret == 0) {
        ret = fcfg_admin_add_env(fcfg_context, env);
    }
    return ret;
}

void fcfg_set_admin_del_env(char *buff, const char *env,
        int *body_len)
{
    FCFGProtoDelEnvReq *del_env_req = (FCFGProtoDelEnvReq *)buff;
    unsigned char env_len = strlen(env);
    memcpy(del_env_req->env, env,
           env_len);
    *body_len = sizeof(FCFGProtoDelEnvReq) + env_len;
}

int fcfg_admin_del_env (struct fcfg_context *fcfg_context, const char *env)
{
    int ret;
    char buff[64 + FCFG_CONFIG_ENV_SIZE];
    int body_len;
    int size;
    FCFGResponseInfo resp_info;
    ConnectionInfo *join_conn;
    FCFGProtoHeader *fcfg_header_proto;

    fcfg_header_proto = (FCFGProtoHeader *)buff;
    join_conn = fcfg_context->join_conn + fcfg_context->join_index;
    fcfg_set_admin_del_env(buff + sizeof(FCFGProtoHeader), env, &body_len);
    fcfg_set_admin_header(fcfg_header_proto, FCFG_PROTO_DEL_ENV_REQ, body_len);
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
                "del env fail. error info: %s",
                __LINE__, resp_info.error.message);
    }

    return ret;
}

int fcfg_admin_env_del (struct fcfg_context *fcfg_context, const char *env)
{
    int ret;

    ret = fcfg_admin_check_arg(env, NULL, NULL);
    if (ret == 0) {
        ret = fcfg_admin_del_env(fcfg_context, env);
    }
    return ret;
}

void fcfg_set_admin_get_env(const char *env, char *buff,
        int *body_len)
{
    FCFGProtoGetEnvReq *get_env_req = (FCFGProtoGetEnvReq *)buff;
    unsigned char env_len = strlen(env);
    memcpy(get_env_req->env, env,
           env_len);
    *body_len = sizeof(FCFGProtoGetEnvReq) + env_len;
}

static int _extract_to_array(char *buff, int len, FCFGEnvArray *array,
        int offset, int count)
{
    int env_size;
    int size;
    int index;
    int ret = 0;

    size = offset;
    for (index = 0; index < count; index ++) {
        ret = fcfg_admin_env_set_entry(
                (FCFGProtoListEnvRespBodyPart *)(buff + size),
                array->rows + index,
                &env_size);
        if (ret) {
            break;
        }
        size += env_size;
        array->count ++;
    }
    if (ret || (size != len)) {
        logError("file: "__FILE__", line: %d, "
                "_extract_to_array fail ret:%d, count:%d, size: %d, len: %d",
                __LINE__, ret, count, size, len);
        return -1;
    }

    return ret;
}

static int fcfg_admin_extract_to_array (char *buff, int len, FCFGEnvArray *array)
{
    array->rows = (FCFGEnvEntry *)malloc(sizeof(FCFGEnvEntry));
    if (array->rows == NULL) {
        logError("file: "__FILE__", line: %d, "
                "malloc %ld bytes fail", __LINE__, sizeof(FCFGEnvEntry));
        fcfg_free_env_info_array(array);
        return ENOMEM;
    }
    return _extract_to_array(buff, len, array, 0, 1);
}

static int fcfg_admin_extract_list_to_array (char *buff, int len, FCFGEnvArray *array)
{
    short count;
    FCFGProtoListEnvRespHeader *list_env_resp_header_proto;

    list_env_resp_header_proto = (FCFGProtoListEnvRespHeader *)buff;
    count = buff2short(list_env_resp_header_proto->count);

    array->rows = (FCFGEnvEntry *)malloc(sizeof(FCFGEnvEntry) * count);
    if (array->rows == NULL) {
        logError("file: "__FILE__", line: %d, "
                "malloc %ld bytes fail", __LINE__, sizeof(FCFGEnvEntry));
        return ENOMEM;
    }
    memset(array->rows, 0, sizeof(FCFGEnvEntry) * count);
    return _extract_to_array(buff, len, array, sizeof(FCFGProtoListEnvRespHeader), count);
}

int fcfg_admin_env_response(ConnectionInfo *join_conn,
        FCFGResponseInfo *resp_info, int network_timeout,
        FCFGEnvArray *array, int is_list)
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
        ret = fcfg_admin_extract_list_to_array(buff, resp_info->body_len, array);
    } else {
        ret = fcfg_admin_extract_to_array(buff, resp_info->body_len, array);
    }
    free(buff);
    return ret;
}

int fcfg_admin_get_env (struct fcfg_context *fcfg_context, const char *env, FCFGEnvArray *array)
{
    int ret;
    char buff[64 + FCFG_CONFIG_ENV_SIZE];
    int body_len;
    int size;
    ConnectionInfo *join_conn;
    FCFGResponseInfo resp_info;
    FCFGProtoHeader *fcfg_header_proto;

    fcfg_header_proto = (FCFGProtoHeader *)buff;
    fcfg_set_admin_get_env(env, buff + sizeof(FCFGProtoHeader), &body_len);
    join_conn = fcfg_context->join_conn + fcfg_context->join_index;
    fcfg_set_admin_header(fcfg_header_proto, FCFG_PROTO_GET_ENV_REQ, body_len);
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
            &resp_info,
            fcfg_context->network_timeout, FCFG_PROTO_GET_ENV_RESP);
    if (ret) {
        logError("file: "__FILE__", line: %d "
                "get env fail. error info: %s",
                __LINE__,
                resp_info.error.message);
    } else {
        ret = fcfg_admin_env_response(join_conn, &resp_info,
                fcfg_context->network_timeout, array, 0);
    }

    return ret;
}

int fcfg_admin_env_get (struct fcfg_context *fcfg_context, const char *env,
        FCFGEnvArray *array)
{
    int ret;
    memset(array, 0, sizeof(FCFGEnvArray));

    ret = fcfg_admin_check_arg(env, NULL, NULL);
    if (ret == 0) {
        ret = fcfg_admin_get_env(fcfg_context, env, array);
    }

    log_destroy();
    return ret;
}

int fcfg_admin_list_env (struct fcfg_context *fcfg_context, FCFGEnvArray *array)
{
    int ret;
    char buff[64];
    int body_len;
    int size;
    FCFGResponseInfo resp_info;
    FCFGProtoHeader *fcfg_header_proto;
    ConnectionInfo *join_conn;
    fcfg_header_proto = (FCFGProtoHeader *)buff;

    join_conn = fcfg_context->join_conn + fcfg_context->join_index;
    body_len = 0;
    fcfg_set_admin_header(fcfg_header_proto, FCFG_PROTO_LIST_ENV_REQ, body_len);
    size = sizeof(FCFGProtoHeader) + body_len;
    ret = send_and_recv_response_header(join_conn, buff, size, &resp_info,
            fcfg_context->network_timeout);
    if (ret) {
        logError("file: "__FILE__", line: %d "
                "send_and_recv_response_header fail. ret:%d, %s",
                __LINE__,
                ret, strerror(ret));
        return ret;
    }
    ret = fcfg_admin_check_response(join_conn,
            &resp_info, fcfg_context->network_timeout, FCFG_PROTO_LIST_ENV_RESP);
    if (ret) {
        logError("file: "__FILE__", line: %d, "
                "list env fail. error info: %s",
                __LINE__,
                resp_info.error.message);
    } else {
        ret = fcfg_admin_env_response(join_conn, &resp_info,
                fcfg_context->network_timeout, array, 1);
        if (ret) {
            logError("file: "__FILE__", line: %d, "
                    "fcfg_admin_env_response fail", __LINE__);
        }
    }

    return ret;
}


int fcfg_admin_env_list (struct fcfg_context *fcfg_context,
        FCFGEnvArray *array)
{
    int ret;
    memset(array, 0, sizeof(FCFGEnvArray));

    ret = fcfg_admin_list_env(fcfg_context, array);
    return ret;
}

void fcfg_print_env_array (FCFGEnvArray *array)
{
    int i;

    for (i = 0; i < array->count; i++) {
        fprintf(stderr, "%s\n", (array->rows+i)->env.str);
    }
}
void fcfg_free_env_info_array(FCFGEnvArray *array)
{
    FCFGEnvEntry *current;
    FCFGEnvEntry *end;

    if (array->rows == NULL) {
        return;
    }

    end = array->rows + array->count;
    for (current=array->rows; current<end; current++) {
        if (current->env.str != NULL) {
            free(current->env.str);
        }
    }

    free(array->rows);
    array->rows = NULL;
    array->count = 0;
}

