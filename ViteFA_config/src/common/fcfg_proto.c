
#include <errno.h>
#include "fastcommon/shared_func.h"
#include "fastcommon/connection_pool.h"
#include "fastcommon/ini_file_reader.h"
#include "fcfg_proto.h"
#include "fcfg_types.h"
#include "fastcommon/sockopt.h"

void fcfg_proto_init()
{
}

int fcfg_proto_set_body_length(struct fast_task_info *task)
{
    task->send.ptr->length = buff2int(((FCFGProtoHeader *)task->send.ptr->data)->body_len);
    return 0;
}

int fcfg_proto_deal_actvie_test(struct fast_task_info *task,
        const FCFGRequestInfo *request, FCFGResponseInfo *response)
{
    return FCFG_PROTO_EXPECT_BODY_LEN(task, request, response, 0);
}

void fcfg_proto_response_extract (FCFGProtoHeader *header_pro,
        FCFGResponseInfo *resp_info)
{
    resp_info->cmd      = header_pro->cmd;
    resp_info->body_len = buff2int(header_pro->body_len);
    resp_info->status   = header_pro->status;
}

void fcfg_set_admin_header (FCFGProtoHeader *fcfg_header_proto,
        unsigned char cmd, int body_len)
{
    fcfg_header_proto->cmd = cmd;
    int2buff(body_len, fcfg_header_proto->body_len);
}

int fcfg_check_response(ConnectionInfo *join_conn,
        FCFGResponseInfo *resp_info, int network_timeout, unsigned char resp_cmd)
{
    if (resp_info->cmd == resp_cmd && resp_info->status == 0) {
        return 0;
    } else {
        if (resp_info->body_len) {
            tcprecvdata_nb_ex(join_conn->sock, resp_info->error.message,
                    resp_info->body_len, network_timeout, NULL);
            resp_info->error.message[resp_info->body_len] = '\0';
        } else {
            resp_info->error.message[0] = '\0';
        }
        return 1;
    }

}

int send_and_recv_response_header(ConnectionInfo *conn, char *data, int len,
        FCFGResponseInfo *resp_info, int network_timeout)
{
    int ret;
    FCFGProtoHeader fcfg_header_resp_pro;

    if ((ret = tcpsenddata_nb(conn->sock, data,
            len, network_timeout)) != 0) {
        return ret;
    }
    if ((ret = tcprecvdata_nb_ex(conn->sock, &fcfg_header_resp_pro,
            sizeof(FCFGProtoHeader), network_timeout, NULL)) != 0) {
        return ret;
    }
    fcfg_proto_response_extract(&fcfg_header_resp_pro, resp_info);
    return 0;
}

int fcfg_send_active_test_req(ConnectionInfo *conn, FCFGResponseInfo *resp_info,
        int network_timeout)
{
    int ret;
    FCFGProtoHeader fcfg_header_proto;

    fcfg_set_admin_header(&fcfg_header_proto, FCFG_PROTO_ACTIVE_TEST_REQ,
            0);
    ret = send_and_recv_response_header(conn, (char *)&fcfg_header_proto,
            sizeof(FCFGProtoHeader), resp_info, network_timeout);
    if (ret == 0) {
        ret = fcfg_check_response(conn, resp_info, network_timeout,
                FCFG_PROTO_ACTIVE_TEST_RESP);
    }

    return ret;
}
