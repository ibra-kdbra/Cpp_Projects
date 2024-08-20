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

static inline int fcfg_server_dao_stmt_execute(FCFGMySQLContext *context,
        MYSQL_STMT *stmt, const char *filename, const int line)
{
    int result;
    int error_no;

    if ((result=mysql_stmt_execute(stmt)) != 0) {
        error_no = mysql_stmt_errno(stmt);
        logError("file: %s, line: %d, "
                "call mysql_stmt_execute fail, "
                "result: %d, error code: %d, error info: %s",
                filename, line, result, error_no, mysql_stmt_error(stmt));

        if ((error_no == CR_SERVER_GONE_ERROR) || (error_no == CR_SERVER_LOST)) {
            fcfg_server_dao_destroy(context);
        }
        return EFAULT;
    }

    return 0;
}

#define MYSQL_STMT_EXECUTE(context, stmt) \
    fcfg_server_dao_stmt_execute(context, stmt, __FILE__, __LINE__)

static inline int fcfg_server_dao_real_query(FCFGMySQLContext *context,
        const char *sql, const int len)
{
    int result;
    int error_no;

    if (context->mysql == NULL) {
        if ((result=fcfg_server_dao_init(context)) != 0) {
            return result;
        }
    }

    if ((result=mysql_real_query(context->mysql, sql, len)) != 0) {
        error_no = mysql_errno(context->mysql);
        logError("file: "__FILE__", line: %d, "
                "call mysql_real_query fail, "
                "result: %d, error code: %d, error info: %s, sql: %s",
                __LINE__, result, error_no, mysql_error(context->mysql), sql);
        if ((error_no == CR_SERVER_GONE_ERROR) || (error_no == CR_SERVER_LOST)) {
            fcfg_server_dao_destroy(context);
        }
        return EFAULT;
    }

    return 0;
}

static int64_t fcfg_server_dao_next_version(FCFGMySQLContext *context,
        const char *name)
{
    MYSQL_RES *mysql_result;
    MYSQL_ROW row;
    int64_t version;
    int len;
    int result;
    char update_sql[256];
    const char *nextval_sql = "SELECT @nextval";
   
    len = sprintf(update_sql, "UPDATE fast_increment "
        "SET value=(@nextval:=value+1) WHERE name = '%s'", name);
    if ((result=fcfg_server_dao_real_query(context, update_sql, len)) != 0) {
        return -1;
    }

    if (mysql_affected_rows(context->mysql) != 1) {
        logError("file: "__FILE__", line: %d, "
                "mysql_affected_rows != 1, sql: %s",
                __LINE__, update_sql);
        return -2;
    }

    if ((result=fcfg_server_dao_real_query(context, nextval_sql,
                    strlen(nextval_sql))) != 0)
    {
        return -3;
    }

    mysql_result = mysql_store_result(context->mysql);
    if (mysql_result == NULL) {
        logError("file: "__FILE__", line: %d, "
                "call mysql_store_result fail, error info: %s, sql: %s",
                __LINE__, mysql_error(context->mysql), nextval_sql);
        return -4;
    }

    row = mysql_fetch_row(mysql_result);
    if (row == NULL) {
        logError("file: "__FILE__", line: %d, "
                "call mysql_fetch_row fail, error info: %s, sql: %s",
                __LINE__, mysql_error(context->mysql), nextval_sql);
        return -5;
    }

    version = strtoll(row[0], NULL, 10);
    mysql_free_result(mysql_result);
    return version;
}

#define fcfg_server_dao_next_config_version(context) \
    fcfg_server_dao_next_version(context, FCFG_KEY_NAME_CONFIG_VERSION)

#define fcfg_server_dao_next_env_version(context) \
    fcfg_server_dao_next_version(context, FCFG_KEY_NAME_ENVIRONMENT_VERSION)


