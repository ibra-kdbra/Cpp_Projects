
#include <sys/stat.h>
#include "fastcommon/shared_func.h"
#include "fastcommon/logger.h"
#include "fastcommon/sched_thread.h"
#include "sf/sf_global.h"
#include "sf/sf_service.h"
#include "common/fcfg_types.h"
#include "fcfg_server_global.h"
#include "fcfg_server_dao.h"

#define FCFG_KEY_NAME_ENVIRONMENT_VERSION  "fast_environment_version"
#define FCFG_KEY_NAME_CONFIG_VERSION       "fast_config_version"

#define FCFG_MYSQL_STMT_PREPARE(stmt, sql) \
    do {  \
    } while (0)

static int fcfg_server_dao_check_stmt(FCFGMySQLContext *context, MYSQL_STMT **stmt,
        const char *sql)
{
    int result;

    if (*stmt != NULL) {
        return 0;
    }
    if (context->mysql == NULL) {
        if ((result=fcfg_server_dao_init(context)) != 0) {
            return result;
        }
    }

    *stmt = mysql_stmt_init(context->mysql);
    if (*stmt == NULL) {
        logError("file: "__FILE__", line: %d, "
                "call mysql_stmt_init fail", __LINE__);
        return ENOMEM;
    }
    if (mysql_stmt_prepare(*stmt, sql, strlen(sql)) != 0) {
        logError("file: "__FILE__", line: %d, "
                "prepare stmt fail, error info: %s, sql: %s",
                __LINE__, mysql_stmt_error(*stmt), sql);
        mysql_stmt_close(*stmt);
        *stmt = NULL;
        return EINVAL;
    }
    return 0;
}

#define CONFIG_SELECT_SQL "SELECT name, type, value, version, status, " \
        "UNIX_TIMESTAMP(create_time), UNIX_TIMESTAMP(update_time) FROM fast_config "

const char *insert_sql = "INSERT INTO fast_config "
    "(env, name, type, value, version, status) VALUES (?, ?, ?, ?, ?, 0)";
const char *update_sql = "UPDATE fast_config "
    "SET type = ?, value = ?, version = ?, status = 0 WHERE env = ? AND name = ?";
const char *delete_sql = "UPDATE fast_config "
    "SET version = ?, status = 1 WHERE env = ? AND name = ? and status = 0";
const char *select_sql = CONFIG_SELECT_SQL
    "WHERE env = ? AND version > ? ORDER BY version limit ?";
const char *search_sql = CONFIG_SELECT_SQL
    "WHERE env = ? AND name like ? and status = 0 ORDER BY name limit ?, ?";
const char *get_pk_sql = CONFIG_SELECT_SQL
    "WHERE env = ? AND name = ? and status = 0";
const char *max_env_ver_sql = "SELECT MAX(version) FROM fast_environment";
const char *max_cfg_ver_sql = "SELECT MAX(version) FROM fast_config "
    "WHERE env = ?";

#define FCFG_GET_ADMIN_UPDATE_STMT(context) \
            fcfg_server_dao_check_stmt(context, &context->admin.update_stmt, \
                    update_sql)

#define FCFG_GET_ADMIN_INSERT_STMT(context) \
            fcfg_server_dao_check_stmt(context, &context->admin.insert_stmt, \
                    insert_sql)

#define FCFG_GET_ADMIN_DELETE_STMT(context) \
            fcfg_server_dao_check_stmt(context, &context->admin.delete_stmt, \
                    delete_sql)

#define FCFG_GET_ADMIN_SEARCH_STMT(context) \
            fcfg_server_dao_check_stmt(context, &context->admin.search_stmt, \
                    search_sql)

#define FCFG_GET_ADMIN_GET_PK_STMT(context) \
            fcfg_server_dao_check_stmt(context, &context->admin.get_pk_stmt, \
                    get_pk_sql)

#define FCFG_GET_AGENT_SELECT_STMT(context) \
            fcfg_server_dao_check_stmt(context, &context->agent.select_stmt, \
                    select_sql)

#define FCFG_GET_MONITOR_MAX_ENV_VER_STMT(context) \
            fcfg_server_dao_check_stmt(context, &context->monitor.max_env_ver_stmt, \
                    max_env_ver_sql)

#define FCFG_GET_MONITOR_MAX_CFG_VER_STMT(context) \
            fcfg_server_dao_check_stmt(context, &context->monitor.max_cfg_ver_stmt, \
                    max_cfg_ver_sql)

int fcfg_server_dao_init(FCFGMySQLContext *context)
{
    my_bool on;
    int timeout;

    context->mysql = mysql_init(NULL);

    on = false;
    mysql_options(context->mysql, MYSQL_OPT_RECONNECT, &on);
    timeout = SF_G_CONNECT_TIMEOUT;
    mysql_options(context->mysql, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);

    timeout = SF_G_NETWORK_TIMEOUT;
    mysql_options(context->mysql, MYSQL_OPT_READ_TIMEOUT, &timeout);

    timeout = SF_G_NETWORK_TIMEOUT;
    mysql_options(context->mysql, MYSQL_OPT_WRITE_TIMEOUT, &timeout);

    if (mysql_real_connect(context->mysql,
                g_server_global_vars.db_config.host,
                g_server_global_vars.db_config.user,
                g_server_global_vars.db_config.password,
                g_server_global_vars.db_config.database,
                g_server_global_vars.db_config.port, NULL, 0) == NULL)
    {
        logError("file: "__FILE__", line: %d, "
                "connect to mysql %s:%d fail, error info: %s",
                __LINE__, g_server_global_vars.db_config.host,
                g_server_global_vars.db_config.port,
                mysql_error(context->mysql));
        mysql_close(context->mysql);
        context->mysql = NULL;
        return ENOTCONN;
    }

    logInfo("file: "__FILE__", line: %d, "
            "connect to mysql server %s:%d success",
            __LINE__, g_server_global_vars.db_config.host,
            g_server_global_vars.db_config.port);
    context->last_ping_time = g_current_time;
    return 0;
}

#define FCFG_MYSQL_STMT_CLOSE(stmt) \
    do { \
        if (stmt != NULL) { \
            mysql_stmt_close(stmt); \
        } \
    } while (0)

void fcfg_server_dao_destroy(FCFGMySQLContext *context)
{
    if (context->mysql == NULL) {
        return;
    }

    FCFG_MYSQL_STMT_CLOSE(context->admin.update_stmt);
    FCFG_MYSQL_STMT_CLOSE(context->admin.insert_stmt);
    FCFG_MYSQL_STMT_CLOSE(context->admin.delete_stmt);
    FCFG_MYSQL_STMT_CLOSE(context->admin.search_stmt);
    FCFG_MYSQL_STMT_CLOSE(context->admin.get_pk_stmt);
    FCFG_MYSQL_STMT_CLOSE(context->agent.select_stmt);
    FCFG_MYSQL_STMT_CLOSE(context->monitor.max_env_ver_stmt);
    FCFG_MYSQL_STMT_CLOSE(context->monitor.max_cfg_ver_stmt);

    mysql_close(context->mysql);
    memset(context, 0, sizeof(FCFGMySQLContext));
    context->last_ping_time = g_current_time;
}
