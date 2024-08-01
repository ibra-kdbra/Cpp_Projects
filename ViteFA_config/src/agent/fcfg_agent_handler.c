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
static void _print_push_config (int status, struct shmcache_key_info *key,
        struct shmcache_value_info *value, int64_t max_version)
{
    if (status == FCFG_CONFIG_STATUS_NORMAL) {
        linfo("push conifg. status: %d, version: %"PRId64", key: %.*s, "
                "value: %.*s, options: %d",
                status, max_version, key->length, key->data,
                value->length, value->data, value->options);
    } else {
        linfo("push conifg. status: %d, version: %"PRId64", key: %.*s",
                status, max_version, key->length, key->data);
    }

    return;
}

static int _get_options_from_type(unsigned char type)
{
    int options;

    if (type == FCFG_CONFIG_TYPE_STRING) {
        options = SHMCACHE_SERIALIZER_STRING;
    } else if (type == FCFG_CONFIG_TYPE_LIST) {
        options = SHMCACHE_SERIALIZER_LIST;
    } else if (type == FCFG_CONFIG_TYPE_MAP) {
        options = SHMCACHE_SERIALIZER_MAP;
    } else {
        options = SHMCACHE_SERIALIZER_STRING;
    }

    return options;
}

static int fcfg_set_push_config(const char *body_data,
        const int body_len, int64_t *max_version)
{
    int ret;
    int i;
    int size;
    FCFGPushConfigHeader fcfg_push_header;
    FCFGProtoPushConfigHeader *fcfg_push_header_pro;
    FCFGProtoPushConfigBodyPart *fcfg_push_body_pro;
    FCFGPushConfigBodyPart fcfg_push_body_data;
    struct shmcache_key_info key;
    struct shmcache_value_info value;

    fcfg_push_header_pro = (FCFGProtoPushConfigHeader *)(body_data);
    fcfg_extract_push_config_header(fcfg_push_header_pro, &fcfg_push_header);

    fcfg_push_body_pro = (FCFGProtoPushConfigBodyPart *)(fcfg_push_header_pro +
            1);
    ret = fcfg_check_push_config_body_len(&fcfg_push_header, fcfg_push_body_pro,
            body_len - sizeof(FCFGProtoPushConfigHeader));
    if (ret) {
        lerr("fcfg_check_push_config_body_len fail.count:%d",
                fcfg_push_header.count);
        return ret;
    }
    size = sizeof(FCFGProtoPushConfigBodyPart);
    for (i = 0; i < fcfg_push_header.count; i++) {
        fcfg_extract_push_config_body_data(fcfg_push_body_pro, &fcfg_push_body_data);
        key.data = fcfg_push_body_pro->name;
        key.length = fcfg_push_body_data.name_len;
        if (fcfg_push_body_data.status == FCFG_CONFIG_STATUS_NORMAL) {
            value.data = fcfg_push_body_pro->name + fcfg_push_body_data.name_len;
            value.length = fcfg_push_body_data.value_len;
            value.options = _get_options_from_type(fcfg_push_body_data.type);
            value.expires = SHMCACHE_NEVER_EXPIRED;
            ret = shmcache_set_ex(&g_agent_global_vars.shm_context, &key, &value);
        } else {
            ret = shmcache_delete(&g_agent_global_vars.shm_context, &key);
        }

        _print_push_config(fcfg_push_body_data.status,
                &key, &value, fcfg_push_body_data.version);
        if (ret) {
            lerr ("shmcache_set_ex/delete fail. status:%d, %d, %s",
                    fcfg_push_body_data.status, ret, strerror(ret));
            if (fcfg_push_body_data.status == FCFG_CONFIG_STATUS_NORMAL) {
                break;
            }
            ret = 0;
        }

        /* the last one is the max version that is ensured by sender */
        *max_version = fcfg_push_body_data.version;

        fcfg_push_body_pro = (FCFGProtoPushConfigBodyPart *)(((char *)fcfg_push_body_pro) + size +
                              fcfg_push_body_data.name_len +
                              fcfg_push_body_data.value_len);
    }

    if (fcfg_push_header.count && (ret == 0)) {
        ret = fcfg_agent_set_config_version(*max_version);
    }

    return ret;
}