int fcfg_server_dao_set_config(FCFGMySQLContext *context, const char *env,
        const char *name, const short type, const char *value)
{
    MYSQL_BIND update_binds[5];
    MYSQL_BIND insert_binds[5];
    int64_t version;
    unsigned long env_len;
    unsigned long name_len;
    unsigned long value_len;
    FCFGConfigArray array;
    int result;

    if (fcfg_server_dao_get_config(context, env, name, &array) == 0
            && array.count == 1)
    {
        bool same;
        same = (type == array.rows[0].type) &&
            (strcmp(value, array.rows[0].value.str) == 0);
        fcfg_server_dao_free_config_array(&array);
        if (same) {
            return 0;
        }
    }

    version = fcfg_server_dao_next_config_version(context);
    if (version < 0) {
        return EINVAL;
    }

    if ((result=FCFG_GET_ADMIN_UPDATE_STMT(context)) != 0) {
        return result;
    }

    env_len = strlen(env);
    name_len = strlen(name);
    value_len = strlen(value);

    memset(update_binds, 0, sizeof(update_binds));

    update_binds[0].buffer_type = MYSQL_TYPE_SHORT;
    update_binds[0].buffer = (char *)&type;

    update_binds[1].buffer_type = MYSQL_TYPE_STRING;
    update_binds[1].buffer = (char *)value;
    update_binds[1].length = &value_len;

    update_binds[2].buffer_type = MYSQL_TYPE_LONGLONG;
    update_binds[2].buffer = (char *)&version;

    update_binds[3].buffer_type = MYSQL_TYPE_STRING;
    update_binds[3].buffer = (char *)env;
    update_binds[3].length = &env_len;

    update_binds[4].buffer_type = MYSQL_TYPE_STRING;
    update_binds[4].buffer = (char *)name;
    update_binds[4].length = &name_len;

    if (mysql_stmt_bind_param(context->admin.update_stmt, update_binds) != 0) {
        logError("file: "__FILE__", line: %d, "
                "call mysql_stmt_bind_param fail, error info: %s",
                __LINE__, mysql_stmt_error(context->admin.update_stmt));
        return EINVAL;
    }

    if ((result=MYSQL_STMT_EXECUTE(context,
                    context->admin.update_stmt)) != 0)
    {
        return result;
    }
    if (mysql_stmt_affected_rows(context->admin.update_stmt) != 0) {
        return 0;
    }

    if ((result=FCFG_GET_ADMIN_INSERT_STMT(context)) != 0) {
        return result;
    }
    memset(insert_binds, 0, sizeof(insert_binds));
    insert_binds[0].buffer_type = MYSQL_TYPE_STRING;
    insert_binds[0].buffer = (char *)env;
    insert_binds[0].length = &env_len;

    insert_binds[1].buffer_type = MYSQL_TYPE_STRING;
    insert_binds[1].buffer = (char *)name;
    insert_binds[1].length = &name_len;

    insert_binds[2].buffer_type = MYSQL_TYPE_SHORT;
    insert_binds[2].buffer = (char *)&type;

    insert_binds[3].buffer_type = MYSQL_TYPE_STRING;
    insert_binds[3].buffer = (char *)value;
    insert_binds[3].length = &value_len;

    insert_binds[4].buffer_type = MYSQL_TYPE_LONGLONG;
    insert_binds[4].buffer = (char *)&version;

    if (mysql_stmt_bind_param(context->admin.insert_stmt, insert_binds) != 0) {
        logError("file: "__FILE__", line: %d, "
                "call mysql_stmt_bind_param fail, error info: %s",
                __LINE__, mysql_stmt_error(context->admin.insert_stmt));
        return EINVAL;
    }

    return MYSQL_STMT_EXECUTE(context, context->admin.insert_stmt);
}

int fcfg_server_dao_del_config(FCFGMySQLContext *context, const char *env,
        const char *name)
{
    MYSQL_BIND delete_binds[3];
    int64_t version;
    unsigned long env_len;
    unsigned long name_len;
    int result;

    version = fcfg_server_dao_next_config_version(context);
    if (version < 0) {
        return EINVAL;
    }

    if ((result=FCFG_GET_ADMIN_DELETE_STMT(context)) != 0) {
        return result;
    }

    env_len = strlen(env);
    name_len = strlen(name);
    memset(delete_binds, 0, sizeof(delete_binds));

    delete_binds[0].buffer_type = MYSQL_TYPE_LONGLONG;
    delete_binds[0].buffer = (char *)&version;

    delete_binds[1].buffer_type = MYSQL_TYPE_STRING;
    delete_binds[1].buffer = (char *)env;
    delete_binds[1].length = &env_len;

    delete_binds[2].buffer_type = MYSQL_TYPE_STRING;
    delete_binds[2].buffer = (char *)name;
    delete_binds[2].length = &name_len;

    if (mysql_stmt_bind_param(context->admin.delete_stmt, delete_binds) != 0) {
        logError("file: "__FILE__", line: %d, "
                "call mysql_stmt_bind_param fail, error info: %s",
                __LINE__, mysql_stmt_error(context->admin.delete_stmt));
        return EINVAL;
    }

    if ((result=MYSQL_STMT_EXECUTE(context,
                    context->admin.delete_stmt)) != 0)
    {
        return result;
    }
    if (mysql_stmt_affected_rows(context->admin.delete_stmt) == 0) {
        return ENOENT;
    }
    return 0;
}

