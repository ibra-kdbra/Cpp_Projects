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

static int fcfg_proto_deal_add_del_env(struct fast_task_info *task,
        const FCFGRequestInfo *request, FCFGResponseInfo *response)
{
    FCFGMySQLContext *mysql_context;
    char *env;
    int result;

    if ((result=FCFG_PROTO_CHECK_BODY_LEN(task, request, response,
                    1, FCFG_CONFIG_ENV_SIZE - 1)) != 0)
    {
        return result;
    }

    mysql_context = &((FCFGServerContext *)task->thread_data->arg)->mysql_context;
    env = task->send.ptr->data + sizeof(FCFGProtoHeader);
    *(env + request->body_len) = '\0';
    if (request->cmd == FCFG_PROTO_ADD_ENV_REQ) {
        result = fcfg_server_dao_add_env(mysql_context, env);
        if (result == EEXIST) {
            response->error.length = sprintf(response->error.message,
                    "env: %s already exist", env);
        }
    } else {
        result = fcfg_server_dao_del_env(mysql_context, env);
        if (result == ENOENT) {
            response->error.length = sprintf(response->error.message,
                    "env: %s not exist", env);
        }
    }
    return result;
}

static int fcfg_proto_deal_get_env(struct fast_task_info *task,
        const FCFGRequestInfo *request, FCFGResponseInfo *response)
{
    FCFGMySQLContext *mysql_context;
    FCFGProtoGetEnvResp *env_resp;
    FCFGEnvEntry entry;
    char env[FCFG_CONFIG_ENV_SIZE];
    int result;

    if ((result=FCFG_PROTO_CHECK_BODY_LEN(task, request, response,
                    1, FCFG_CONFIG_ENV_SIZE - 1)) != 0)
    {
        return result;
    }

    mysql_context = &((FCFGServerContext *)task->thread_data->arg)->mysql_context;
    memcpy(env, task->send.ptr->data + sizeof(FCFGProtoHeader), request->body_len);
    *(env + request->body_len) = '\0';
    if ((result=fcfg_server_dao_get_env(mysql_context, env, &entry)) != 0) {
        return result;
    }

    env_resp = (FCFGProtoGetEnvResp *)(task->send.ptr->data + sizeof(FCFGProtoHeader));
    env_resp->env_len = entry.env.len;
    int2buff(entry.create_time, env_resp->create_time);
    int2buff(entry.update_time, env_resp->update_time);
    memcpy(env_resp->env, entry.env.str, entry.env.len);

    response->body_len = sizeof(FCFGProtoGetEnvResp) + entry.env.len;
    response->cmd = FCFG_PROTO_GET_ENV_RESP;
    response->response_done = true;
    return 0;
}

static int fcfg_proto_deal_list_env(struct fast_task_info *task,
        const FCFGRequestInfo *request, FCFGResponseInfo *response)
{
    FCFGMySQLContext *mysql_context;
    FCFGProtoListEnvRespHeader *resp_header;
    FCFGProtoGetEnvResp *env_resp;
    char *p;
    FCFGEnvEntry *entry;
    FCFGEnvEntry *end;
    FCFGEnvArray array;
    int result;
    int expect_size;

    if ((result=FCFG_PROTO_EXPECT_BODY_LEN(task, request, response, 0)) != 0) {
        return result;
    }

    mysql_context = &((FCFGServerContext *)task->thread_data->arg)->mysql_context;
    if ((result=fcfg_server_dao_list_env(mysql_context, &array)) != 0) {
        return result;
    }

    end = array.rows + array.count;
    expect_size = sizeof(FCFGProtoHeader) + sizeof(FCFGProtoListEnvRespHeader);
    for (entry=array.rows; entry<end; entry++) {
        expect_size += sizeof(FCFGProtoGetEnvResp) + entry->env.len;
    }
    if (expect_size > task->send.ptr->size) {
        if ((result=sf_set_task_send_buffer_size(task, expect_size)) != 0) {
            fcfg_server_dao_free_env_array(&array);
            return result;
        }
    }

    p = task->send.ptr->data + sizeof(FCFGProtoHeader) + sizeof(FCFGProtoListEnvRespHeader);
    for (entry=array.rows; entry<end; entry++) {
        env_resp = (FCFGProtoGetEnvResp *)p;
        env_resp->env_len = entry->env.len;
        int2buff(entry->create_time, env_resp->create_time);
        int2buff(entry->update_time, env_resp->update_time);
        memcpy(env_resp->env, entry->env.str, entry->env.len);
        
        p += sizeof(FCFGProtoGetEnvResp) + entry->env.len;
    }

    resp_header = (FCFGProtoListEnvRespHeader *)(task->send.ptr->data + sizeof(FCFGProtoHeader));
    short2buff(array.count, resp_header->count);
    response->body_len = (p - task->send.ptr->data) - sizeof(FCFGProtoHeader);
    response->cmd = FCFG_PROTO_LIST_ENV_RESP;
    response->response_done = true;

    fcfg_server_dao_free_env_array(&array);
    return 0;
}

