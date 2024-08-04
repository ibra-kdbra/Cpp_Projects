
#ifndef _FCFG_SERVER_ENV_H
#define _FCFG_SERVER_ENV_H

#include "common/fcfg_types.h"

struct fcfg_mysql_context;

#ifdef __cplusplus
extern "C" {
#endif

    int fcfg_server_env_init();

    void fcfg_server_env_destroy();

    int fcfg_server_env_load(struct fcfg_mysql_context *context);

    bool fcfg_server_env_exists(const char *env);

#ifdef __cplusplus
}
#endif

#endif