static int fcfg_server_dao_store_rows(FCFGMySQLContext *context,
        MYSQL_STMT *stmt, FCFGConfigArray *array)
{
    MYSQL_BIND result_binds[7];
    int result;
    int row_count;
    int bytes;
    struct {
        char name[FCFG_CONFIG_NAME_SIZE];
        char value[FCFG_CONFIG_VALUE_SIZE];
        int64_t version;
        short status;
        short type;
        unsigned long name_len;
        unsigned long value_len;
        int create_time;
        int update_time;
    } buffer;
    my_bool is_null[7];
    my_bool error[7];
    FCFGConfigEntry *current;
    FCFGConfigEntry *end;

    if ((result=MYSQL_STMT_EXECUTE(context, stmt)) != 0) {
        array->rows = NULL;
        array->alloc = array->count = 0;
        return result;
    }

    buffer.name_len = buffer.value_len = 0;
    memset(result_binds, 0, sizeof(result_binds));
    result_binds[0].buffer_type = MYSQL_TYPE_STRING;
    result_binds[0].buffer = buffer.name;
    result_binds[0].buffer_length = FCFG_CONFIG_NAME_SIZE;
    result_binds[0].length = &buffer.name_len;
    result_binds[0].is_null = &is_null[0];
    result_binds[0].error = &error[0];

    result_binds[1].buffer_type = MYSQL_TYPE_SHORT;
    result_binds[1].buffer = (char *)&buffer.type;
    result_binds[1].is_null = &is_null[1];
    result_binds[1].error = &error[1];

    result_binds[2].buffer_type = MYSQL_TYPE_STRING;
    result_binds[2].buffer = buffer.value;
    result_binds[2].buffer_length = FCFG_CONFIG_VALUE_SIZE;
    result_binds[2].length = &buffer.value_len;
    result_binds[2].is_null = &is_null[2];
    result_binds[2].error = &error[2];

    result_binds[3].buffer_type = MYSQL_TYPE_LONGLONG;
    result_binds[3].buffer = (char *)&buffer.version;
    result_binds[3].is_null = &is_null[3];
    result_binds[3].error = &error[3];

    result_binds[4].buffer_type = MYSQL_TYPE_SHORT;
    result_binds[4].buffer = (char *)&buffer.status;
    result_binds[4].is_null = &is_null[4];
    result_binds[4].error = &error[4];

    result_binds[5].buffer_type = MYSQL_TYPE_LONG;
    result_binds[5].buffer = (char *)&buffer.create_time;
    result_binds[5].is_null = &is_null[5];
    result_binds[5].error = &error[5];

    result_binds[6].buffer_type = MYSQL_TYPE_LONG;
    result_binds[6].buffer = (char *)&buffer.update_time;
    result_binds[6].is_null = &is_null[6];
    result_binds[6].error = &error[6];

    if (mysql_stmt_bind_result(stmt, result_binds) != 0) {
        logError("file: "__FILE__", line: %d, "
                "call mysql_stmt_bind_result fail, error info: %s",
                __LINE__, mysql_stmt_error(stmt));
        array->rows = NULL;
        array->alloc = array->count = 0;
        return EINVAL;
    }

    if (mysql_stmt_store_result(stmt) != 0) {
        logError("file: "__FILE__", line: %d, "
                "call mysql_stmt_store_result fail, error info: %s",
                __LINE__, mysql_stmt_error(stmt));
        array->rows = NULL;
        array->alloc = array->count = 0;
        return EINVAL;
    }

    row_count = mysql_stmt_num_rows(stmt);
    if (row_count == 0) {
        array->alloc = array->count = 0;
        array->rows = NULL;
        return 0;
    }

    if (row_count <= 2) {  //fast path
        array->alloc = row_count;
    } else {
        array->alloc = 4;
        while (array->alloc < row_count) {
            array->alloc *= 2;
        }
    }

    bytes = sizeof(FCFGConfigEntry) * array->alloc;
    array->rows = (FCFGConfigEntry *)malloc(bytes);
    if (array->rows == NULL) {
        logError("file: "__FILE__", line: %d, "
                "malloc %d bytes fail", __LINE__, bytes);
        array->alloc = array->count = 0;
        return ENOMEM;
    }

    end = array->rows + row_count;
    for (current=array->rows; current<end; current++) {
        if (mysql_stmt_fetch(stmt) != 0) {
            logError("file: "__FILE__", line: %d, "
                    "call mysql_stmt_fetch fail, error info: %s",
                    __LINE__, mysql_stmt_error(stmt));
            array->count = current - array->rows;
            fcfg_server_dao_free_config_array(array);
            return EFAULT;
        }

        bytes = buffer.name_len + buffer.value_len + 2;
        current->name.str = (char *)malloc(bytes);
        if (current->name.str == NULL) {
            logError("file: "__FILE__", line: %d, "
                    "malloc %d bytes fail", __LINE__, bytes);
            array->count = current - array->rows;
            fcfg_server_dao_free_config_array(array);
            return ENOMEM;
        }
        current->value.str = current->name.str + buffer.name_len + 1;
        memcpy(current->name.str, buffer.name, buffer.name_len + 1);
        memcpy(current->value.str, buffer.value, buffer.value_len + 1);
        current->version = buffer.version;
        current->status = buffer.status;
        current->type = buffer.type;
        current->create_time = buffer.create_time;
        current->update_time = buffer.update_time;
        current->name.len = buffer.name_len;
        current->value.len = buffer.value_len;

        /*
        logInfo("name: %.*s, type: %d, value: %.*s, version: %"PRId64", status: %d, "
                "create_time: %ld, update_time: %ld",
                current->name.len, current->name.str, current->type,
                current->value.len, current->value.str,
                current->version, current->status,
                (long)current->create_time, (long)current->update_time);
                */
    }

    array->count = row_count;
    return 0;
}

