
#ifndef _FCFG_SERVER_CFG_H
#define _FCFG_SERVER_CFG_H

#include "fastcommon/fast_mblock.h"
#include "common/fcfg_types.h"
#include "fcfg_server_types.h"

#ifdef __cplusplus
extern "C" {
#endif

    int fcfg_server_cfg_init();

    void fcfg_server_cfg_destroy();

    int fcfg_server_cfg_reload(struct fcfg_mysql_context *context);

    int fcfg_server_cfg_add_subscriber(const char *env, struct fast_task_info *task);

    void fcfg_server_cfg_remove_subscriber(struct fast_task_info *task);

    int fcfg_server_add_task_event(struct fast_task_info *task, const int type);

    void fcfg_server_free_event(FCFGServerPushEvent *event);

    static inline int fcfg_server_add_config_push_event(struct fast_task_info *task)
    {
        return fcfg_server_add_task_event(task, FCFG_SERVER_EVENT_TYPE_PUSH_CONFIG);
    }

#ifdef __cplusplus
}
#endif

#endif
