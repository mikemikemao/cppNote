/*****************************************************
Copyright 2009-2010 Hikvision Digital Technology Co., Ltd.   
FileName: NetManagerAgent_define.h
Description: 结构和宏定义    
Author: guanguochen       
Date: 2009.09.14    
*****************************************************/
#if defined(WIN32)
#pragma once
#endif

#ifndef __NETMANAGERAGENT_DEFINE_H__
#define __NETMANAGERAGENT_DEFINE_H__

#if defined(WIN32)
#if defined(NETMANAGERAGENT_EXPORTS)
#define NETMANAGERAGENT_DECLARE extern "C" __declspec(dllexport)
#else
#define NETMANAGERAGENT_DECLARE extern "C" __declspec(dllimport)
#endif

#ifndef CALLBACK
#define CALLBACK __stdcall
#endif
#else
#define NETMANAGERAGENT_DECLARE extern "C"

#ifndef CALLBACK
#define CALLBACK
#else
#undef CALLBACK
#define CALLBACK
#endif
#endif

class IStreamServer_Collector                   //流媒体服务器
{
public:
	virtual int GetWorkState() = 0;             //工作是否正常
	virtual int GetSessionCount(void) = 0;      //获取客户端连接数
	virtual int GetChannelCount(void) = 0;      //获取连接通道数

	virtual ~IStreamServer_Collector(){};
};

class IMagServer_Collector                     //手机接入服务器
{
public:
	virtual int GetWorkState() = 0;             //工作是否正常
	virtual int GetSessionCount(void) = 0;      //获取客户端连接数
	virtual int GetChannelCount(void) = 0;      //获取连接通道数

	virtual ~IMagServer_Collector(){};
};

class IKpsServer_Collector                    //键盘服务器
{
public:
    virtual int GetWorkState() = 0;             //工作是否正常

    virtual ~IKpsServer_Collector(){};
};

class IVRM_Collector                            //VRM服务器
{
public:
    virtual int GetWorkState() = 0;             //工作是否正常
    virtual int GetCameraCount() = 0;           //获取通道总数
    virtual int GetVodCount() = 0;              //获取当前点播总数
    virtual int GetStoreSpaceCount() = 0;       //获取VRM存储空间大小总数
    virtual int GetStoreFreeSpace() = 0;        //获取VRM存储空间剩余大小
    virtual const char* GetRecordList() = 0;    //获取当前录像的监控点状态

    virtual ~IVRM_Collector(){};
};

class IPCNVR_Collector                          //PCNVR服务器
{
public:
    virtual int GetWorkState() = 0;             //工作是否正常
    virtual int GetRecCameraCount() = 0;        //获取当前录象通道总数
    virtual int GetVodCount() = 0;              //获取当前点播总数
    virtual const char* GetRecordList() = 0;    //获取当前录像的监控点状态

    virtual ~IPCNVR_Collector(){};
};

class IPCDVR_Collector                                      //PCDVR服务器
{
public:
    virtual int Get_PCDVR_AnalogChannel_Count() = 0;                //获取模拟通道数目
    virtual int Get_PCDVR_IPChannel_Count() = 0;                    //获取IP通道数目
    virtual int Get_PCDVR_Disk_Count() = 0;                         //获取硬盘数目
    virtual int Get_PCDVR_Channel_IsOFF(int channel) = 0;           //获取通道断线状态
    virtual int Get_PCDVR_Channel_Record_State(int channel) = 0;    //获取通道录像状态
    virtual int Get_PCDVR_Channel_Connected_Count(int channel) = 0; //获取通道连接数

    virtual ~IPCDVR_Collector(){};
};

class IAlarmServer_Collector                    //报警服务器
{
public:
    virtual int GetWorkState() = 0;             //工作是否正常
    virtual int GetPoolThreadCount() = 0;       //当前服务器线程池数目
    virtual int GetCurAlarmCount() = 0;         //当前接收的报警数