int fcfg_server_dao_list_config_by_env_and_version(FCFGMySQLContext *context,
        const char *env, const int64_t version, const int limit,
        FCFGConfigArray *array)
{
    MYSQL_BIND select_binds[3];
    unsigned long env_len;
    int result;

    if ((result=FCFG_GET_AGENT_SELECT_STMT(context)) != 0) {
        return result;
    }

    env_len = strlen(env);
    memset(select_binds, 0, sizeof(select_binds));

    select_binds[0].buffer_type = MYSQL_TYPE_STRING;
    select_binds[0].buffer = (char *)env;
    select_binds[0].length = &env_len;

    select_binds[1].buffer_type = MYSQL_TYPE_LONGLONG;
    select_binds[1].buffer = (char *)&version;

    select_binds[2].buffer_type = MYSQL_TYPE_LONG;
    select_binds[2].buffer = (char *)&limit;

    if (mysql_stmt_bind_param(context->agent.select_stmt, select_binds) != 0) {
        logError("file: "__FILE__", line: %d, "
                "call mysql_stmt_bind_param fail, error info: %s",
                __LINE__, mysql_stmt_error(context->agent.select_stmt));
        array->rows = NULL;
        array->count = 0;
        return EINVAL;
    }

    return fcfg_server_dao_store_rows(context, context->agent.select_stmt, array);
}

int fcfg_server_dao_search_config(FCFGMySQLContext *context,
        const char *env, const char *name, const int offset,
        const int limit, FCFGConfigArray *array)
{
    MYSQL_BIND search_binds[4];
    unsigned long env_len;
    unsigned long name_len;
    int result;

    if ((result=FCFG_GET_ADMIN_SEARCH_STMT(context)) != 0) {
        return result;
    }

    env_len = strlen(env);
    name_len = strlen(name);
    memset(search_binds, 0, sizeof(search_binds));

    search_binds[0].buffer_type = MYSQL_TYPE_STRING;
    search_binds[0].buffer = (char *)env;
    search_binds[0].length = &env_len;

    search_binds[1].buffer_type = MYSQL_TYPE_STRING;
    search_binds[1].buffer = (char *)name;
    search_binds[1].length = &name_len;

    search_binds[2].buffer_type = MYSQL_TYPE_LONG;
    search_binds[2].buffer = (char *)&offset;

    search_binds[3].buffer_type = MYSQL_TYPE_LONG;
    search_binds[3].buffer = (char *)&limit;

    if (mysql_stmt_bind_param(context->admin.search_stmt, search_binds) != 0) {
        logError("file: "__FILE__", line: %d, "
                "call mysql_stmt_bind_param fail, error info: %s",
                __LINE__, mysql_stmt_error(context->admin.search_stmt));
        array->rows = NULL;
        array->count = 0;
        return EINVAL;
    }

    return fcfg_server_dao_store_rows(context, context->admin.search_stmt, array);
}

