#ifndef _FCFG_TYPES_SERVER_H
#define _FCFG_TYPES_SERVER_H

#include <time.h>
#include "fastcommon/common_define.h"

typedef struct {
    unsigned char cmd;  //response command
    int body_len;       //response body length
} FCFGRequestInfo;

typedef struct {
    struct {
        char message[FCFG_ERROR_INFO_SIZE];
        int length;
    } error;
    int status;
    int body_len;    //response body length
    bool response_done;
    bool log_error;
    unsigned char cmd;   //response command
} FCFGResponseInfo;

#endif
