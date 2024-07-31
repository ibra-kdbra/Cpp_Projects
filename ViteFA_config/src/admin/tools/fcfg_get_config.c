#include<stdio.h>
#include "fcfg/fcfg_admin.h"
#include "fastcommon/logger.h"

static bool show_usage = false;
char *config_file = NULL;
char *config_name = NULL;
char *env = NULL;
static void usage(char *program)
{
    fprintf(stderr, "Usage: %s options, the options as:\n"
            "\t -h help\n"
            "\t -c <config-filename>\n"
            "\t -e <env>\n"
            "\t -n <config-name>\n"
            "\n", program);
}
static void parse_args(int argc, char **argv)
{
    int ch;
    int found = 0;

    while ((ch = getopt(argc, argv, "hc:e:n:")) != -1) {
        found = 1;
        switch (ch) {
            case 'c':
                config_file = optarg;
                break;
            case 'n':
                config_name = optarg;
                break;
            case 'e':
                env = optarg;
                break;
            case 'h':
            default:
                show_usage = true;
                break;
        }
    }
    if (found == 0 ||
        config_file == NULL ||
        env == NULL ||
        config_name == NULL) {
        show_usage = true;
    }
}

int main (int argc, char **argv)
{
    int ret;
    struct fcfg_context fcfg_context;
    FCFGConfigArray array;
    memset(&array, 0, sizeof(FCFGEnvArray));
    if (argc < 7) {
        usage(argv[0]);
        return 0;
    }
    parse_args(argc, argv);
    if (show_usage) {
        usage(argv[0]);
        return 0;
    }
    log_init2();

    ret = fcfg_admin_init_from_file(&fcfg_context, config_file);
    if (ret) {
        goto END;
    }
    ret = fcfg_admin_config_get(&fcfg_context, env, config_name, &array);
    if (ret == 0) {
        fcfg_print_config_array(&array, 1);
    }

END:
    log_destroy();
    fcfg_free_config_info_array(&array);
    fcfg_admin_destroy(&fcfg_context);
    return ret;
}
