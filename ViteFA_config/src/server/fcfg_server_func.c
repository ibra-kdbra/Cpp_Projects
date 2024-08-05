
#include <sys/stat.h>
#include "fastcommon/ini_file_reader.h"
#include "fastcommon/shared_func.h"
#include "fastcommon/logger.h"
#include "sf/sf_global.h"
#include "sf/sf_service.h"
#include "fcfg_server_types.h"
#include "fcfg_server_global.h"
#include "fcfg_server_func.h"

static int fcfg_server_load_db_config(IniContext *ini_context)
{
#define DB_SECTION_NAME "mysql"

    char *host;
    char *user;
    char *password;
    char *database;
    char *buff;
    char *p;
    struct {
        int host;
        int user;
        int password;
        int database;
    } lengths;
    int bytes;

    if ((host=iniGetRequiredStrValue(DB_SECTION_NAME, "host",
                    ini_context)) == NULL)
    {
        return ENOENT;
    }
    g_server_global_vars.db_config.port = iniGetIntValue(DB_SECTION_NAME,
            "port", ini_context, 3306);
    if ((user=iniGetRequiredStrValue(DB_SECTION_NAME, "user",
                    ini_context)) == NULL)
    {
        return ENOENT;
    }
    if ((password=iniGetRequiredStrValueEx(DB_SECTION_NAME, "password",
                    ini_context, 0)) == NULL)
    {
        return ENOENT;
    }
    if ((database=iniGetRequiredStrValue(DB_SECTION_NAME, "database",
                    ini_context)) == NULL)
    {
        return ENOENT;
    }
    g_server_global_vars.db_config.ping_interval = iniGetIntValue(
            DB_SECTION_NAME, "ping_interval", ini_context, 14400);

    lengths.host = strlen(host) + 1;
    lengths.user = strlen(user) + 1;
    lengths.password = strlen(password) + 1;
    lengths.database = strlen(database) + 1;

    bytes = lengths.host + lengths.user + lengths.password + lengths.database;
    buff = (char *)malloc(bytes);
    if (buff == NULL) {
        logError("file: "__FILE__", line: %d, "
                "malloc %d bytes fail", __LINE__, bytes);
        return ENOMEM;
    }

    p = buff;
    g_server_global_vars.db_config.host = p;
    p += lengths.host;

    g_server_global_vars.db_config.user = p;
    p += lengths.user;

    g_server_global_vars.db_config.password = p;
    p += lengths.password;

    g_server_global_vars.db_config.database = p;
    p += lengths.database;

    memcpy(g_server_global_vars.db_config.host, host, lengths.host);
    memcpy(g_server_global_vars.db_config.user, user, lengths.user);
    memcpy(g_server_global_vars.db_config.password, password, lengths.password);
    memcpy(g_server_global_vars.db_config.database, database, lengths.database);

    return 0;
}

static int fcfg_server_load_admin_config(IniContext *ini_context)
{
#define ADMIN_SECTION_NAME "admin"

    char *username;
    char *secret_key;
    char *buff;
    char *p;
    struct {
        int username;
        int secret_key;
    } lengths;
    int bytes;

    if ((username=iniGetRequiredStrValue(ADMIN_SECTION_NAME, "username",
                    ini_context)) == NULL)
    {
        return ENOENT;
    }

    if ((secret_key=iniGetRequiredStrValue(ADMIN_SECTION_NAME, "secret_key",
                    ini_context)) == NULL)
    {
        return ENOENT;
    }

    g_server_global_vars.admin.username.len = strlen(username);
    g_server_global_vars.admin.secret_key.len = strlen(secret_key);

    lengths.username = g_server_global_vars.admin.username.len + 1;
    lengths.secret_key = g_server_global_vars.admin.secret_key.len + 1;

    bytes = lengths.username + lengths.secret_key;
    buff = (char *)malloc(bytes);
    if (buff == NULL) {
        logError("file: "__FILE__", line: %d, "
                "malloc %d bytes fail", __LINE__, bytes);
        return ENOMEM;
    }

    p = buff;
    g_server_global_vars.admin.username.str = p;
    p += lengths.username;

    g_server_global_vars.admin.secret_key.str = p;
    p += lengths.secret_key;

    memcpy(g_server_global_vars.admin.username.str, username, lengths.username);
    memcpy(g_server_global_vars.admin.secret_key.str, secret_key, lengths.secret_key);
    return 0;
}
