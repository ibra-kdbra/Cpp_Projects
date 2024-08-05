
#include <sys/stat.h>
#include "fastcommon/shared_func.h"
#include "fastcommon/logger.h"
#include "fastcommon/pthread_func.h"
#include "fastcommon/sched_thread.h"
#include "fastcommon/fc_list.h"
#include "sf/sf_global.h"
#include "common/fcfg_types.h"
#include "fcfg_server_global.h"
#include "fcfg_server_dao.h"
#include "fcfg_server_cfg.h"

typedef struct {
    FCFGEnvPublisher **envs;
    int count;
    int alloc;
    pthread_mutex_t lock;
} FCFGPublisherArray;

static FCFGPublisherArray publisher_array = {NULL, 0, 0};

static int64_t current_config_version = 0;

static struct fast_mblock_man event_allocator;

static int fcfg_server_cfg_free_config_array(void *args)
{
    fcfg_server_dao_free_config_array((FCFGConfigArray *)args);
    free(args);
    return 0;
}

static int check_alloc_config_array(FCFGConfigArray **array, const int inc)
{
    FCFGConfigArray *old_array;
    FCFGConfigArray *new_array;
    int target_count;
    int result;
    int bytes;

    target_count = (*array)->count + inc;
    if ((*array)->alloc >= target_count) {
        return 0;
    }

    new_array = (FCFGConfigArray *)malloc(sizeof(FCFGConfigArray));
    if (new_array == NULL) {
        logError("file: "__FILE__", line: %d, "
                "malloc %d bytes fail",
                __LINE__, (int)sizeof(FCFGConfigArray));
        return ENOMEM;
    }

    new_array->alloc = ((*array)->alloc > 0) ? (*array)->alloc : 8;
    while (new_array->alloc < target_count) {
        new_array->alloc *= 2;
    }

    bytes = sizeof(FCFGConfigEntry) * new_array->alloc;
    new_array->rows = (FCFGConfigEntry *)malloc(bytes);
    if (new_array->rows == NULL) {
        logError("file: "__FILE__", line: %d, "
                "malloc %d bytes fail", __LINE__, bytes);
        return ENOMEM;
    }

    new_array->count = 0;
    new_array->version = __sync_add_and_fetch(&current_config_version, 1);
    if ((result=fcfg_server_dao_copy_config_array(*array, new_array)) != 0) {
        return result;
    }

    old_array = *array;
    *array = new_array;
    return sched_add_delay_task(fcfg_server_cfg_free_config_array,
            old_array, 10 * SF_G_NETWORK_TIMEOUT, false);
}

