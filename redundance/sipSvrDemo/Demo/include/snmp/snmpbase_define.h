/*****************************************************
Copyright 2009-2010 Hikvision Digital Technology Co., Ltd.   
FileName: snmpbase_define.h
Description: 结构和宏定义  	 
Author: guanguochen       
Date: 2009.09.14    
*****************************************************/
#if defined(WIN32)
#pragma once
#endif

#ifndef __SNMPBASE_DEFINE_H__
#define __SNMPBASE_DEFINE_H__

#define ENTITY_TYPE_UNKNOWN 0x4000
#define ENTITY_TYPE_SERVER 0x6000
#define ENTITY_TYPE_NOSNMP_DEVICE 0x6300
#define ENTITY_TYPE_SNMP_DEVICE 0x6600

#define ENTITY_SUBTYPE_UNKNOWN 0x4001
#define ENTITY_SUBTYPE_CMS_SERVER 0x6001
#define ENTITY_SUBTYPE_ALARM_SERVER 0x6002
#define ENTITY_SUBTYPE_RECORD_SERVER 0x6003
#define ENTITY_SUBTYPE_STREAM_SERVER 0x6004
#define ENTITY_SUBTYPE_LB 0x6006
#define ENTITY_SUBTYPE_VRM_SERVER 0x6007
#define ENTITY_SUBTYPE_PCNVR_SERVER 0x6008
#define ENTITY_SUBTYPE_VRB_SERVER 0x6010
#define ENTITY_SUBTYPE_NMS_SERVER 0x6012
#define ENTITY_SUBTYPE_CAG_SERVER 0x6052
#define ENTITY_SUBTYPE_VTM_SERVER 0x6053
#define ENTITY_SUBTYPE_INTERAREA_SERVER 0x6054
#define ENTITY_SUBTYPE_MAG_SERVER 0x6055
#define ENTITY_SUBTYPE_VMS_SERVER 0x6056
#define ENTITY_SUBTYPE_DAG_SERVER 0X6057
#define ENTITY_SUBTYPE_KPS_SERVER 0X6058
#define ENTITY_SUBTYPE_VAG_SERVER 0x6059

#define ENTITY_SUBTYPE_HIK_DSHC 0x6301
#define ENTITY_SUBTYPE_HIK_CARD 0x6302
#define ENTITY_SUBTYPE_HIK_DSME 0x6303
#define ENTITY_SUBTYPE_DECODE_DEVICE 0x6304
#define ENTITY_SUBTYPE_EMBE_DEVICE  0x6307
#define ENTITY_SUBTYPE_CVR_DEVICE   0x6308
#define ENTITY_SUBTYPE_ACCESS_DEVICE   0x6309
#define ENTITY_SUBTYPE_ALARM_DEVICE   0x6310
#define ENTITY_SUBTYPE_EHOME_DEVICE     0x6311


#define ENTITY_SUBTYPE_BNRECORD_DEVICE 0x6601

#define DEFAULT_AGENT_PORT 161
#define DEFAULT_MANAGER_PORT 162
#define DEFAULT_SNMP_VERSION 2
#define DEFUALT_SNMP_RETRIES 1
#define DEFAULT_SNMP_TIMEOUT 100
#define DEFAULT_SNMP_COMMUNITY "public"

#define sNMP_PDU_GET_ASYNC       21
#define sNMP_PDU_GETNEXT_ASYNC   22
#define sNMP_PDU_SET_ASYNC       23
#define sNMP_PDU_GETBULK_ASYNC   24
#define sNMP_PDU_INFORM_ASYNC    25

#define MAX_SNMP_PACKET 4096
#define MAX_OID_LEN 256
#define MAX_OID_NODE_LEN 32
#define MAX_OID_NODE_NUMBER 16
#define MAX_CHILD_NUMBER 100

#define _IPX_ADDRESS
#define _MAC_ADDRESS
#define SNMP_BROADCAST

#ifdef _WIN32
#else
#include <sys/socket.h>
#endif

// #if !defined WIN32 && !(defined (CPU) && CPU == PPC603)
// #define SNMP_PP_IPv6
// #endif

#ifndef _MSC_VER
#ifndef __BCPLUSPLUS__
#define HAVE_INET_ATON
#endif
#endif

#ifdef SNMP_PP_IPv6
#define HAVE_INET_PTON
#ifdef __GNUC__
#define HAVE_GETHOSTBYNAME2
#endif
#endif

