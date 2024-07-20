#ifndef _FCFG_PROTO_H
#define _FCFG_PROTO_H

#include "fastcommon/fast_task_queue.h"
#include "fastcommon/shared_func.h"
#include "fastcommon/logger.h"
#include "fastcommon/connection_pool.h"
#include "fastcommon/ini_file_reader.h"
#include "fcfg_types.h"

#define FCFG_PROTO_ACK                 6

#define FCFG_PROTO_ACTIVE_TEST_REQ    35   //center -> agent
#define FCFG_PROTO_ACTIVE_TEST_RESP   36

#define FCFG_PROTO_AGENT_JOIN_REQ     37   //agent -> center
#define FCFG_PROTO_AGENT_JOIN_RESP    38

#define FCFG_PROTO_PUSH_CONFIG        39   //center -> agent
#define FCFG_PROTO_PUSH_RESP          40


#define FCFG_PROTO_ADMIN_JOIN_REQ     41  //amdin -> center

#define FCFG_PROTO_SET_CONFIG_REQ     43  //admin -> center
#define FCFG_PROTO_DEL_CONFIG_REQ     45  //admin -> center
#define FCFG_PROTO_GET_CONFIG_REQ     47  //admin -> center
#define FCFG_PROTO_GET_CONFIG_RESP    48

#define FCFG_PROTO_LIST_CONFIG_REQ    49  //admin -> center
#define FCFG_PROTO_LIST_CONFIG_RESP   50

#define FCFG_PROTO_ADD_ENV_REQ        51  //admin -> center
#define FCFG_PROTO_DEL_ENV_REQ        53  //admin -> center
#define FCFG_PROTO_GET_ENV_REQ        55  //admin -> center
#define FCFG_PROTO_GET_ENV_RESP       56

#define FCFG_PROTO_LIST_ENV_REQ       57  //admin -> center
#define FCFG_PROTO_LIST_ENV_RESP      58


typedef struct fcfg_proto_header {
    char body_len[4];       //body length
    unsigned char cmd;      //the command code
    unsigned char status;   //status to store errno
    char padding[2];
} FCFGProtoHeader;

typedef struct fcfg_proto_agent_join_req {
    char env[64];
    char agent_cfg_version[8];
} FCFGProtoAgentJoinReq;

typedef struct fcfg_proto_agent_join_resp {
    char center_cfg_version[8];
} FCFGProtoAgentJoinResp;

typedef struct fcfg_proto_push_config_header {
    char count[2];  //config count in body
} FCFGProtoPushConfigHeader;

typedef struct fcfg_proto_push_config_body_part {
    unsigned char status;
    unsigned char name_len;
    unsigned char type;
    char value_len[4];
    char version[8];
    char create_time[4];  //unix timestamp
    char update_time[4];  //unix timestamp
    char name[0];
    //char *value;   //value = name + name_len
} FCFGProtoPushConfigBodyPart;

typedef struct fcfg_proto_push_resp {
    char agent_cfg_version[8];
} FCFGProtoPushResp;

typedef struct fcfg_proto_admin_join_req {
    unsigned char username_len;
    unsigned char secret_key_len;
    char username[0];
    //char *secret_key;   //secret_key = username + username_len
} FCFGProtoAdminJoinReq;

typedef struct fcfg_proto_set_config_req {
    unsigned char env_len;
    unsigned char name_len;
    unsigned char type;
    char value_len[4];
    char env[0];
    //char *name;    //name = env + env_len
    //char *value;   //value = name + name_len
} FCFGProtoSetConfigReq;

typedef struct fcfg_proto_del_config_req {
    unsigned char env_len;
    unsigned char name_len;
    char env[0];
    //char *name;    //name = env + env_len
} FCFGProtoDelConfigReq;

typedef struct fcfg_proto_get_config_req {
    unsigned char env_len;
    unsigned char name_len;
    char env[0];
    //char *name;    //name = env + env_len
} FCFGProtoGetConfigReq;

typedef FCFGProtoPushConfigBodyPart FCFGProtoGetConfigResp;

typedef struct fcfg_proto_list_config_req {
    unsigned char env_len;
    unsigned char name_len;
    struct {
        char offset[2];
        char count[2];
    } limit;  //mysql limit
    char env[0];
    //char *name;    //name = env + env_len
} FCFGProtoListConfigReq;

