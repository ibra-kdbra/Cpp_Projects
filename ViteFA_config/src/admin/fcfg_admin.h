#ifndef _FCFG_ADMIN_H
#define _FCFG_ADMIN_H

#include "fastcommon/multi_socket_client.h"
#include "fcfg_types_admin.h"

#ifdef __cplusplus
extern "C" {
#endif

struct fcfg_context {
    char username[128];
    char secret_key[512];
    int network_timeout;
    int connect_timeout;
    int server_count;
    int join_index;
    char base_path[MAX_PATH_SIZE];
    ConnectionInfo *join_conn;
};

int fcfg_admin_init_from_file (struct fcfg_context *fcfg_context,
        const char *config_filename);
int fcfg_admin_destroy (struct fcfg_context *fcfg_context);

int fcfg_admin_config_set (struct fcfg_context *fcfg_context,
        const char *env, const char *config_name, const char *config_value,
        const unsigned char type);    
int fcfg_admin_config_get (struct fcfg_context *fcfg_context,
        const char *env, const char *config_name, FCFGConfigArray *array);
int fcfg_admin_config_del (struct fcfg_context *fcfg_context,
        const char *env, const char *config_name);
int fcfg_admin_config_list (struct fcfg_context *fcfg_context,
        const char *env, const char *config_name, const int limit, FCFGConfigArray *array);

int fcfg_admin_env_add (struct fcfg_context *fcfg_context, const char *env);
int fcfg_admin_env_get (struct fcfg_context *fcfg_context, const char *env,
        FCFGEnvArray *array);
int fcfg_admin_env_del (struct fcfg_context *fcfg_context, const char *env);
int fcfg_admin_env_list (struct fcfg_context *fcfg_context,
        FCFGEnvArray *array);
void fcfg_print_env_array (FCFGEnvArray *array);
void fcfg_print_config_array (FCFGConfigArray *array, int print_detail);
void fcfg_free_env_info_array(FCFGEnvArray *array);
void fcfg_free_config_info_array(FCFGConfigArray *array);
   
#ifdef __cplusplus
}
#endif

#endif
