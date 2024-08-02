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