typedef struct fcfg_proto_list_config_resp_header {
    char count[2];  //config count in body
} FCFGProtoListConfigRespHeader;

typedef FCFGProtoGetConfigResp FCFGProtoListConfigRespBodyPart;


typedef struct fcfg_proto_add_env_req {
    char env[0];
} FCFGProtoAddEnvReq;

typedef struct fcfg_proto_del_env_req {
    char env[0];
} FCFGProtoDelEnvReq;


typedef struct fcfg_proto_get_env_req {
    char env[0];
} FCFGProtoGetEnvReq;

typedef struct fcfg_proto_get_env_resp {
    unsigned char env_len;
    char create_time[4];  //unix timestamp
    char update_time[4];  //unix timestamp
    char env[0];
} FCFGProtoGetEnvResp;


typedef struct fcfg_proto_list_env_req {
} FCFGProtoListEnvReq;

typedef struct fcfg_proto_list_env_resp_header {
    char count[2];  //env count in body
} FCFGProtoListEnvRespHeader;

typedef FCFGProtoGetEnvResp FCFGProtoListEnvRespBodyPart;

#ifdef __cplusplus
extern "C" {
#endif

void fcfg_proto_init();

int fcfg_proto_set_body_length(struct fast_task_info *task);

int fcfg_proto_deal_actvie_test(struct fast_task_info *task,
        const FCFGRequestInfo *request, FCFGResponseInfo *response);

int send_and_recv_response_header(ConnectionInfo *conn, char *data, int len,
        FCFGResponseInfo *resp_info, int network_timeout);

void fcfg_proto_response_extract (FCFGProtoHeader *header_pro,
        FCFGResponseInfo *resp_info);

int fcfg_send_active_test_req(ConnectionInfo *conn, FCFGResponseInfo *resp_info,
        int network_timeout);
int fcfg_check_response(ConnectionInfo *join_conn,
        FCFGResponseInfo *resp_info, int network_timeout,
        unsigned char resp_cmd);

static inline int fcfg_proto_expect_body_length(struct fast_task_info *task,
        const FCFGRequestInfo *request, FCFGResponseInfo *response,
        const int expect_body_length)
{
    if (request->body_len != expect_body_length) {
        response->error.length = sprintf(response->error.message,
                "request body length: %d != %d",
                request->body_len, expect_body_length);
        return EINVAL;
    }

    return 0;
}

static inline int fcfg_proto_check_min_body_length(struct fast_task_info *task,
        const FCFGRequestInfo *request, FCFGResponseInfo *response,
        const int min_body_length)
{
    if (request->body_len < min_body_length) {
        response->error.length = sprintf(response->error.message,
                "request body length: %d < %d",
                request->body_len, min_body_length);
        return EINVAL;
    }

    return 0;
}

static inline int fcfg_proto_check_max_body_length(struct fast_task_info *task,
        const FCFGRequestInfo *request, FCFGResponseInfo *response,
        const int max_body_length)
{
    if (request->body_len > max_body_length) {
        response->error.length = sprintf(response->error.message,
                "request body length: %d > %d",
                request->body_len, max_body_length);
        return EINVAL;
    }

    return 0;
}

static inline int fcfg_proto_check_body_length(struct fast_task_info *task,
        const FCFGRequestInfo *request, FCFGResponseInfo *response,
        const int min_body_length, const int max_body_length)
{
    int result;
    if ((result=fcfg_proto_check_min_body_length(task, request, response,
            min_body_length)) != 0)
    {
        return result;
    }
    return fcfg_proto_check_max_body_length(task, request, response,
            max_body_length);
}

#define FCFG_PROTO_EXPECT_BODY_LEN(task, request, response, expect_length) \
    fcfg_proto_expect_body_length(task, request, response, expect_length)

#define FCFG_PROTO_CHECK_MIN_BODY_LEN(task, request, response, min_length) \
    fcfg_proto_check_min_body_length(task, request, response, min_length)

#define FCFG_PROTO_CHECK_MAX_BODY_LEN(task, request, response, max_length) \
    fcfg_proto_check_max_body_length(task, request, response, max_length)

#define FCFG_PROTO_CHECK_BODY_LEN(task, request, response, min_length, max_length) \
    fcfg_proto_check_body_length(task, request, response, \
            min_length, max_length)

#ifdef __cplusplus
}
#endif

#endif
