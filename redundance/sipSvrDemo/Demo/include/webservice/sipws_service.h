#pragma once
//
// Copyright (c) 2014 Hikvision Digital Technology Co., Ltd.
//
// Summary:
//     SIP WebService Service 接口。
//
// Modification History:
//
#include "cmsws_defines.h"
#include "sipws_service_defines.h"

//
// Summary:
//     SIP WebService Service 接口。
//
#ifdef __cplusplus
extern "C"
{
#endif

    //
    // Summary:
    //     初始化 SIP WebService Service 。
    //
    // Parameters:
    //   [in] entry:
    //     sipws services 入口。
    //
    CMSWS_EXTERN void CMSWS_API SIPWS_SetServiceEntry(sipws_services_entry_t* entry);

    //
    // Summary:
    //     启动 SIP WebService Service 。
    //
    // Parameters:
    //   [in] uri:
    //     服务 URI 的相对路径，如 http://172.7.86.56:8080/HikCMS/services/ClientService 的相对路径为 /HikCMS/services/ClientService 。
    //
    // Returns:
    //     CMSWS_SUCCESS - 成功，CMSWS_FAILURE - 失败。
    //
    CMSWS_EXTERN int CMSWS_API SIPWS_StartService(const char* uri);

    //
    // Summary:
    //     停止 SIP WebService Service 。
    //
    CMSWS_EXTERN void CMSWS_API SIPWS_StopService(void);

    //
    // Summary:
    //     申请内存，并由 SIP WebService 托管。
    //
    // Parameters:
    //   [in] size:
    //     申请空间的尺寸。
    //
    // Returns:
    //     托管空间的地址。
    //
    CMSWS_EXTERN void* CMSWS_API SIPWS_Malloc(int size);

#ifdef __cplusplus
}
#endif
