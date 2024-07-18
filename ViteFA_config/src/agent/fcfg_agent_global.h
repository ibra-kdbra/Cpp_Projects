
#ifndef _FCFG_AGENT_GLOBAL_H
#define _FCFG_AGENT_GLOBAL_H

#include "fastcommon/common_define.h"
#include "shmcache/shmcache.h"
#include "fastcommon/multi_socket_client.h"
#include "common/fcfg_types.h"

typedef struct fcfg_agent_global_vars {
    ConnectionInfo *join_conn;
    char *config_file;
    struct shmcache_context shm_context;
    char shm_config_file[MAX_PATH_SIZE];
    char env[FCFG_CONFIG_ENV_SIZE];
    char shm_version_key[128];
    char base_path[MAX_PATH_SIZE];
    int sync_log_buff_interval; //sync log buff to disk every interval seconds
    volatile bool continue_flag;
    bool rotate_error_log;
    int thread_stack_size;
    int log_file_keep_days;
    int network_timeout;
    int connect_timeout;
    int server_count;
} FCFGAgentGlobalVars;

#ifdef __cplusplus
extern "C" {
#endif
extern FCFGAgentGlobalVars g_agent_global_vars;

#ifdef __cplusplus
}
#endif

#endif
