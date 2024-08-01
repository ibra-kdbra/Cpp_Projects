#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "fastcommon/shared_func.h"
#include "fastcommon/pthread_func.h"
#include "fastcommon/process_ctrl.h"
#include "fastcommon/logger.h"
#include "fastcommon/sockopt.h"
#include "fastcommon/sched_thread.h"
#include "sf/sf_util.h"
#include "common/fcfg_proto.h"
#include "common/fcfg_global.h"
#include "fastcommon/sockopt.h"
#include "fcfg_agent_handler.h"
#include "fcfg_agent_func.h"
#include "fcfg_agent_global.h"

static bool daemon_mode = true;
static bool bTerminateFlag = false;
FCFGAgentGlobalVars g_agent_global_vars;
static int setup_server_env(const char *config_filename);
static int fcfg_agent_startup_schedule (pthread_t *schedule_tid);
static void usage(char *program)
{
    fprintf(stderr, "Usage: %s options, the options as:\n"
            "\t -h help\n"
            "\t -c <config-filename>\n"
            "\n", program);
}

int main (int argc, char **argv)
{
    char g_pid_filename[MAX_PATH_SIZE + 32];
    pthread_t schedule_tid;
    int wait_count = 0;
    char *action;
    bool stop;
    int r;
    failvars;

    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }
    memset(&g_agent_global_vars, 0, sizeof(FCFGAgentGlobalVars));
    g_agent_global_vars.continue_flag = true;
    g_agent_global_vars.config_file = argv[1];

    log_init2();
    log_set_use_file_write_lock(true);

    r = get_base_path_from_conf_file(g_agent_global_vars.
            config_file, g_agent_global_vars.base_path,
            sizeof(g_agent_global_vars.base_path));

    gofailif (r, "get base path fail");
    snprintf(g_pid_filename, sizeof(g_pid_filename),
            "%s/fcfg_agent.pid", g_agent_global_vars.base_path);

    sf_parse_daemon_mode_and_action(argc, argv, &g_fcfg_global_vars.
            version, &daemon_mode, &action);
    r = process_action(g_pid_filename, action, &stop);
    if (r == EINVAL) {
        usage(argv[0]);
        log_destroy();
        return 1;
    }
    gofailif(r, "process arg error");

    if (stop) {
        log_destroy();
        return 0;
    }

    r = setup_server_env(g_agent_global_vars.config_file);
     gofailif(r, "setup_server_env fail");

     tcp_dont_try_again_when_interrupt();
     r = write_to_pid_file(g_pid_filename);
     gofailif(r, "write pid error");

    r = fcfg_agent_startup_schedule(&schedule_tid);
    gofailif(r, "sf_startup_schedule fail");

    fcfg_agent_wait_config_server_loop();

    if (g_schedule_flag) {
        pthread_kill(schedule_tid, SIGINT);
    }
    while (g_schedule_flag) {
        usleep(10000);
        if (++wait_count > 1000) {
            lwarning("waiting timeout, exit!");
            break;
        }
    }
FAIL_:
    lerr("agent exit...");
    delete_pid_file(g_pid_filename);
    log_destroy();
    return 0;
}

static int fcfg_agent_startup_schedule (pthread_t *schedule_tid)
{
#define SCHEDULE_ENTRIES_COUNT 3

    ScheduleArray scheduleArray;
    ScheduleEntry scheduleEntries[SCHEDULE_ENTRIES_COUNT];
    int id;

    scheduleArray.entries = scheduleEntries;
    scheduleArray.count = 0;

    memset(scheduleEntries, 0, sizeof(scheduleEntries));

    id = scheduleArray.count++;
    INIT_SCHEDULE_ENTRY(scheduleEntries[id], id, TIME_NONE, TIME_NONE, 0,
            g_agent_global_vars.sync_log_buff_interval, log_sync_func, &g_log_context);

    if (g_agent_global_vars.rotate_error_log) {
        log_set_rotate_time_format(&g_log_context, "%Y%m%d");

        id = scheduleArray.count++;
        INIT_SCHEDULE_ENTRY(scheduleEntries[id], id, 0, 0, 0,
                86400, log_notify_rotate, &g_log_context);

        if (g_agent_global_vars.log_file_keep_days > 0) {
            log_set_keep_days(&g_log_context, g_agent_global_vars.log_file_keep_days);

            id = scheduleArray.count++;
            INIT_SCHEDULE_ENTRY(scheduleEntries[id], id, 1, 0, 0,
                    86400, log_delete_old_files, &g_log_context);
        }
    }
    return sched_start(&scheduleArray, schedule_tid,
            g_agent_global_vars.thread_stack_size, (bool * volatile)
            &g_agent_global_vars.continue_flag);
}

static void sigQuitHandler(int sig)
{
    if (!bTerminateFlag) {
        bTerminateFlag = true;
        g_agent_global_vars.continue_flag = false;
        logCrit("file: "__FILE__", line: %d, " \
            "catch signal %d, program exiting...", \
            __LINE__, sig);
    }
}

static void sigHupHandler(int sig)
{
    logInfo("file: "__FILE__", line: %d, " \
        "catch signal %d", __LINE__, sig);
}

static void sigUsrHandler(int sig)
{
    logInfo("file: "__FILE__", line: %d, "
        "catch signal %d, ignore it", __LINE__, sig);
}

static int fcfg_setup_signal_handler()
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);

    act.sa_handler = sigUsrHandler;
    if(sigaction(SIGUSR1, &act, NULL) < 0 ||
        sigaction(SIGUSR2, &act, NULL) < 0)
    {
        logCrit("file: "__FILE__", line: %d, "
            "call sigaction fail, errno: %d, error info: %s",
            __LINE__, errno, strerror(errno));
        logCrit("exit abnormally!\n");
        return errno;
    }

    act.sa_handler = sigHupHandler;
    if(sigaction(SIGHUP, &act, NULL) < 0) {
        logCrit("file: "__FILE__", line: %d, "
            "call sigaction fail, errno: %d, error info: %s",
            __LINE__, errno, strerror(errno));
        logCrit("exit abnormally!\n");
        return errno;
    }

    act.sa_handler = SIG_IGN;
    if(sigaction(SIGPIPE, &act, NULL) < 0) {
        logCrit("file: "__FILE__", line: %d, "
            "call sigaction fail, errno: %d, error info: %s",
            __LINE__, errno, strerror(errno));
        logCrit("exit abnormally!\n");
        return errno;
    }

    act.sa_handler = sigQuitHandler;
    if(sigaction(SIGINT, &act, NULL) < 0 ||
        sigaction(SIGTERM, &act, NULL) < 0 ||
        sigaction(SIGQUIT, &act, NULL) < 0)
    {
        logCrit("file: "__FILE__", line: %d, "
            "call sigaction fail, errno: %d, error info: %s",
            __LINE__, errno, strerror(errno));
        logCrit("exit abnormally!\n");
        return errno;
    }

    return 0;
}

static int setup_server_env(const char *config_filename)
{
    int result;

    result = fcfg_agent_load_config(config_filename);
    if (result != 0) {
        fprintf(stderr, "load from conf file %s fail, "
                "erro no: %d, error info: %s",
                config_filename, result, strerror(result));
        return result;
    }

    if (daemon_mode) {
        daemon_init(false);
    }
    umask(0);

    result = fcfg_setup_signal_handler();
    returnif(result);

    result = fcfg_agent_shm_init();
    log_set_cache(true);
    return 0;
}