static int fcfg_proto_deal_set_config(struct fast_task_info *task,
        const FCFGRequestInfo *request, FCFGResponseInfo *response)
{
    FCFGMySQLContext *mysql_context;
    FCFGProtoSetConfigReq *set_config_req;
    char env[FCFG_CONFIG_ENV_SIZE];
    char name[FCFG_CONFIG_NAME_SIZE];
    int env_len;
    int name_len;
    short type;
    int data_body_len;
    string_t value;
    string_t new_value;
    const fc_json_array_t *array;
    const fc_json_map_t *map;
    BufferInfo buffer;
    int result;

    if ((result=FCFG_PROTO_CHECK_BODY_LEN(task, request, response,
                    sizeof(FCFGProtoSetConfigReq),
                    sizeof(FCFGProtoSetConfigReq) + FCFG_CONFIG_MAX_ENV_LEN +
                    FCFG_CONFIG_MAX_NAME_LEN + FCFG_CONFIG_MAX_VALUE_LEN)) != 0)
    {
        return result;
    }

    set_config_req = (FCFGProtoSetConfigReq *)(
            task->send.ptr->data + sizeof(FCFGProtoHeader));

    env_len = set_config_req->env_len;
    name_len = set_config_req->name_len;
    type = set_config_req->type;
    value.len = buff2int(set_config_req->value_len);

    if (env_len <= 0 || env_len > FCFG_CONFIG_MAX_ENV_LEN) {
        response->error.length = sprintf(response->error.message,
                "invalid env length: %d", env_len);
        return EINVAL;
    }

    if (name_len <= 0 || name_len  > FCFG_CONFIG_MAX_NAME_LEN) {
        response->error.length = sprintf(response->error.message,
                "invalid name length: %d", name_len);
        return EINVAL;
    }

    if (!(type == FCFG_CONFIG_TYPE_NONE || type == FCFG_CONFIG_TYPE_STRING ||
                type == FCFG_CONFIG_TYPE_LIST || type == FCFG_CONFIG_TYPE_MAP))
    {
        response->error.length = sprintf(response->error.message,
                "invalid type: %d", type);
        return EINVAL;
    }

    if (value.len < 0 || value.len > FCFG_CONFIG_MAX_VALUE_LEN) {
        response->error.length = sprintf(response->error.message,
                "invalid value length: %d", value.len);
        return EINVAL;
    }

    data_body_len = sizeof(FCFGProtoSetConfigReq) +
        env_len + name_len + value.len;
    if (request->body_len != data_body_len) {
        response->error.length = sprintf(response->error.message,
                "invalid body length: %d, expect: %d",
                request->body_len, data_body_len);
        return EINVAL;
    }

    memcpy(env, set_config_req->env, env_len);
    *(env + env_len) = '\0';
    if (!fcfg_server_env_exists(env)) {
        response->error.length = sprintf(response->error.message,
                "env: %s not exist", env);
        return ENOENT;
    }

    memset(&buffer, 0, sizeof(buffer));
    memcpy(name, set_config_req->env + env_len, name_len);
    *(name + name_len) = '\0';

    value.str = set_config_req->env + env_len + name_len;
    *(value.str + value.len) = '\0';
    if (type == FCFG_CONFIG_TYPE_NONE) {
        int json_type;
        json_type = fc_detect_json_type(&value);
        switch (json_type) {
            case FC_JSON_TYPE_STRING:
                type = FCFG_CONFIG_TYPE_STRING;
                break;
            case FC_JSON_TYPE_ARRAY:
                type = FCFG_CONFIG_TYPE_LIST;
                break;
            case FC_JSON_TYPE_MAP:
                type = FCFG_CONFIG_TYPE_MAP;
                break;
            default:
                response->error.length = sprintf(response->error.message,
                        "unkown json type: %d", json_type);
                return EINVAL;
        }
    }

    switch (type) {
        case FCFG_CONFIG_TYPE_LIST:
            if ((array=fc_decode_json_array(&SERVER_CTX->
                            json_ctx, &value)) != NULL)
            {
                result = fc_encode_json_array_ex(&SERVER_CTX->json_ctx,
                        array->elements, array->count, &buffer);
                if (result == 0) {
                    new_value.str = buffer.buff;
                    new_value.len = buffer.length;
                } else {
                    new_value = value;
                }
            } else {
                result = fc_json_parser_get_error_no(&SERVER_CTX->json_ctx);
                new_value = value;
            }

            if (result != 0) {
                response->error.length = snprintf(response->error.message,
                        sizeof(response->error.message),
                        "%s", fc_json_parser_get_error_info(
                            &SERVER_CTX->json_ctx)->str);
                return result;
            }

            break;
        case FCFG_CONFIG_TYPE_MAP:
            if ((map=fc_decode_json_map(&SERVER_CTX->
                            json_ctx, &value)) != NULL)
            {
                result = fc_encode_json_map_ex(&SERVER_CTX->json_ctx,
                        map->elements, map->count, &buffer);
                if (result == 0) {
                    new_value.str = buffer.buff;
                    new_value.len = buffer.length;
                } else {
                    new_value = value;
                }
            } else {
                result = fc_json_parser_get_error_no(&SERVER_CTX->json_ctx);
                new_value = value;
            }

            if (result != 0) {
                response->error.length = snprintf(response->error.message,
                        sizeof(response->error.message),
                        "%s", fc_json_parser_get_error_info(
                            &SERVER_CTX->json_ctx)->str);
                return result;
            }

            break;
	default:
            result = 0;
            new_value = value;
            break;
    }

    mysql_context = &SERVER_CTX->mysql_context;
    result = fcfg_server_dao_set_config(mysql_context, env, name,
            type, new_value.str);
    if (result != 0) {
        response->error.length = sprintf(response->error.message,
                "internal server error");
    }

    fc_free_buffer(&buffer);
    return result;
}

