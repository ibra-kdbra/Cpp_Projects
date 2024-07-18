
#ifndef _FCFG_AGENT_FUNC_H
#define _FCFG_AGENT_FUNC_H

#include "fcfg_agent_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int fcfg_agent_load_config(const char *filename);
int fcfg_proto_set_join_req(char *buff, char *env,
        int64_t version, int *req_len);
int fcfg_extract_join_resp(FCFGJoinResp *join_resp_data,
        FCFGProtoAgentJoinResp *join_resp_pro);
int fcfg_extract_push_config_header(
        FCFGProtoPushConfigHeader *fcfg_push_header_pro,
        FCFGPushConfigHeader *fcfg_push_header);
int fcfg_check_push_config_body_len(FCFGPushConfigHeader *fcfg_push_header,
        FCFGProtoPushConfigBodyPart *fcfg_push_body_pro, int len);
int fcfg_extract_push_config_body_data (
        FCFGProtoPushConfigBodyPart *fcfg_push_body_pro,
        FCFGPushConfigBodyPart *fcfg_push_body_data);
void fcfg_proto_response_extract (FCFGProtoHeader *header_pro,
        FCFGResponseInfo *resp_info);
int fcfg_agent_load_config(const char *filename);
#define FCFG_AGENT_SHM_VERSION_KEY_SUFFIX   "shm_version"

#define FCFG_SYNC_LOG_BUFF_DEF_INTERVAL 10
#define FCFG_DEF_THREAD_STACK_SIZE (64 * 1024)

#ifdef __cplusplus
}
#endif

#endif
