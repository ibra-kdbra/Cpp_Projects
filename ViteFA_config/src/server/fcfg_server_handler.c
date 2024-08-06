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

