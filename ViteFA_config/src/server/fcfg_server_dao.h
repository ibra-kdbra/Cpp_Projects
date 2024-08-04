
#ifndef _FCFG_SERVER_DAO_H
#define _FCFG_SERVER_DAO_H

#include <time.h>
#include <mysql.h>
#include <errmsg.h>
#include "fastcommon/common_define.h"
#include "common/fcfg_types.h"
#include "fcfg_server_env.h"

#if LIBMYSQL_VERSION_ID >= 80000
typedef bool my_bool;
#endif

typedef struct fcfg_mysql_context {
    MYSQL *mysql;

    struct {
        MYSQL_STMT *update_stmt;
        MYSQL_STMT *insert_stmt;
        MYSQL_STMT *delete_stmt;
        MYSQL_STMT *search_stmt;  //query by env and name for admin
        MYSQL_STMT *get_pk_stmt;  //get by env and name for admin
    } admin;

    struct {
        MYSQL_STMT *max_env_ver_stmt; //query max env version
        MYSQL_STMT *max_cfg_ver_stmt; //query max config version
    } monitor;

    struct {
        MYSQL_STMT *select_stmt;  //query by env and version
    } agent;

    time_t last_ping_time;
} FCFGMySQLContext;

#ifdef __cplusplus
extern "C" {
#endif

    int fcfg_server_dao_init(FCFGMySQLContext *context);

    void fcfg_server_dao_destroy(FCFGMySQLContext *context);

    int fcfg_server_dao_set_config(FCFGMySQLContext *context, const char *env,
            const char *name, const short type, const char *value);

    int fcfg_server_dao_get_config(FCFGMySQLContext *context, const char *env,
            const char *name, FCFGConfigArray *array);

    int fcfg_server_dao_del_config(FCFGMySQLContext *context, const char *env,
            const char *name);

    int fcfg_server_dao_list_config_by_env_and_version(FCFGMySQLContext *context,
            const char *env, const int64_t version, const int limit,
            FCFGConfigArray *array);

    int fcfg_server_dao_search_config(FCFGMySQLContext *context,
            const char *env, const char *name, const int offset,
            const int limit, FCFGConfigArray *array);

    int fcfg_server_dao_copy_config_array(FCFGConfigArray *src,
            FCFGConfigArray *dest);

    void fcfg_server_dao_free_config_array(FCFGConfigArray *array);

    int fcfg_server_dao_max_config_version(FCFGMySQLContext *context,
            const char *env, int64_t *max_version);

    int fcfg_server_dao_add_env(FCFGMySQLContext *context, const char *env);

    int fcfg_server_dao_del_env(FCFGMySQLContext *context, const char *env);

    int fcfg_server_dao_get_env(FCFGMySQLContext *context, const char *env,
            FCFGEnvEntry *entry);

    int fcfg_server_dao_list_env(FCFGMySQLContext *context, FCFGEnvArray *array);

    void fcfg_server_dao_free_env_array(FCFGEnvArray *array);

    int fcfg_server_dao_max_env_version(FCFGMySQLContext *context,
            int64_t *max_version);

    int fcfg_server_dao_ping(FCFGMySQLContext *context, const int thread_index);

#ifdef __cplusplus
}
#endif

#endif
