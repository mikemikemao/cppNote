#pragma once
//
// Copyright (c) 2009 Hikvision Digital Technology Co., Ltd.
//
// Summary:
//     CMS WebService �������ӿڡ�
//
// Modification History:
//   2009-10-11:
//     V1.0 / ��ͨ�� / ����
//
#include "cmsws_defines.h"


//
// Summary:
//     �趨�ṹ����� 4 �ֽڶ��롣
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
//     �ָ��ṹ��Ĭ�϶��뷽ʽ��
//
// #pragma pack()


//
// Summary:
//     CMS WebService ������ں�����
//
// Parameters:
//   [in] soapEnv:
//     CMS WebService runtime environment.
//
// Returns:
//     SOAP_OK - �ɹ������򷵻ش���ֵ��
//
typedef int (__cdecl *CMSWS_ServiceServeRequest)(cmsws_soap* soapEnv);


//
// Summary:
//     CMS WebService �������ӿڡ�
//
#ifdef __cplusplus
extern "C"
{
#endif

    //
    // Summary:
    //     ��ʼ�� CMS WebService ��������
    //
    // Returns:
    //     CMSWS_SUCCESS - �ɹ���CMSWS_FAILURE - ʧ�ܡ�
    //
    CMSWS_EXTERN int CMSWS_API CMSWS_InitServer(void);

    //
    // Summary:
    //     �ͷ� CMS WebService ��������Դ��
    //
    CMSWS_EXTERN void CMSWS_API CMSWS_UninitServer(void);

    //
    // Summary:
    //     �� CMS WebService ������ע�����
    //
    // Parameters:
    //   [in] uri:
    //     ���� URI �����·������ http://172.7.86.56:8080/HikCMS/services/ClientService �����·��Ϊ /HikCMS/services/ClientService ��
    //   [in] entry:
    //     ��������ڡ�
    //   [in] ns:
    //     CMS WebService envelope ns ��
    //
    // Returns:
    //     CMSWS_SUCCESS - �ɹ���CMSWS_FAILURE - ʧ�ܡ�
    //
    CMSWS_EXTERN int CMSWS_API CMSWS_RegisterService(const char* uri, CMSWS_ServiceServeRequest entry, cmsws_namespace* ns);

    //
    // Summary:
    //     ȡ�� CMS WebService ���������Ѿ�ע��ķ���
    //
    // Parameters:
    //   [in] uri:
    //     ���� URI �����·������ http://172.7.86.56:8080/HikCMS/services/ClientService �����·��Ϊ /HikCMS/services/ClientService ��
    //
    CMSWS_EXTERN void CMSWS_API CMSWS_UnregisterService(const char* uri);

    //
    // Summary:
    //     ���� CMS WebService ��������
    //
    // Parameters:
    //   [in] param:
    //     ����������
    //
    // Returns:
    //     CMSWS_SUCCESS - �ɹ���CMSWS_FAILURE - ʧ�ܡ�
    //
    CMSWS_EXTERN int CMSWS_API CMSWS_StartServer(ServerStartParam* param);

    //
    // Summary:
    //     ֹͣ CMS WebService ��������
    //
    CMSWS_EXTERN void CMSWS_API CMSWS_StopServer(void);

#ifdef __cplusplus
}
#endif
