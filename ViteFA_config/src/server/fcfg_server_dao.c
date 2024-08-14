
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

