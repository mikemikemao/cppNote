#pragma once
//
// Copyright (c) 2014 Hikvision Digital Technology Co., Ltd.
//
// Summary:
//     SIP WebService ��ض��塣
//
// Modification History:
//   2014-07-29:
//     V1.0 / ���� / ����
//
#include "cmsws_defines.h"

//
// Summary:
//     �趨�ṹ����� 4 �ֽڶ��롣
//
#pragma pack(4)

//
// Summary:
//     SIP Web Service ��� sipws_services_entry_t
//
typedef struct sipws_services_entry_t
{
    //
    // Summary:
    //     ����豸
    //Param:
    // szUserName �û���
    // 
    // szPassword ����
    // 
    // strXml Э������
    // 
    // Returns:
    //     1 - �ɹ���0 - ʧ�ܡ�
    //
    int (CMSWS_API *AddDevice)(const char* szUserName,const char* szPassword,const char* strXml);
    //
    // Summary:
    //     Զ������
    //Param:
    // szUserName �û���
    // 
    // szPassword ����
    // 
    // strXml Э������
    // 
    // Returns:
    //     1 - �ɹ���0 - ʧ�ܡ�
    //
    int (CMSWS_API *RemoteCfg)(const char* szUserName,const char* szPassword,const char* strXml);
    //
    // Summary:
    //     ����豸
    //Param:
    // szUserName �û���
    // 
    // szPassword ����
    // 
    // strXml Э������
    // 
    // Returns:
    //     1 - �ɹ���0 - ʧ�ܡ�
    //
    int (CMSWS_API *ClrDevice)(const char* szUserName,const char* szPassword,const char* strXml);
    //
    // Summary:
    //     Զ������
    //Param:
    // szUserName �û���
    // 
    // szOldpsw ������
    // 
    // szNewpsw ������
    // 
    // Returns:
    //     1 - �ɹ���0 - ʧ�ܡ�
    //
    int (CMSWS_API *ModifyPsw)(const char* szUserName,const char* szOldpsw,const char* szNewpsw);

    int (CMSWS_API *SyncTime)(time_t time);
   
    sipws_services_entry_t(void)
    {
        AddDevice = NULL;
        RemoteCfg = NULL;
        ClrDevice = NULL;
        ModifyPsw = NULL;
        SyncTime = NULL;
    }

} sipws_services_entry_t;


//
// Summary:
//     �ָ��ṹ��Ĭ�϶��뷽ʽ��
//
#pragma pack()
