#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "fastcommon/logger.h"
#include "fastcommon/sockopt.h"
#include "fastcommon/shared_func.h"
#include "fastcommon/pthread_func.h"
#include "fastcommon/sched_thread.h"
#include "fastcommon/ioevent_loop.h"
#include "sf/sf_util.h"
#include "common/fcfg_proto.h"
#include "fcfg_agent_global.h"
#include "fcfg_agent_func.h"
#include "fcfg_agent_handler.h"

static int fcfg_agent_set_config_version(int64_t version)
{
    int ret;
    struct shmcache_value_info value;
    struct shmcache_key_info key;
    char buff[32];
    
    key.data = g_agent_global_vars.shm_version_key;
    key.length = strlen(g_agent_global_vars.shm_version_key);
    value.length = snprintf(buff, sizeof(buff), "%"PRId64, version);
    value.data = buff;
    value.expires = SHMCACHE_NEVER_EXPIRED;
    value.options = SHMCACHE_SERIALIZER_STRING;
    ret = shmcache_set_ex(&g_agent_global_vars.shm_context,
            &key,
            &value);
    if (ret) {
        lerr("shmcache_set_ex set config version fail:%d, %s, version:%"PRId64,
                ret, strerror(ret), version);
    } else {
        linfo("shmcache_set_ex set config version success. version:%"PRId64,
                 version);
    }

    return ret;
}

static int fcfg_agent_get_config_version(int64_t *version)
{
    char buff[FCFG_CONFIG_ENV_SIZE];
    int ret;
    struct shmcache_value_info value;
    struct shmcache_key_info key;
    
    key.data = g_agent_global_vars.shm_version_key;
    key.length = strlen(g_agent_global_vars.shm_version_key);
    ret = shmcache_get(&g_agent_global_vars.shm_context,
            &key,
            &value);
    if (ret) {
        lerr("shmcache_get fail:%d, %s", ret, strerror(ret));
        ret = shmcache_clear(&g_agent_global_vars.shm_context);
        if (ret) {
            lerr("shmcache_remove_all fail. %d, %s. new env: %s",
                 ret, strerror(ret), g_agent_global_vars.env);
            return ret;
        } else {
            linfo("shmcache_remove_all for new env: %s",
                   g_agent_global_vars.env);
        }

        *version = 0;
        return ret;
    }
    memcpy(buff, value.data, value.length);
    buff[value.length] = '\0';
    *version = atol(buff);

    return ret;
}




