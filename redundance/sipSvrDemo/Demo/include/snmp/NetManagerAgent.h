/*****************************************************
Copyright 2009-2010 Hikvision Digital Technology Co., Ltd.   
FileName: NetManagerAgent.h
Description: APIº¯Êý¶¨Òå     
Author: guanguochen       
Date: 2009.09.14
*****************************************************/
#if defined(WIN32)
#pragma once
#endif

#ifndef __NETMANAGERAGENT_H__
#define __NETMANAGERAGENT_H__

#include "NetManagerAgent_define.h"
#include "snmpAgent_define.h"

NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_InitLib(void);
NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_FiniLib(void);

NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_StartService(unsigned short agentport = DEFAULT_AGENT_PORT, unsigned short family_type = AF_INET);
NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_StopService(void);
NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_SetTrapInfo(char* ip, int port);
NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_SendTrap(int vbcount, char **oidarray, SmiLPVALUE valuearray);

NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_Set_StreamServer_collector(IStreamServer_Collector *collector);
NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_Set_MagServer_collector(IMagServer_Collector *collector);
NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_Set_KpsServer_Collector(IKpsServer_Collector *collector);
NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_Set_VRM_collector(IVRM_Collector *collector);
NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_Set_PCNVR_collector(IPCNVR_Collector *collector);
NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_Set_AlarmServer_collector(IAlarmServer_Collector *collector);
NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_Set_PCDVR_collector(IPCDVR_Collector *collector);
NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_Set_DecodeServer_collector(IDecodeServer_Collector *collector);
NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_Set_VideoWall_collector(IVideoWall_Collector *collector);
NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_Set_PAG_collector(IPAG_Collector *collector);
NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_Set_CAG_collector(ICAG_Collector *collector);
NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_Set_VTM_collector(IVTM_Collector *collector);
NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_Set_VAG_collector(IVAG_Collector *collector);
NETMANAGERAGENT_DECLARE int CALLBACK NetManagerAgent_Set_SIP_collector(ISIP_Collector *collector);
#endif