int fcfg_server_dao_get_config(FCFGMySQLContext *context, const char *env,
        const char *name, FCFGConfigArray *array)
{
    MYSQL_BIND get_pk_binds[2];
    unsigned long env_len;
    unsigned long name_len;
    int result;

    if ((result=FCFG_GET_ADMIN_GET_PK_STMT(context)) != 0) {
        return result;
    }

    env_len = strlen(env);
    name_len = strlen(name);
    memset(get_pk_binds, 0, sizeof(get_pk_binds));

    get_pk_binds[0].buffer_type = MYSQL_TYPE_STRING;
    get_pk_binds[0].buffer = (char *)env;
    get_pk_binds[0].length = &env_len;

    get_pk_binds[1].buffer_type = MYSQL_TYPE_STRING;
    get_pk_binds[1].buffer = (char *)name;
    get_pk_binds[1].length = &name_len;

    if (mysql_stmt_bind_param(context->admin.get_pk_stmt, get_pk_binds) != 0) {
        logError("file: "__FILE__", line: %d, "
                "call mysql_stmt_bind_param fail, error info: %s",
                __LINE__, mysql_stmt_error(context->admin.get_pk_stmt));
        array->rows = NULL;
        array->count = 0;
        return EINVAL;
    }

    return fcfg_server_dao_store_rows(context, context->admin.get_pk_stmt, array);
}

int fcfg_server_dao_copy_config_array(FCFGConfigArray *src, FCFGConfigArray *dest)
{
    FCFGConfigEntry *cur;
    FCFGConfigEntry *end;
    FCFGConfigEntry *out;
    int bytes;

    if (src->count == 0) {
        return 0;
    }

    out = dest->rows + dest->count;
    end = src->rows + src->count;
    for (cur=src->rows; cur<end; cur++, out++) {
        out->name.len = cur->name.len;
        out->value.len = cur->value.len;
        out->type = cur->type;
        out->version = cur->version;
        out->status = cur->status;
        out->create_time = cur->create_time;
        out->update_time = cur->update_time;

        bytes = cur->name.len + cur->value.len + 2;
        out->name.str = (char *)malloc(bytes);
        if (out->name.str == NULL) {
            logError("file: "__FILE__", line: %d, "
                    "malloc %d bytes fail", __LINE__, bytes);
            return ENOMEM;
        }
        out->value.str = out->name.str + cur->name.len + 1;
        memcpy(out->name.str, cur->name.str, bytes);
    }

    dest->count += src->count;
    return 0;
}

void fcfg_server_dao_free_config_array(FCFGConfigArray *array)
{
    FCFGConfigEntry *current;
    FCFGConfigEntry *end;

    if (array->rows == NULL) {
        return;
    }

    end = array->rows + array->count;
    for (current=array->rows; current<end; current++) {
        if (current->name.str != NULL) {
            free(current->name.str);
        }
    }

    free(array->rows);
    array->rows = NULL;
    array->alloc = array->count = 0;
}

