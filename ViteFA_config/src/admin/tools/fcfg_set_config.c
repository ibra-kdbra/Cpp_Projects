#include<stdio.h>
#include <ctype.h>
#include "fcfg/fcfg_admin.h"
#include "fastcommon/logger.h"

static bool show_usage = false;
char *config_file = NULL;
char *config_name = NULL;
char *env = NULL;
char *config_value = NULL;
char type_char  = '\0';
unsigned char type = FCFG_CONFIG_TYPE_NONE;
static void usage(char *program)
{
    fprintf(stderr, "Usage: %s options, the options as:\n"
            "\t -h help\n"
            "\t -c <config-filename>\n"
            "\t -e <env>\n"
            "\t -n <config-name>\n"
            "\t -v <config-value>\n"
            "\t -t <value-type. s: string, l: list, m: map. if not set, "
            "recognize automatically>\n"
            "\n", program);
}
static void parse_args(int argc, char **argv)
{
    int ch;
    int found = 0;

    while ((ch = getopt(argc, argv, "hc:e:n:v:t:")) != -1) {
        found = 1;
        switch (ch) {
            case 'c':
                config_file = optarg;
                break;
            case 'n':
                config_name = optarg;
                break;
            case 'v':
                config_value = optarg;
                break;
            case 'e':
                env = optarg;
                break;
            case 't':
                type_char = *optarg;
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
        config_name == NULL ||
        config_value == NULL) {
        show_usage = true;
    }
}

int main (int argc, char **argv)
{
    int ret;
    struct fcfg_context fcfg_context;
    if (argc < 9) {
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

    type = FCFG_CONFIG_TYPE_NONE;
    if (type_char) {
        if (tolower(type_char) == 'l') {
            type = FCFG_CONFIG_TYPE_LIST;
        } else if (tolower(type_char) == 'm') {
            type = FCFG_CONFIG_TYPE_MAP;
        } else if (tolower(type_char) == 's') {
            type = FCFG_CONFIG_TYPE_STRING;
        }
    }

    ret = fcfg_admin_config_set(&fcfg_context, env, config_name, config_value,
            type);
    if (ret == 0) {
        fprintf(stderr, "set config success\n");
    }

END:
    log_destroy();
    fcfg_admin_destroy(&fcfg_context);
    return ret;
}
