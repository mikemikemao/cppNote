/*****************************************************
Copyright 2009-2010 Hikvision Digital Technology Co., Ltd.   
FileName: snmpAgent_define.h
Description: 结构和宏定义    
Author: guanguochen       
Date: 2009.09.14    
*****************************************************/
#if defined(WIN32)
#pragma once
#endif

#ifndef __SNMPAGENT_DEFINE_H__
#define __SNMPAGENT_DEFINE_H__

#if defined(WIN32)
    #if defined(SNMPAGENT_EXPORTS)
        #define SNMPAGENT_DECLARE extern "C" __declspec(dllexport)
    #else
        #define SNMPAGENT_DECLARE extern "C" __declspec(dllimport)
    #endif

    #ifndef CALLBACK
    #define CALLBACK __stdcall
    #endif
#else
    #define SNMPAGENT_DECLARE extern "C"

    #ifndef CALLBACK
        #define CALLBACK
    #else
        #undef CALLBACK
        #define CALLBACK
    #endif
#endif

enum SnmpAgent_IO_TYPE          //IO操作类型
{
    SNMPAGENT_SEND,         
    SNMPAGENT_RECV
};

typedef struct SnmpAgent_AsyncIO_Info
{
    SnmpAgent_IO_TYPE iotype;
    void *callclass;
    void *pbuffer;
} SNMPAGENT_ASYNCIO_INFO, *PSNMPAGENT_ASYNCIO_INFO;

#include "smival.h"

typedef struct PduInfo
{
    unsigned long requestid;
    int errorstatus;
    int errorindex;
    int vbcount;
    char **stroidarray;
    unsigned short type;
    SmiVALUE *objvaluearray;
} PDUINFO, *PPDUINFO;

typedef void (CALLBACK *SnmpAgent_Msg_CB)(PPDUINFO ppdu, char *srcaddr, unsigned short srcport, void *userdata);

#endif
