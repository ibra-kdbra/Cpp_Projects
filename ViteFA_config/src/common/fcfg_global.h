#ifndef _FCFG_GLOBAL_H
#define _FCFG_GLOBAL_H

#include "fcfg_types.h"

typedef struct fcfg_global_vars {
    Version version;
} FCFGGlobalVars;

#ifdef __cplusplus
extern "C" {
#endif

    extern FCFGGlobalVars g_fcfg_global_vars;

#ifdef __cplusplus
}
#endif

#endif
