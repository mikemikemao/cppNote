#pragma once
//
// Copyright (c) 2014 Hikvision Digital Technology Co., Ltd.
//
// Summary:
//     SIP WebService 相关定义。
//
// Modification History:
//   2014-07-29:
//     V1.0 / 王立 / 创建
//
#include "cmsws_defines.h"

//
// Summary:
//     设定结构体最大按 4 字节对齐。
//
#pragma pack(4)

//
// Summary:
//     SIP Web Service 入口 sipws_services_entry_t
//
typedef struct sipws_services_entry_t
{
    //
    // Summary:
    //     添加设备
    //Param:
    // szUserName 用户名
    // 
    // szPassword 密码
    // 
    // strXml 协议内容
    // 
    // Returns:
    //     1 - 成功，0 - 失败。
    //
    int (CMSWS_API *AddDevice)(const char* szUserName,const char* szPassword,const char* strXml);
    //
    // Summary:
    //     远程配置
    //Param:
    // szUserName 用户名
    // 
    // szPassword 密码
    // 
    // strXml 协议内容
    // 
    // Returns:
    //     1 - 成功，0 - 失败。
    //
    int (CMSWS_API *RemoteCfg)(const char* szUserName,const char* szPassword,const char* strXml);
    //
    // Summary:
    //     清空设备
    //Param:
    // szUserName 用户名
    // 
    // szPassword 密码
    // 
    // strXml 协议内容
    // 
    // Returns:
    //     1 - 成功，0 - 失败。
    //
    int (CMSWS_API *ClrDevice)(const char* szUserName,const char* szPassword,const char* strXml);
    //
    // Summary:
    //     远程配置
    //Param:
    // szUserName 用户名
    // 
    // szOldpsw 旧密码
    // 
    // szNewpsw 新密码
    // 
    // Returns:
    //     1 - 成功，0 - 失败。
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
//     恢复结构体默认对齐方式。
//
#pragma pack()