static int fcfg_server_dao_env_execute(FCFGMySQLContext *context,
        const char *sql, const char *env, int *affected_rows)
{
    MYSQL_STMT *stmt;
    MYSQL_BIND binds[2];
    unsigned long env_len;
    int64_t version;
    int result;

    version = fcfg_server_dao_next_env_version(context);
    if (version < 0) {
        return EINVAL;
    }

    stmt = NULL;
    if ((result=fcfg_server_dao_check_stmt(context, &stmt, sql)) != 0) {
        return result;
    }

    env_len = strlen(env);
    memset(binds, 0, sizeof(binds));

    binds[0].buffer_type = MYSQL_TYPE_LONGLONG;
    binds[0].buffer = (char *)&version;

    binds[1].buffer_type = MYSQL_TYPE_STRING;
    binds[1].buffer = (char *)env;
    binds[1].length = &env_len;

    do {
        if (mysql_stmt_bind_param(stmt, binds) != 0) {
            logError("file: "__FILE__", line: %d, "
                    "call mysql_stmt_bind_param fail, "
                    "error info: %s, stmt: %s",
                    __LINE__, mysql_stmt_error(stmt), sql);
            result = EINVAL;
            break;
        }

        if ((result=MYSQL_STMT_EXECUTE(context, stmt)) != 0) {
            logError("file: "__FILE__", line: %d, "
                    "execute stmt fail, sql: %s", __LINE__, sql);
            break;
        }

        if (affected_rows != NULL) {
            *affected_rows = mysql_stmt_affected_rows(stmt);
        }

        result = 0;
    } while (0);

    mysql_stmt_close(stmt);
    return result;
}
int fcfg_server_dao_del_env(FCFGMySQLContext *context, const char *env)
{
    int affected_rows;
    int result;
    FCFGEnvEntry entry;
    const char *delete_sql = "UPDATE fast_environment SET status = 1, "
        "version = ? WHERE env = ?";

    if ((result=fcfg_server_dao_get_env(context, env, &entry)) != 0) {
        return result;
    }

    if ((result=fcfg_server_dao_env_execute(context, delete_sql, env,
                    &affected_rows)) != 0)
    {
        return result;
    }
    return affected_rows >= 1 ? 0 : ENOENT;
}

static int fcfg_server_dao_do_list_env(FCFGMySQLContext *context, const char *env,
        FCFGEnvArray *array)
{
    MYSQL_RES *mysql_result;
    MYSQL_ROW row;
    FCFGEnvEntry *current;
    FCFGEnvEntry  *end;
    int row_count;
    int bytes;
    int sql_len;
    int result;
    char select_sql[512];
    
    strcpy(select_sql, "SELECT env, UNIX_TIMESTAMP(create_time), "
        "UNIX_TIMESTAMP(update_time) FROM fast_environment "
        "WHERE status = 0");
    sql_len = strlen(select_sql);
    if (env != NULL) {
        char escaped_env[2 * FCFG_CONFIG_ENV_SIZE];
        mysql_real_escape_string_quote(context->mysql, escaped_env,
                env, strlen(env), '\'');
        sql_len += sprintf(select_sql + sql_len, " AND env = '%s'",
                escaped_env);
    } else {
        sql_len += sprintf(select_sql + sql_len, " ORDER BY env");
    }

    if ((result=fcfg_server_dao_real_query(context, select_sql,
                    sql_len)) != 0)
    {
        return result;
    }

    mysql_result = mysql_store_result(context->mysql);
    if (mysql_result == NULL) {
        logError("file: "__FILE__", line: %d, "
                "call mysql_store_result fail, error info: %s, sql: %s",
                __LINE__, mysql_error(context->mysql), select_sql);
        return EINVAL;
    }

    row_count = mysql_num_rows(mysql_result);
    if (row_count == 0) {
        array->count = 0;
        array->rows = NULL;
        mysql_free_result(mysql_result);
        return 0;
    }

    bytes = sizeof(FCFGEnvEntry) * row_count;
    array->rows = (FCFGEnvEntry *)malloc(bytes);
    if (array->rows == NULL) {
        logError("file: "__FILE__", line: %d, "
                "malloc %d bytes fail", __LINE__, bytes);
        array->count = 0;
        mysql_free_result(mysql_result);
        return ENOMEM;
    }

    array->count = row_count;
    end = array->rows + row_count;
    for (current=array->rows; current<end; current++) {
        if ((row=mysql_fetch_row(mysql_result)) == NULL) {
            logError("file: "__FILE__", line: %d, "
                    "call mysql_fetch_row fail, error info: %s",
                    __LINE__, mysql_error(context->mysql));
            array->count = current - array->rows;
            break;
        }

        current->env.str = strdup(row[0]);
        current->env.len = strlen(row[0]);
        if (current->env.str == NULL) {
            logError("file: "__FILE__", line: %d, "
                    "strdup %s fail", __LINE__, row[0]);
            array->count = current - array->rows;
            break;
        }
        current->create_time = strtoll(row[1], NULL, 10);
        current->update_time = strtoll(row[2], NULL, 10);
    }
    mysql_free_result(mysql_result);

    if (current < end) {
        fcfg_server_dao_free_env_array(array);
        return EFAULT;
    }

    return 0;
}