#ifdef WIN32
#include <windows.h>
#ifdef __BCPLUSPLUS__
typedef unsigned __int64 pp_uint64;
#else
typedef ULONGLONG pp_uint64;
#endif
#else // not WIN32
typedef unsigned long long pp_uint64;
#endif

// Make use of mutable keyword
//#define SNMP_PP_MUTABLE mutable
#define SNMP_PP_MUTABLE

#define SAFE_INT_CAST(expr)  ((int)(expr))
#define SAFE_UINT_CAST(expr) ((unsigned int)(expr))

// Safe until 32 bit second counter wraps to zero (time functions)
#define SAFE_LONG_CAST(expr)  ((long)(expr))
#define SAFE_ULONG_CAST(expr) ((unsigned long)(expr))

enum snmp_version
{
	version1,         ///< (0) SNMPv1 
	version2c         ///< (1) SNMPv2c
};

#define iso								"1"
#define identified_organization			"1.3"
#define dod								"1.3.6"
#define internet						"1.3.6.1"
#define mgmt							"1.3.6.1.2"
#define mib2							"1.3.6.1.2.1"
#define mibprivate						"1.3.6.1.4"
#define enterprises						"1.3.6.1.4.1"

///////////////////////////////////////////////////////////////
#define hikCompany						"1.3.6.1.2.1.74"
#define hikEntityNumber					"1.3.6.1.2.1.74.1"
#define hikEntityTable					"1.3.6.1.2.1.74.2"
#define hikEntityEntry					"1.3.6.1.2.1.74.2.1"
///////////////////////////索引////////////////////////////////
#define hikIP							"1.3.6.1.2.1.74.2.1.1"
#define hikPort							"1.3.6.1.2.1.74.2.1.2"
#define hikEntityIndex					"1.3.6.1.2.1.74.2.1.3"
///////////////////////////固有属性////////////////////////////
#define hikEntityType					"1.3.6.1.2.1.74.2.1.100"
#define hikEntitySubType				"1.3.6.1.2.1.74.2.1.101"
#define hikOnline						"1.3.6.1.2.1.74.2.1.102"
#define hikService						"1.3.6.1.2.1.74.2.1.103"
#define hikCMSDefType					"1.3.6.1.2.1.74.2.1.104"
#define hikObjectID						"1.3.6.1.2.1.74.2.1.105"
#define hikObjectName					"1.3.6.1.2.1.74.2.1.106"
#define hikControlUnitID				"1.3.6.1.2.1.74.2.1.107"
////////////////////////////////////////////////////////////////

///////////////////////////海康公共状态////////////////////////
///////////////////////////CPU状态/////////////////////////////
#define hikCPUNum						"1.3.6.1.2.1.74.2.1.200"
#define hikCPUFrequency					"1.3.6.1.2.1.74.2.1.201"
#define hikCPUUsage						"1.3.6.1.2.1.74.2.1.202"
/////////////////////////8700 V1.1新增begin/////////////////////
///////////////////////////操作系统状态////////////////////////
#define hikOSInfo						"1.3.6.1.2.1.74.2.1.203"
///////////////////////cpu info//////////////////////////////////////
#define hikCPUInfo                      "1.3.6.1.2.1.74.2.1.204"
//////////////////////////memory info//////////////////////////////
#define hikMemoryInfo                   "1.3.6.1.2.1.74.2.1.205"
/////////////////////////服务器硬盘状态///////////////////////////
#define hikServerDiskInfo		         	    "1.3.6.1.2.1.74.2.1.206"
/////////////////////////服务器网卡状态///////////////////////////
#define hikNetAdapterInfo			    "1.3.6.1.2.1.74.2.1.207"
#define hikNetAdapterRunTimeInfo        "1.3.6.1.2.1.74.2.1.208"
///////////////////////////////进程状态/////////////////////////
#define hikProcessInfo			      	"1.3.6.1.2.1.74.2.1.209"
///////////////////dag registered entity info///////////////////////
#define hikDagRegEntityInfo             "1.3.6.1.2.1.74.2.1.210"

//////////////////////////////////异常类型和描述//////////////////////////////
#define hikExceptionDescription         "1.3.6.1.2.1.74.2.1.211"
#define hikExceptionAlarmType           "1.3.6.1.2.1.74.2.1.212"
////////////////////////////////////end////////////////////////////////