static int fcfg_proto_deal_get_config(struct fast_task_info *task,
        const FCFGRequestInfo *request, FCFGResponseInfo *response)
{
    FCFGMySQLContext *mysql_context;
    FCFGProtoGetConfigReq *get_config_req;
    FCFGConfigArray array;
    char env[FCFG_CONFIG_ENV_SIZE];
    char name[FCFG_CONFIG_NAME_SIZE];
    int env_len;
    int name_len;
    int expect_size;
    int data_body_len;
    FCFGProtoGetConfigResp *get_config_resp;
    int result;

    if ((result=FCFG_PROTO_CHECK_BODY_LEN(task, request, response,
                    sizeof(FCFGProtoGetConfigReq),
                    sizeof(FCFGProtoGetConfigReq) + FCFG_CONFIG_MAX_ENV_LEN +
                    FCFG_CONFIG_MAX_NAME_LEN)) != 0)
    {
        return result;
    }

    get_config_req = (FCFGProtoGetConfigReq *)(task->send.ptr->data + sizeof(FCFGProtoHeader));

    env_len = get_config_req->env_len;
    name_len = get_config_req->name_len;

    if (env_len <= 0 || env_len > FCFG_CONFIG_MAX_ENV_LEN) {
        response->error.length = sprintf(response->error.message,
                "invalid env length: %d", env_len);
        return EINVAL;
    }

    if (name_len <= 0 || name_len  > FCFG_CONFIG_MAX_NAME_LEN) {
        response->error.length = sprintf(response->error.message,
                "invalid name length: %d", name_len);
        return EINVAL;
    }

    data_body_len = sizeof(FCFGProtoGetConfigReq) + env_len + name_len;
    if (request->body_len != data_body_len) {
        response->error.length = sprintf(response->error.message,
                "invalid body length: %d, expect: %d",
                request->body_len, data_body_len);
        return EINVAL;
    }
    memcpy(env, get_config_req->env, env_len);
    *(env + env_len) = '\0';

    memcpy(name, get_config_req->env + env_len, name_len);
    *(name + name_len) = '\0';

    mysql_context = &((FCFGServerContext *)task->thread_data->arg)->mysql_context;
    result = fcfg_server_dao_get_config(mysql_context, env, name, &array);
    if (result != 0) {
        response->error.length = sprintf(response->error.message,
                "query config fail, errno: %d", result);
        return result;
    }

    if (array.count == 0) {
        response->error.length = sprintf(response->error.message,
                "config not exist");
        return ENOENT;
    }

    expect_size = sizeof(FCFGProtoHeader) + sizeof(FCFGProtoGetConfigResp)
        + array.rows->name.len + array.rows->value.len;
    if (expect_size > task->send.ptr->size) {
        if ((result=sf_set_task_send_buffer_size(task, expect_size)) != 0) {
            response->error.length = sprintf(response->error.message,
                    "response data is too large: %d", expect_size);
            fcfg_server_dao_free_config_array(&array);
            return result;
        }
    }

    get_config_resp = (FCFGProtoGetConfigResp *)(task->send.ptr->data + sizeof(FCFGProtoHeader));
    get_config_resp->status = array.rows->status;
    get_config_resp->name_len = array.rows->name.len;
    get_config_resp->type = array.rows->type;
    int2buff(array.rows->value.len, get_config_resp->value_len);
    long2buff(array.rows->version, get_config_resp->version);
    int2buff(array.rows->create_time, get_config_resp->create_time);
    int2buff(array.rows->update_time, get_config_resp->update_time);
    memcpy(get_config_resp->name, array.rows->name.str, array.rows->name.len);
    memcpy(get_config_resp->name + array.rows->name.len,
           array.rows->value.str, array.rows->value.len);

    response->body_len = sizeof(FCFGProtoGetConfigResp) +
        array.rows->name.len + array.rows->value.len;
    response->cmd = FCFG_PROTO_GET_CONFIG_RESP;
    response->response_done = true;

    fcfg_server_dao_free_config_array(&array);
    return 0;
}

