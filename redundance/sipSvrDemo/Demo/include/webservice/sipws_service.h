#pragma once
//
// Copyright (c) 2014 Hikvision Digital Technology Co., Ltd.
//
// Summary:
//     SIP WebService Service �ӿڡ�
//
// Modification History:
//
#include "cmsws_defines.h"
#include "sipws_service_defines.h"

//
// Summary:
//     SIP WebService Service �ӿڡ�
//
#ifdef __cplusplus
extern "C"
{
#endif

    //
    // Summary:
    //     ��ʼ�� SIP WebService Service ��
    //
    // Parameters:
    //   [in] entry:
    //     sipws services ��ڡ�
    //
    CMSWS_EXTERN void CMSWS_API SIPWS_SetServiceEntry(sipws_services_entry_t* entry);

    //
    // Summary:
    //     ���� SIP WebService Service ��
    //
    // Parameters:
    //   [in] uri:
    //     ���� URI �����·������ http://172.7.86.56:8080/HikCMS/services/ClientService �����·��Ϊ /HikCMS/services/ClientService ��
    //
    // Returns:
    //     CMSWS_SUCCESS - �ɹ���CMSWS_FAILURE - ʧ�ܡ�
    //
    CMSWS_EXTERN int CMSWS_API SIPWS_StartService(const char* uri);

    //
    // Summary:
    //     ֹͣ SIP WebService Service ��
    //
    CMSWS_EXTERN void CMSWS_API SIPWS_StopService(void);

    //
    // Summary:
    //     �����ڴ棬���� SIP WebService �йܡ�
    //
    // Parameters:
    //   [in] size:
    //     ����ռ�ĳߴ硣
    //
    // Returns:
    //     �йܿռ�ĵ�ַ��
    //
    CMSWS_EXTERN void* CMSWS_API SIPWS_Malloc(int size);

#ifdef __cplusplus
}
#endif