    virtual ~IAlarmServer_Collector(){};
};

class IDecodeServer_Collector                   //解码服务器
{
public:
    virtual int GetWorkState() = 0;             //工作是否正常
    virtual int GetRealPlayCount() = 0;         //获取当前实时解码路数
    virtual int GetPlayBackCount() = 0;         //获取当前回放路数

    virtual ~IDecodeServer_Collector(){};
};

class IVideoWall_Collector                      //电视墙服务器
{
public:
    virtual int GetWorkState() = 0;             //工作是否正常
    virtual int GetDeviceCount() = 0;           //获取解码器个数
    virtual int GetServerCount() = 0;           //获取解码服务器个数

    virtual ~IVideoWall_Collector(){};
};

class IPAG_Collector
{
public:
    virtual int GetWorkState() = 0;
    virtual int GetPagCapability() = 0;
    virtual int GetPuOnlineCount() = 0;
    virtual int GetPuRegFailCount() = 0;
    virtual int GetConnectedCAGCount() = 0;
    virtual int GetConnectedVRMCount() = 0;
    virtual int GetConnectedVTMStatus() = 0;

    virtual int GetPuChannelNum(unsigned int deviceid) = 0;
    virtual int GetPuHardDiskNum(unsigned int deviceid) = 0;
    virtual int GetPuAlarmInNum(unsigned int deviceid) = 0;
    virtual int GetPuAlarmOutNum(unsigned int deviceid) = 0;
    virtual int GetPuAudioNum(unsigned int deviceid) = 0;

    virtual int GetRecordStatic(unsigned int deviceid, unsigned int chan) = 0;
    virtual int GetSignalStatic(unsigned int deviceid, unsigned int chan) = 0;
    virtual int GetHardwareStatic(unsigned int deviceid, unsigned int chan) = 0;
    virtual int GetBitRate(unsigned int deviceid, unsigned int chan) = 0;
    virtual int GetLinkNum(unsigned int deviceid, unsigned int chan) = 0;

    virtual int GetDiskVolume(unsigned int deviceid, unsigned int chan) = 0;
    virtual int GetDiskStatic(unsigned int deviceid, unsigned int chan) = 0;
    virtual int GetDiskFree(unsigned int deviceid, unsigned int chan) = 0;

    virtual int GetAlarmInState(unsigned int deviceid, unsigned int chan) = 0;
    virtual int GetAlarmOutState(unsigned int deviceid, unsigned int chan) = 0;
    virtual int GetAudioInState(unsigned int deviceid, unsigned int chan) = 0;

    virtual int GetLocalDisplay(unsigned int deviceid) = 0;

    virtual ~IPAG_Collector(){};
};


class ICAG_Collector
{
public:
    virtual int GetWorkState() = 0;
    virtual int GetCagCapability() = 0;
    virtual int GetCagMaxCapability() = 0;
    virtual int GetCuOnlineSessionCount() = 0;
    virtual int GetCuRegFailCount() = 0;
    virtual int GetConnectedPAGCount() = 0;
    virtual int GetConnectedVRMCount() = 0;

    virtual ~ICAG_Collector(){};
};


class IVTM_Collector
{
public:
    virtual int GetWorkState() = 0;
    virtual int GetPlayPuChannelCount() = 0;
    virtual int GetPlayCuSessionCount() = 0;
    virtual int GetConnectedVtduCount() = 0;
    virtual int GetConnectedPagCount() = 0;

    virtual ~IVTM_Collector(){};
};

class IVAG_Collector                    //Vag服务器
{
public:
    virtual int GetWorkState() = 0;             //工作是否正常

    virtual ~IVAG_Collector(){};
};

class ISIP_Collector                    //SIP服务器
{
public:
    virtual int GetWorkState() = 0;             //工作是否正常

    virtual ~ISIP_Collector(){};
};

#define MAX_INDEX_LEN 256

#endif
