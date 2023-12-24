#pragma once
//
// Copyright (c) 2009 Hikvision Digital Technology Co., Ltd.
//
// Summary:
//     CMS WebService 服务器接口。
//
// Modification History:
//   2009-10-11:
//     V1.0 / 胡通海 / 创建
//
#include "cmsws_defines.h"


//
// Summary:
//     设定结构体最大按 4 字节对齐。
//
// #pragma pack(4)

typedef struct cmsws_start_server_param
{
    //
    // Summary:
    //     server ip
    //
    const char* ip;
    //
    // Summary:
    //     server port
    //
    int port;
    //
    // Summary:
    //     request backlog
    //
    int backlog;
    //
    // Summary:
    //     send timeout (second)
    //
    int sendTimeout;
    //
    // Summary:
    //     receive timeout (second)
    //
    int recvTimeout;
    //
    // Summary:
    //     size of thread pool
    //
    int maxResponseThread;
    //
    // Summary:
    //     size of request queue
    //
    int maxRequestQueue;
    //
    // Summary:
    //     max keep alive
    //
    int maxKeepAlive;

} ServerStartParam;

//
// Summary:
//     恢复结构体默认对齐方式。
//
// #pragma pack()


//
// Summary:
//     CMS WebService 服务入口函数。
//
// Parameters:
//   [in] soapEnv:
//     CMS WebService runtime environment.
//
// Returns:
//     SOAP_OK - 成功，否则返回错误值。
//
typedef int (__cdecl *CMSWS_ServiceServeRequest)(cmsws_soap* soapEnv);


//
// Summary:
//     CMS WebService 服务器接口。
//
#ifdef __cplusplus
extern "C"
{
#endif

    //
    // Summary:
    //     初始化 CMS WebService 服务器。
    //
    // Returns:
    //     CMSWS_SUCCESS - 成功，CMSWS_FAILURE - 失败。
    //
    CMSWS_EXTERN int CMSWS_API CMSWS_InitServer(void);

    //
    // Summary:
    //     释放 CMS WebService 服务器资源。
    //
    CMSWS_EXTERN void CMSWS_API CMSWS_UninitServer(void);

    //
    // Summary:
    //     向 CMS WebService 服务器注册服务。
    //
    // Parameters:
    //   [in] uri:
    //     服务 URI 的相对路径，如 http://172.7.86.56:8080/HikCMS/services/ClientService 的相对路径为 /HikCMS/services/ClientService 。
    //   [in] entry:
    //     服务函数入口。
    //   [in] ns:
    //     CMS WebService envelope ns 。
    //
    // Returns:
    //     CMSWS_SUCCESS - 成功，CMSWS_FAILURE - 失败。
    //
    CMSWS_EXTERN int CMSWS_API CMSWS_RegisterService(const char* uri, CMSWS_ServiceServeRequest entry, cmsws_namespace* ns);

    //
    // Summary:
    //     取消 CMS WebService 服务器上已经注册的服务。
    //
    // Parameters:
    //   [in] uri:
    //     服务 URI 的相对路径，如 http://172.7.86.56:8080/HikCMS/services/ClientService 的相对路径为 /HikCMS/services/ClientService 。
    //
    CMSWS_EXTERN void CMSWS_API CMSWS_UnregisterService(const char* uri);

    //
    // Summary:
    //     启动 CMS WebService 服务器。
    //
    // Parameters:
    //   [in] param:
    //     启动参数。
    //
    // Returns:
    //     CMSWS_SUCCESS - 成功，CMSWS_FAILURE - 失败。
    //
    CMSWS_EXTERN int CMSWS_API CMSWS_StartServer(ServerStartParam* param);

    //
    // Summary:
    //     停止 CMS WebService 服务器。
    //
    CMSWS_EXTERN void CMSWS_API CMSWS_StopServer(void);

#ifdef __cplusplus
}
#endif
