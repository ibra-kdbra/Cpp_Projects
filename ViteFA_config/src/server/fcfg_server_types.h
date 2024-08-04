#ifndef _FCFG_SERVER_TYPES_H
#define _FCFG_SERVER_TYPES_H

#include <time.h>
#include <pthread.h>
#include "fastcommon/common_define.h"
#include "fastcommon/common_blocked_queue.h"
#include "fastcommon/fast_task_queue.h"
#include "fastcommon/fast_mblock.h"
#include "fastcommon/fc_list.h"
#include "fastcommon/json_parser.h"
#include "fcfg_server_dao.h"

#define FCFG_SERVER_DEFAULT_RELOAD_INTERVAL       500
#define FCFG_SERVER_DEFAULT_CHECK_ALIVE_INTERVAL  300

#define FCFG_SERVER_EVENT_TYPE_PUSH_CONFIG   1
#define FCFG_SERVER_EVENT_TYPE_ACTIVE_TEST   2

#define FCFG_SERVER_TASK_WAITING_REQUEST          0
#define FCFG_SERVER_TASK_WAITING_PUSH_RESP        1
#define FCFG_SERVER_TASK_WAITING_ACTIVE_TEST_RESP 2

#define FCFG_SERVER_TASK_WAITING_RESP (FCFG_SERVER_TASK_WAITING_PUSH_RESP | \
        FCFG_SERVER_TASK_WAITING_ACTIVE_TEST_RESP)


#define SERVER_CTX      ((FCFGServerContext *)task->thread_data->arg)

typedef struct fcfg_server_context {
    FCFGMySQLContext mysql_context;
    fc_json_context_t json_ctx;
    struct common_blocked_queue push_queue;
} FCFGServerContext;

typedef struct fcfg_env_publisher {
    char *env;
    int64_t current_version;
    struct fc_list_head head;   //subscribe task double chain
    pthread_mutex_t lock;
    FCFGConfigArray *config_array;

    struct {
        time_t last_reload_all_time;
        struct {
            int64_t total_count;
            int64_t last_count;
        } version_changed;
        bool reload_all;
    } config_stat;
} FCFGEnvPublisher;

typedef struct fcfg_config_message_queue {
    int64_t agent_cfg_version;
    FCFGConfigArray *config_array;
    int offset;
} FCFGConfigMessageQueue;

typedef struct fcfg_server_task_arg {
    volatile int64_t task_version;

    struct fc_list_head subscribe;
    FCFGEnvPublisher *publisher;

    FCFGConfigMessageQueue msg_queue;

    int last_recv_pkg_time;
    short waiting_type;
    bool joined;
} FCFGServerTaskArg;

typedef struct fcfg_server_push_event {
    struct fast_task_info *task;
    int64_t task_version;
    int type;
} FCFGServerPushEvent;

#endif