static int fcfg_proto_deal_del_config(struct fast_task_info *task,
        const FCFGRequestInfo *request, FCFGResponseInfo *response)
{
    FCFGMySQLContext *mysql_context;
    FCFGProtoDelConfigReq *del_config_req;
    char env[FCFG_CONFIG_ENV_SIZE];
    char name[FCFG_CONFIG_NAME_SIZE];
    int env_len;
    int name_len;
    int data_body_len;
    int result;

    if ((result=FCFG_PROTO_CHECK_BODY_LEN(task, request, response,
                    sizeof(FCFGProtoDelConfigReq),
                    sizeof(FCFGProtoDelConfigReq) + FCFG_CONFIG_MAX_ENV_LEN +
                    FCFG_CONFIG_MAX_NAME_LEN)) != 0)
    {
        return result;
    }

    del_config_req = (FCFGProtoDelConfigReq *)(task->send.ptr->data + sizeof(FCFGProtoHeader));
    env_len = del_config_req->env_len;
    name_len = del_config_req->name_len;

    if (env_len <= 0 || env_len > FCFG_CONFIG_MAX_ENV_LEN) {
        response->error.length = sprintf(response->error.message,
                "invalid env length: %d", env_len);
        return EINVAL;
    }

    if (name_len <= 0 || name_len  > FCFG_CONFIG_MAX_NAME_LEN) {
        response->error.length = sprintf(response->error.message,
                "invalid name length: %d", name_len);
        return EINVAL;
    }

    data_body_len = sizeof(FCFGProtoDelConfigReq) + env_len + name_len;
    if (request->body_len != data_body_len) {
        response->error.length = sprintf(response->error.message,
                "invalid body length: %d, expect: %d",
                request->body_len, data_body_len);
        return EINVAL;
    }
    memcpy(env, del_config_req->env, env_len);
    *(env + env_len) = '\0';

    memcpy(name, del_config_req->env + env_len, name_len);
    *(name + name_len) = '\0';

    mysql_context = &((FCFGServerContext *)task->thread_data->arg)->mysql_context;
    result = fcfg_server_dao_del_config(mysql_context, env, name);
    if (result != 0) {
        if (result == ENOENT) {
            response->error.length = sprintf(response->error.message,
                    "config: %s not exist", name);
        } else {
            response->error.length = sprintf(response->error.message,
                    "internal server error");
        }
    }

    return result;
}