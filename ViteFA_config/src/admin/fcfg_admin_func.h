#ifndef _FCFG_ADMIN_FUNC_H
#define _FCFG_ADMIN_FUNC_H

#include "fcfg_admin.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct fcfg_admin {
    char username[128];
    char secret_key[512];
    int network_timeout;
    int connect_timeout;
    int server_count;
    ConnectionInfo *join_conn;
} FCFGAdminGlobal;

int fcfg_send_admin_join_request(struct fcfg_context *fcfg_context, int network_timeout,
        int connect_timeout);
int fcfg_admin_check_response(ConnectionInfo *join_conn,
        FCFGResponseInfo *resp_info, int network_timeout,
        unsigned char resp_cmd);
void fcfg_set_admin_header (FCFGProtoHeader *fcfg_header_proto,
        unsigned char cmd, int body_len);
int fcfg_admin_load_config (struct fcfg_context *fcfg_context,
        const char *config_filename);
int fcfg_do_conn_config_server (struct fcfg_context *fcfg_context);
void fcfg_disconn_config_server (ConnectionInfo *conn);
int fcfg_admin_env_set_entry(FCFGProtoGetEnvResp *get_env_resp,
        FCFGEnvEntry *rows, int *env_size);
int fcfg_admin_config_set_entry (FCFGProtoGetConfigResp *get_config_resp,
        FCFGConfigEntry *rows, int *config_len);
void fcfg_admin_print_env_array (FCFGEnvArray *array);
void fcfg_admin_print_config_array (FCFGConfigArray *array);
int fcfg_admin_check_arg(const char *env, const char *name, const char *value);

#define fcfg_admin_check_response(join_conn, resp_info, network_timeout, resp_cmd) \
    fcfg_check_response(join_conn, resp_info, network_timeout, resp_cmd)

#define FCFG_ADMIN_LIST_REQUEST_COUNT   20

#ifdef __cplusplus
}
#endif

#endif
