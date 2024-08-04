//fcfg_server_push.h

#ifndef FCFG_SERVER_PUSH_H
#define FCFG_SERVER_PUSH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fastcommon/fast_task_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

int fcfg_server_push_init();
int fcfg_server_push_destroy();

int fcfg_server_push_configs(struct fast_task_info *task);

#ifdef __cplusplus
}
#endif

#endif