///////////////////////////内存状态/////////////////////////////
#define hikMemoryCapability				"1.3.6.1.2.1.74.2.1.220"
#define hikMemoryUsage					"1.3.6.1.2.1.74.2.1.221"
///////////////////////////////////////////////////////////////
///////////////////////////end/////////////////////////////////


///////////////////////////海康HC设备///////////////////////////
///////////////////////////设备状态/////////////////////////////
#define hikDeviceStatic					"1.3.6.1.2.1.74.2.1.230"
#define hikDeviceIndexCode              "1.3.6.1.2.1.74.2.1.231"
///////////////////////////硬盘状态/////////////////////////////
#define hikDiskNum						"1.3.6.1.2.1.74.2.1.240"
#define hikDiskTable					"1.3.6.1.2.1.74.2.1.241"
#define hikDiskEntry					"1.3.6.1.2.1.74.2.1.241.1"
#define hikDiskVolume					"1.3.6.1.2.1.74.2.1.241.1.1"
#define hikDiskFreeSpace				"1.3.6.1.2.1.74.2.1.241.1.2"
#define hikDiskStatic					"1.3.6.1.2.1.74.2.1.241.1.3"
///////////////////////////通道状态/////////////////////////////
#define hikChannelNum					"1.3.6.1.2.1.74.2.1.260"
#define hikIPChannelNum					"1.3.6.1.2.1.74.2.1.262"
#define hikChannelTable					"1.3.6.1.2.1.74.2.1.261"
#define hikChannelEntry					"1.3.6.1.2.1.74.2.1.261.1"
#define hikChannelRecordStatic			"1.3.6.1.2.1.74.2.1.261.1.1"
#define hikChannelSignalStatic			"1.3.6.1.2.1.74.2.1.261.1.2"
#define hikChannelHardwareStatic		"1.3.6.1.2.1.74.2.1.261.1.3"
#define hikChannelBitRate				"1.3.6.1.2.1.74.2.1.261.1.4"
#define hikChannelLinkNum				"1.3.6.1.2.1.74.2.1.261.1.5"
#define hikChannelLinkTable				"1.3.6.1.2.1.74.2.1.261.1.6"
#define hikChannelLinkEntry				"1.3.6.1.2.1.74.2.1.261.1.6.1"
#define hikChannelLinkIP				"1.3.6.1.2.1.74.2.1.261.1.6.1.1"
///////////////////////////报警输入端口状态/////////////////////////
#define hikAlarmInNum					"1.3.6.1.2.1.74.2.1.280"
#define hikAlarmInTable					"1.3.6.1.2.1.74.2.1.281"
#define hikAlarmInEntry					"1.3.6.1.2.1.74.2.1.281.1"
#define hikAlarmInStatic				"1.3.6.1.2.1.74.2.1.281.1.1"
///////////////////////////报警输出端口状态/////////////////////////
#define hikAlarmOutNum					"1.3.6.1.2.1.74.2.1.290"
#define hikAlarmOutTable				"1.3.6.1.2.1.74.2.1.291"
#define hikAlarmOutEntry				"1.3.6.1.2.1.74.2.1.291.1"
#define hikAlarmOutStatic				"1.3.6.1.2.1.74.2.1.291.1.1"
///////////////////////////本地显示状态/////////////////////////
#define hikLocalDisplay					"1.3.6.1.2.1.74.2.1.300"
///////////////////////////语音通道状态/////////////////////////
#define hikAudioChannelNum				"1.3.6.1.2.1.74.2.1.320"
#define hikAudioChannelTable			"1.3.6.1.2.1.74.2.1.321"
#define hikAudioChannelEntry			"1.3.6.1.2.1.74.2.1.321.1"
#define hikAudioInChanStatus			"1.3.6.1.2.1.74.2.1.321.1.1"
///////////////////////////////////////////////////////////////

///////////////////////////海康流媒体服务器状态/////////////////////
#define hikStreamWorkState				"1.3.6.1.2.1.74.2.1.400"	
#define hikStreamSessionCount			"1.3.6.1.2.1.74.2.1.401"
#define hikStreamChannelCount			"1.3.6.1.2.1.74.2.1.402"
////////////////////////////////////////////////////////////////////

///////////////////////////海康报警服务器状态/////////////////////////
#define hikAlarmWorkState				"1.3.6.1.2.1.74.2.1.500"
#define hikAlarmPoolThreadCount			"1.3.6.1.2.1.74.2.1.501"
#define hikAlarmCurAlarmCount			"1.3.6.1.2.1.74.2.1.502"
////////////////////////////////////////////////////////////////////