int fcfg_server_dao_list_env(FCFGMySQLContext *context, FCFGEnvArray *array)
{
    const char *env = NULL;
    return fcfg_server_dao_do_list_env(context, env, array);
}

int fcfg_server_dao_get_env(FCFGMySQLContext *context, const char *env,
        FCFGEnvEntry *entry)
{
    FCFGEnvArray array;
    int result;

    if ((result=fcfg_server_dao_do_list_env(context, env, &array)) != 0) {
        return result;
    }
    if (array.count == 0) {
        return ENOENT;
    }

    entry->env.len = strlen(env);
    entry->env.str = (char *)env;
    entry->create_time = array.rows[0].create_time;
    entry->update_time = array.rows[0].update_time;

    fcfg_server_dao_free_env_array(&array);
    return 0;
}

void fcfg_server_dao_free_env_array(FCFGEnvArray *array)
{
    FCFGEnvEntry *current;
    FCFGEnvEntry  *end;
    
    if (array->rows == NULL) {
        return;
    }

    end = array->rows + array->count;
    for (current=array->rows; current<end; current++) {
        if (current->env.str != NULL) {
            free(current->env.str);
        }
    }

    free(array->rows);
    array->rows = NULL;
    array->count = 0;
}

static int fcfg_server_dao_store_max_version(FCFGMySQLContext *context,
        MYSQL_STMT *stmt, int64_t *max_version)
{
    MYSQL_BIND result_binds[1];
    my_bool is_null;
    my_bool error;
    int row_count;
    int result;

    *max_version = 0;

    if ((result=MYSQL_STMT_EXECUTE(context, stmt)) != 0) {
        return result;
    }

    memset(result_binds, 0, sizeof(result_binds));
    result_binds[0].buffer_type = MYSQL_TYPE_LONGLONG;
    result_binds[0].buffer = (char *)max_version;
    result_binds[0].is_null = &is_null;
    result_binds[0].error = &error;
    if (mysql_stmt_bind_result(stmt, result_binds) != 0) {
        logError("file: "__FILE__", line: %d, "
                "call mysql_stmt_bind_result fail, error info: %s",
                __LINE__, mysql_stmt_error(stmt));
        return EINVAL;
    }

    if (mysql_stmt_store_result(stmt) != 0) {
        logError("file: "__FILE__", line: %d, "
                "call mysql_stmt_store_result fail, error info: %s",
                __LINE__, mysql_stmt_error(stmt));
        return EINVAL;
    }

    row_count = mysql_stmt_num_rows(stmt);
    if (row_count == 0) {
        return 0;
    }

    if (mysql_stmt_fetch(stmt) != 0) {
        logError("file: "__FILE__", line: %d, "
                "call mysql_stmt_fetch fail, error info: %s",
                __LINE__, mysql_stmt_error(stmt));
        return EFAULT;
    }

    return 0;
}

int fcfg_server_dao_max_config_version(FCFGMySQLContext *context,
            const char *env, int64_t *max_version)
{
    MYSQL_BIND select_binds[1];
    unsigned long env_len;
    int result;

    if ((result=FCFG_GET_MONITOR_MAX_CFG_VER_STMT(context)) != 0) {
        return result;
    }

    env_len = strlen(env);
    memset(select_binds, 0, sizeof(select_binds));

    select_binds[0].buffer_type = MYSQL_TYPE_STRING;
    select_binds[0].buffer = (char *)env;
    select_binds[0].length = &env_len;
    if (mysql_stmt_bind_param(context->monitor.max_cfg_ver_stmt, select_binds) != 0) {
        logError("file: "__FILE__", line: %d, "
                "call mysql_stmt_bind_param fail, error info: %s",
                __LINE__, mysql_stmt_error(context->monitor.max_cfg_ver_stmt));
        *max_version = 0;
        return EINVAL;
    }

    return fcfg_server_dao_store_max_version(context,
            context->monitor.max_cfg_ver_stmt, max_version);
}

int fcfg_server_dao_max_env_version(FCFGMySQLContext *context,
        int64_t *max_version)
{
    int result;

    if ((result=FCFG_GET_MONITOR_MAX_ENV_VER_STMT(context)) != 0) {
        return result;
    }

    return fcfg_server_dao_store_max_version(context,
            context->monitor.max_env_ver_stmt, max_version);
}
