//fcfg_server_push.c

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
#include "fcfg_server_env.h"
#include "fcfg_server_cfg.h"
#include "fcfg_server_push.h"

static struct fcfg_mysql_context mysql_context;

static void *fcfg_server_push_entrance(void *arg)
{
    int usleep_time;
    int64_t count = 0;

    usleep_time = g_server_global_vars.reload_interval_ms * 1000;
    while (g_sf_global_vars.continue_flag) {
        if (fcfg_server_env_load(&mysql_context) != 0) {
            sleep(1);
        }

        if (fcfg_server_cfg_reload(&mysql_context) != 0) {
            sleep(1);
        }

        usleep(usleep_time);
        if (++count % 10000 == 0) {
            logDebug("push loop count: %"PRId64, count);
        }
    }

    return NULL;
}

int fcfg_server_push_init()
{
    int result;
    pthread_attr_t thread_attr;
    pthread_t tid;

    if ((result=fcfg_server_env_init()) != 0) {
        return result;
    }

    if ((result=fcfg_server_cfg_init()) != 0) {
        return result;
    }

    if ((result=fcfg_server_dao_init(&mysql_context)) != 0) {
        return result;
    }

    if ((result=fcfg_server_env_load(&mysql_context)) != 0) {
        return result;
    }

    if ((result=init_pthread_attr(&thread_attr,
                    g_sf_global_vars.thread_stack_size)) != 0)
    {
        return result;
    }

    if ((result=pthread_create(&tid, &thread_attr,
                    fcfg_server_push_entrance, NULL)) != 0)
    {
        logError("file: "__FILE__", line: %d, "
                "create thread failed, errno: %d, error info: %s",
                __LINE__, result, strerror(result));
        return result;
    }

    pthread_attr_destroy(&thread_attr);
    return 0;
}

int fcfg_server_push_destroy()
{
    return 0;
}

static int find_config_version_closest_less_equal(FCFGConfigArray *array,
        int64_t target_version)
{
    int low;
    int high;
    int mid;

    if (array->count == 0 || array->rows[0].version > target_version) {
        return -1;
    }

    if (array->rows[array->count - 1].version == target_version) {
        return array->count - 1;
    }

    low = 0;
    high = array->count - 1;
    while (low <= high) {
        mid = (low + high) / 2;
        if (array->rows[mid].version > target_version) {
            high = mid - 1;
        } else if (array->rows[mid].version < target_version) {
            low = mid + 1;
        } else {
            return mid;
        }
    }

    if (array->rows[high].version < target_version) {
        return high;
    } else {
        return high - 1;
    }
}

static int fcfg_server_do_push_configs(struct fast_task_info *task)
{
#define CONFIG_RECORD_SIZE(config) \
    (sizeof(FCFGProtoPushConfigBodyPart) + config->name.len + config->value.len)

    FCFGConfigMessageQueue *msg_queue;
    FCFGConfigEntry *config;
    FCFGProtoHeader *proto_header;
    FCFGProtoPushConfigHeader *header_part;
    FCFGProtoPushConfigBodyPart *body_part;
    FCFGEnvPublisher *publisher;
    char *env;
    int result;
    int start_offset;
    int record_size;
    int expect_size;
    int config_count;

    task->send.ptr->length = sizeof(FCFGProtoHeader) + sizeof(FCFGProtoPushConfigHeader);

    msg_queue = &((FCFGServerTaskArg *)task->arg)->msg_queue;
    publisher = ((FCFGServerTaskArg *)task->arg)->publisher;
    config = msg_queue->config_array->rows + msg_queue->offset;
    record_size = CONFIG_RECORD_SIZE(config);
    expect_size = task->send.ptr->length + record_size;
    env = publisher != NULL ? publisher->env : "";

    logDebug("file: "__FILE__", line: %d, client ip: %s, env: %s, "
            "config offset: %d, total count: %d", __LINE__,
            task->client_ip, env, msg_queue->offset,
            msg_queue->config_array->count);

    if (expect_size > task->send.ptr->size) {
        if ((result=sf_set_task_send_buffer_size(task, expect_size)) != 0) {
            return result;
        }
    }

    start_offset = msg_queue->offset;
    while (msg_queue->offset < msg_queue->config_array->count) {
        config = msg_queue->config_array->rows + msg_queue->offset;
        record_size = CONFIG_RECORD_SIZE(config);
        expect_size = task->send.ptr->length + record_size;
        if (expect_size > task->send.ptr->size) {
            break;
        }

        body_part = (FCFGProtoPushConfigBodyPart *)(task->send.ptr->data + task->send.ptr->length);
        body_part->status = config->status;
        body_part->type = config->type;
        body_part->name_len = config->name.len;
        int2buff(config->value.len, body_part->value_len);
        long2buff(config->version, body_part->version);
        int2buff(config->create_time, body_part->create_time);
        int2buff(config->update_time, body_part->update_time);
        memcpy(body_part->name, config->name.str, config->name.len);
        memcpy(body_part->name + config->name.len, config->value.str,
                config->value.len);

        task->send.ptr->length += record_size;
        msg_queue->offset++;
    }
    msg_queue->agent_cfg_version = msg_queue->config_array->rows
        [msg_queue->offset - 1].version;
    config_count = msg_queue->offset - start_offset;

    header_part = (FCFGProtoPushConfigHeader *)(task->send.ptr->data + sizeof(FCFGProtoHeader));
    short2buff(config_count, header_part->count);

    logDebug("file: "__FILE__", line: %d, client ip: %s, env: %s, "
            "send %d configs, config offset: %d, total count: %d, "
            "agent_cfg_version: %"PRId64", task length: %d",
            __LINE__, task->client_ip, env, config_count,
            msg_queue->offset, msg_queue->config_array->count,
            msg_queue->agent_cfg_version, task->send.ptr->length);

    proto_header = (FCFGProtoHeader *)task->send.ptr->data;
    int2buff(task->send.ptr->length - sizeof(FCFGProtoHeader), proto_header->body_len);
    proto_header->cmd = FCFG_PROTO_PUSH_CONFIG;
    proto_header->status = 0;
    return sf_send_add_event(task);
}

int fcfg_server_push_configs(struct fast_task_info *task)
{
    FCFGServerTaskArg *task_arg;
    FCFGConfigMessageQueue *msg_queue;
    int config_count;

    task_arg = (FCFGServerTaskArg *)task->arg;
    msg_queue = &task_arg->msg_queue;

    if (msg_queue->config_array == NULL || (msg_queue->config_array->version !=
                task_arg->publisher->config_array->version))
    {
        msg_queue->config_array = task_arg->publisher->config_array;
        config_count = msg_queue->config_array->count;
        if (config_count > 0) {
            msg_queue->offset = find_config_version_closest_less_equal(
                    msg_queue->config_array, msg_queue->agent_cfg_version) + 1;
        } else {
            msg_queue->offset = 0;
        }
    }

    if (msg_queue->offset >= msg_queue->config_array->count) {
        return 0;
    }

    task_arg->waiting_type |= FCFG_SERVER_TASK_WAITING_PUSH_RESP;
    return fcfg_server_do_push_configs(task);
}
