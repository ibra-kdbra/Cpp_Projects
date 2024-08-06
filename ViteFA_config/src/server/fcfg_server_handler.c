//fcfg_server_handler.c

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "fastcommon/logger.h"
#include "fastcommon/sockopt.h"
#include "fastcommon/shared_func.h"
#include "fastcommon/pthread_func.h"
#include "fastcommon/sched_thread.h"
#include "fastcommon/ioevent_loop.h"
#include "fastcommon/json_parser.h"
#include "sf/sf_util.h"
#include "sf/sf_func.h"
#include "sf/sf_proto.h"
#include "sf/sf_nio.h"
#include "sf/sf_global.h"
#include "common/fcfg_proto.h"
#include "fcfg_server_types.h"
#include "fcfg_server_global.h"
#include "fcfg_server_func.h"
#include "fcfg_server_dao.h"
#include "fcfg_server_cfg.h"
#include "fcfg_server_push.h"
#include "fcfg_server_handler.h"

int fcfg_server_handler_init()
{
    return 0;
}

int fcfg_server_handler_destroy()
{   
    return 0;
}

void fcfg_server_task_finish_cleanup(struct fast_task_info *task)
{
    FCFGServerTaskArg *task_arg;

    task_arg = (FCFGServerTaskArg *)task->arg;

    fcfg_server_cfg_remove_subscriber(task);
    task_arg->msg_queue.config_array = NULL;
    task_arg->msg_queue.agent_cfg_version = 0;
    task_arg->msg_queue.offset = 0;
    task_arg->waiting_type = FCFG_SERVER_TASK_WAITING_REQUEST;
    task_arg->joined = false;

    __sync_add_and_fetch(&((FCFGServerTaskArg *)task->arg)->task_version, 1);
    sf_task_finish_clean_up(task);
}

int fcfg_server_recv_timeout_callback(struct fast_task_info *task)
{
    FCFGServerTaskArg *task_arg;
    task_arg = (FCFGServerTaskArg *)task->arg;
    if ((task_arg->waiting_type & FCFG_SERVER_TASK_WAITING_RESP) != 0) {
        logWarning("file: "__FILE__", line: %d, "
                "client ip: %s, waiting type: %d, "
                "recv timeout", __LINE__, task->client_ip,
                task_arg->waiting_type);
        return ETIMEDOUT;
    }

    if (g_current_time - task_arg->last_recv_pkg_time >=
            g_server_global_vars.check_alive_interval)
    {
        return fcfg_server_add_task_event(task, FCFG_SERVER_EVENT_TYPE_ACTIVE_TEST);
    }

    return 0;
}

static int fcfg_proto_deal_agent_join(struct fast_task_info *task,
        const FCFGRequestInfo *request, FCFGResponseInfo *response)
{
    FCFGProtoAgentJoinReq *join_req;
    FCFGProtoAgentJoinResp *join_resp;
    char env[FCFG_CONFIG_ENV_SIZE];
    int result;
    int64_t agent_cfg_version;
    int64_t center_cfg_version;

    if ((result=FCFG_PROTO_EXPECT_BODY_LEN(task, request, response,
                    sizeof(FCFGProtoAgentJoinReq))) != 0)
    {
        return result;
    }

    memset(env, 0, sizeof(env));
    join_req = (FCFGProtoAgentJoinReq *)(task->send.ptr->data + sizeof(FCFGProtoHeader));
    memcpy(env, join_req->env, sizeof(join_req->env));
    if (!fcfg_server_env_exists(env)) {
        response->error.length = sprintf(response->error.message,
                "env: %s not exist", env);
        return ENOENT;
    }

    if ((result=fcfg_server_cfg_add_subscriber(env, task)) != 0) {
        return result;
    }

    agent_cfg_version = buff2long(join_req->agent_cfg_version);
    center_cfg_version = ((FCFGServerTaskArg *)task->arg)->publisher->current_version;
    if (agent_cfg_version > center_cfg_version) {
        logWarning("file: "__FILE__", line: %d, client ip: %s, "
                "agent_cfg_version: %"PRId64" > center_cfg_version: %"PRId64,
                __LINE__, task->client_ip, agent_cfg_version, center_cfg_version);
    } else if (agent_cfg_version < center_cfg_version) {
        result = fcfg_server_add_config_push_event(task);
    }

    logDebug("file: "__FILE__", line: %d, client ip: %s, env: %s, "
            "agent_cfg_version: %"PRId64", center_cfg_version: %"PRId64,
            __LINE__, task->client_ip, env, agent_cfg_version, center_cfg_version);

    ((FCFGServerTaskArg *)task->arg)->msg_queue.agent_cfg_version = agent_cfg_version;
    join_resp = (FCFGProtoAgentJoinResp *)(task->send.ptr->data + sizeof(FCFGProtoHeader));
    long2buff(center_cfg_version, join_resp->center_cfg_version);

    ((FCFGServerTaskArg *)task->arg)->joined = true;
    response->body_len = 8;
    response->cmd = FCFG_PROTO_AGENT_JOIN_RESP;
    response->response_done = true;
    return result;
}

static int fcfg_proto_deal_admin_join(struct fast_task_info *task,
        const FCFGRequestInfo *request, FCFGResponseInfo *response)
{
    FCFGProtoAdminJoinReq *join_req;
    string_t username;
    string_t secret_key;
    int expect_len;
    int result;

    if ((result=FCFG_PROTO_CHECK_BODY_LEN(task, request, response,
                    sizeof(FCFGProtoAdminJoinReq), 256)) != 0)
    {
        return result;
    }

    join_req = (FCFGProtoAdminJoinReq *)(task->send.ptr->data + sizeof(FCFGProtoHeader));
    username.len = join_req->username_len;
    secret_key.len = join_req->secret_key_len;
    username.str = join_req->username;
    secret_key.str = join_req->username + username.len;

    expect_len = sizeof(FCFGProtoAdminJoinReq) + username.len + secret_key.len;
    if (request->body_len != expect_len) {
        response->error.length = sprintf(response->error.message,
                "invalid body length: %d, expect length: %d",
                request->body_len, expect_len);
        return EINVAL;
    }

    if (!(fc_compare_string(&username, &g_server_global_vars.admin.username) == 0 &&
            fc_compare_string(&secret_key, &g_server_global_vars.admin.secret_key) == 0))
    {
        response->error.length = sprintf(response->error.message,
                "invalid username or secret_key");

        logError("file: "__FILE__", line: %d, "
                "client ip: %s, cmd: %d, %s. "
                "username: %.*s, secret_key: %.*s",
                __LINE__, task->client_ip, request->cmd,
                response->error.message, username.len, username.str,
                secret_key.len, secret_key.str);
        response->log_error = false;
        return EINVAL;
    }

    ((FCFGServerTaskArg *)task->arg)->joined = true;
    return result;
}