///////////////////////////海康VRM服务器状态/////////////////////////
#define hikVRMWorkState					"1.3.6.1.2.1.74.2.1.600"
#define hikVRMCameraCount				"1.3.6.1.2.1.74.2.1.601"
#define hikVRMVodCount					"1.3.6.1.2.1.74.2.1.602"
#define hikVRMStoreSpaceCount			"1.3.6.1.2.1.74.2.1.603"
#define hikVRMStoreFreeSpace			"1.3.6.1.2.1.74.2.1.604"
#define hikVRMRecordList			    "1.3.6.1.2.1.74.2.1.605"
#define hikVRMVersion                   "1.3.6.1.2.1.74.2.1.606"
////////////////////////////////////////////////////////////////////

///////////////////////////海康PCNVR服务器状态/////////////////////////
#define hikPCNVRWorkState				"1.3.6.1.2.1.74.2.1.700"
#define hikPCNVRRecCameraCount			"1.3.6.1.2.1.74.2.1.701"
#define hikPCNVRVodCount				"1.3.6.1.2.1.74.2.1.702"
#define hikPCNVRRecordList				"1.3.6.1.2.1.74.2.1.703"
#define hikPCNVRVersion				    "1.3.6.1.2.1.74.2.1.704"
////////////////////////////////////////////////////////////////////

///////////////////////////海康多路解码器状态/////////////////////////
#define hikDecodeDeviceWorkState		"1.3.6.1.2.1.74.2.1.800"
////////////////////////////////////////////////////////////////////


///////////////////////////海康网管服务器状态/////////////////////////
#define hikNetManagerServerWorkState		"1.3.6.1.2.1.74.2.1.900"
#define hikNetManagerServerEntityCount		"1.3.6.1.2.1.74.2.1.901"
#define hikNetManagerServerClientInfo		"1.3.6.1.2.1.74.2.1.902"
////////////////////////////////////////////////////////////////////


//////////////////////////海康解码服务器状态///////////////////////////
#define hikDecodeServerWorkState		"1.3.6.1.2.1.74.2.1.1000"
#define hikDecodeServerRealPlayCount	"1.3.6.1.2.1.74.2.1.1001"
#define hikDecodeServerPlayBackCount	"1.3.6.1.2.1.74.2.1.1002"
////////////////////////////////////////////////////////////////////

//////////////////////////海康电视墙服务器状态/////////////////////////
#define hikVideoWallWorkState			"1.3.6.1.2.1.74.2.1.1100"
#define hikVideoWallDeviceCount			"1.3.6.1.2.1.74.2.1.1101"
#define hikVideoWallServerCount			"1.3.6.1.2.1.74.2.1.1102"
#define hikVideoWallVersion			    "1.3.6.1.2.1.74.2.1.1103"
////////////////////////////////////////////////////////////////////


//////////////////////////邦诺设备/////////////////////////////////////////////////
#define hikbnRecordDeviceFCServiceStatus			"1.3.6.1.2.1.74.2.1.1200"
#define hikbnRecordDeviceISCSIServiceStatus			"1.3.6.1.2.1.74.2.1.1201"
#define hikbnRecordDeviceRaidStatus					"1.3.6.1.2.1.74.2.1.1202"
#define hikbnRecordDevicePvStatus					"1.3.6.1.2.1.74.2.1.1203"
#define hikbnRecordDeviceDiskArrayStatus			"1.3.6.1.2.1.74.2.1.1204"
#define hikbnRecordDeviceFlashFullStatus			"1.3.6.1.2.1.74.2.1.1205"
#define hikbnRecordDeviceDiskChannelStatus			"1.3.6.1.2.1.74.2.1.1206"
#define hikbnRecordDeviceDiskSmartAlarmStatus		"1.3.6.1.2.1.74.2.1.1207"
#define hikbnRecordDeviceFlashStatus				"1.3.6.1.2.1.74.2.1.1208"
#define hikbnRecordDeviceNASServiceStatus			"1.3.6.1.2.1.74.2.1.1209"
#define hikbnRecordDeviceCPUStatus					"1.3.6.1.2.1.74.2.1.1210"
#define hikbnRecordDeviceFanStatus					"1.3.6.1.2.1.74.2.1.1211"
#define hikbnRecordDeviceStoragePoolCapacity		"1.3.6.1.2.1.74.2.1.1212"
#define hikbnRecordDeviceStoragePoolFreeCapacity	"1.3.6.1.2.1.74.2.1.1213"
#define hikbnRecordDeviceISCSILUNCapacity			"1.3.6.1.2.1.74.2.1.1214"
#define hikbnRecordDeviceNASCapacity				"1.3.6.1.2.1.74.2.1.1215"
#define hikbnRecordDeviceNASSnapShotsCapacity		"1.3.6.1.2.1.74.2.1.1216"
#define hikbnRecordDeviceWORMCapacity				"1.3.6.1.2.1.74.2.1.1217"
#define hikbnRecordDeviceNASFreeCapacity			"1.3.6.1.2.1.74.2.1.1218"
#define hikbnRecordDevicePowerStatus                "1.3.6.1.2.1.74.2.1.1219"
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////海康MAG服务器状态/////////////////////
#define hikMagWorkState				"1.3.6.1.2.1.74.2.1.1300"	
#define hikMagSessionCount			"1.3.6.1.2.1.74.2.1.1301"
#define hikMagChannelCount			"1.3.6.1.2.1.74.2.1.1302"
////////////////////////////////////////////////////////////////////

