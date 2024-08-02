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
#include "fastcommon/sockopt.h"
#include "fcfg_proto.h"
#include "fcfg_admin_func.h"
#include "fcfg_types.h"

void fcfg_set_admin_add_env(char *buff, const char *env,
        int *body_len)
{
    FCFGProtoAddEnvReq *add_env_req = (FCFGProtoAddEnvReq *)buff;
    unsigned char env_len = strlen(env);
    memcpy(add_env_req->env, env,
           env_len);
    *body_len = sizeof(FCFGProtoAddEnvReq) + env_len;
}
}