/************************************************************************/
/*                          键盘服务器状态                                                                     */
/************************************************************************/
#define hikKeyboardProxyServerWrokState "1.3.6.1.2.1.74.2.1.1400"



///////////////////////// PAG 服务器状态 ///////////////////////
#define hikPAGWorkState				    "1.3.6.1.2.1.74.2.1.2000"
#define hikPAGCapabilityPUCount		    "1.3.6.1.2.1.74.2.1.2001"			//PAG接入设备能力
#define hikPAGOnlinePUCount			    "1.3.6.1.2.1.74.2.1.2002"			//在线PU个数
#define hikPAGRegisterPUFailureCount    "1.3.6.1.2.1.74.2.1.2003"			//PU注册失败次数
#define hikPAGConnectedCAGCount		    "1.3.6.1.2.1.74.2.1.2004"			//已连接的CAG个数
#define hikPAGConnectedVRMCount		    "1.3.6.1.2.1.74.2.1.2005"			//已连接的VRM个数
#define hikPAGConnectedVTMStatus	    "1.3.6.1.2.1.74.2.1.2006"			//是否与VTM连接成功
///////////////////////////////////////////////////////////////////////////////////

///////////////////////// CAG 服务器状态 ///////////////////////
#define hikCAGWorkState                 "1.3.6.1.2.1.74.2.1.2100"
#define hikCAGCapabiliyCUCount          "1.3.6.1.2.1.74.2.1.2101"   
#define hikCAGOnlineCUSessionCount      "1.3.6.1.2.1.74.2.1.2102"
#define hikCAGRegisterCUFailureCount    "1.3.6.1.2.1.74.2.1.2103"
#define hikCAGConnectedPAGCount         "1.3.6.1.2.1.74.2.1.2104"
#define hikCAGConnectedVRMCount         "1.3.6.1.2.1.74.2.1.2105"
#define hikCAGMaxCapabiliyCUCount       "1.3.6.1.2.1.74.2.1.2106"
#define hikCAGStartTime                 "1.3.6.1.2.1.74.2.1.2107"
#define hikCAGVersion                   "1.3.6.1.2.1.74.2.1.2108"
#define hikCAGStart                     "1.3.6.1.2.1.74.2.1.2109"
#define hikCAGStop                      "1.3.6.1.2.1.74.2.1.2110"
#define hikCAGRestart                   "1.3.6.1.2.1.74.2.1.2111"
#define hikCAGIndexCode                 "1.3.6.1.2.1.74.2.1.2112"
#define hikCAGRegisterCMSAddr           "1.3.6.1.2.1.74.2.1.2113"
#define hikCAGRegisterCMSPort           "1.3.6.1.2.1.74.2.1.2114"

///////////////////////// VTM 服务器状态 ///////////////////////
#define hikVTMWorkState                 "1.3.6.1.2.1.74.2.1.2200"
#define hikVTMPlayPUChannelCount        "1.3.6.1.2.1.74.2.1.2201"           //正在播放的PU通道数
#define hikVTMPlayCUSessionCount        "1.3.6.1.2.1.74.2.1.2202"
#define hikVTMConnectedVTDUCount        "1.3.6.1.2.1.74.2.1.2203"
#define hikVTMConnectedPAGCount         "1.3.6.1.2.1.74.2.1.2204"
#define hikVTMStartTime                 "1.3.6.1.2.1.74.2.1.2205"
#define hikVTMVersion                   "1.3.6.1.2.1.74.2.1.2206"
#define hikVTMStart                     "1.3.6.1.2.1.74.2.1.2207"
#define hikVTMStop                      "1.3.6.1.2.1.74.2.1.2208"
#define hikVTMRestart                   "1.3.6.1.2.1.74.2.1.2209"
#define hikVTMIndexCode                 "1.3.6.1.2.1.74.2.1.2210"
#define hikVTMRegisterCMSAddr           "1.3.6.1.2.1.74.2.1.2211"
#define hikVTMRegisterCMSPort           "1.3.6.1.2.1.74.2.1.2212"

///////////////////////// Trap状态 ///////////////////////		
#define hikTrapContent               	"1.3.6.1.2.1.74.2.1.2300"
#define hikTrapRaidState            	"1.3.6.1.2.1.74.2.1.2301"
#define hikTrapChannelState            	"1.3.6.1.2.1.74.2.1.2302"
#define hikTrapCPUAlarm             	"1.3.6.1.2.1.74.2.1.2303"
#define hikTrapFanStatus            	"1.3.6.1.2.1.74.2.1.2304"
#define hikTrapPowerStatus              "1.3.6.1.2.1.74.2.1.2305"
#define hikTrapLevel                    "1.3.6.1.2.1.74.2.1.2306"
#define hikServerType                   "1.3.6.1.2.1.74.2.1.2307"
#define hikServerIndexCode              "1.3.6.1.2.1.74.2.1.2308"
///<存储子系统新定义的Trap oid
///<公共部分
#define hiksnmpTrapOid  "1.3.6.1.6.3.1.1.4.1.0"
#define hikServiceName	"1.3.6.1.4.1.1.8092.6.1.2"///<服务的名称，目前定义为：vrm
#define hikServiceIndexCode	"1.3.6.1.4.1.1.8092.6.1.3"///<服务的索引编码
#define hikServiceIp	"1.3.6.1.4.1.1.8092.6.1.7"///<服务的IP地址
#define hikServiceErrCode	"1.3.6.1.4.1.1.8092.6.1.14"	///<Int	Trap对应的错误码，后面详述
#define hikServiceErrDesc	"1.3.6.1.4.1.1.8092.6.1.15"///<Trap的描述信息

///<VRM
#define hikVrmRecPlanUnmatchTrap	"1.3.6.1.4.1.1.8093.8.9"	///<设备录像计划与数据库录像计划不匹配

///<PCNVR
#define hikNvrRecTypeCHGTrap	"1.3.6.1.4.1.1.8093.9.12"	///<录像类型变化	20000
#define hikNvrModuleStartTrap	"1.3.6.1.4.1.1.8093.9.13"	///<功能模块启动异常	20001
#define hikNvrGetStreamTrap	"1.3.6.1.4.1.1.8093.9.14"	///<取流异常	20002
#define hikNvrTransFormatTrap	"1.3.6.1.4.1.1.8093.9.15"	///<转封装异常	20003
#define hikNvrFrameAnalyzeTrap	"1.3.6.1.4.1.1.8093.9.16"	///<帧分析异常	20004
#define hikNvrDiskIOTrap	"1.3.6.1.4.1.1.8093.9.17"	///<磁盘I/O异常	10001
#define hikNvrBufferFullTrap	"1.3.6.1.4.1.1.8093.9.18"	///<缓冲区满异常	10006

///////////////////////////////////////////////////////////////////////////////////

#define DEFAULT_SENDTIMEOUT 3000
#define DEFAULT_RECVTIMEOUT 3000

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER 
#endif

typedef enum { NOACCESS, READONLY, READWRITE, READCREATE } mib_access;

typedef struct stuEntityType
{
	unsigned int itype;
	char *strtype;
} STUENTITYTYPE, *PSTUENTITYTYPE;

typedef struct stuEntitySubType
{
	unsigned int isubtype;
	char *strsubtype;
} STUENTITYSUBTYPE, *PSTUENTITYSUBTYPE;

typedef struct stuBnOid
{
	char *hikbnoid;
	char *bnoid;
} STUBNOID, *PSTUBNOID;

typedef struct stuHikDes
{
	char *hikbnoid;
	char *desc;
} STUHIKDES, *PSTUHIKDES;

char *GetEntityTypeString(unsigned int type);
char *GetEntitySubTypeString(unsigned int subtype);
char *GetBnOidString(char *oid);
char *GetTrapdesc(char *oid);

#endif
