/*****************************************************
Copyright 2009-2010 Hikvision Digital Technology Co., Ltd.   
FileName: NetManagerAgent_define.h
Description: �ṹ�ͺ궨��    
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

class IStreamServer_Collector                   //��ý�������
{
public:
	virtual int GetWorkState() = 0;             //�����Ƿ�����
	virtual int GetSessionCount(void) = 0;      //��ȡ�ͻ���������
	virtual int GetChannelCount(void) = 0;      //��ȡ����ͨ����

	virtual ~IStreamServer_Collector(){};
};

class IMagServer_Collector                     //�ֻ����������
{
public:
	virtual int GetWorkState() = 0;             //�����Ƿ�����
	virtual int GetSessionCount(void) = 0;      //��ȡ�ͻ���������
	virtual int GetChannelCount(void) = 0;      //��ȡ����ͨ����

	virtual ~IMagServer_Collector(){};
};

class IKpsServer_Collector                    //���̷�����
{
public:
    virtual int GetWorkState() = 0;             //�����Ƿ�����

    virtual ~IKpsServer_Collector(){};
};

class IVRM_Collector                            //VRM������
{
public:
    virtual int GetWorkState() = 0;             //�����Ƿ�����
    virtual int GetCameraCount() = 0;           //��ȡͨ������
    virtual int GetVodCount() = 0;              //��ȡ��ǰ�㲥����
    virtual int GetStoreSpaceCount() = 0;       //��ȡVRM�洢�ռ��С����
    virtual int GetStoreFreeSpace() = 0;        //��ȡVRM�洢�ռ�ʣ���С
    virtual const char* GetRecordList() = 0;    //��ȡ��ǰ¼��ļ�ص�״̬

    virtual ~IVRM_Collector(){};
};

class IPCNVR_Collector                          //PCNVR������
{
public:
    virtual int GetWorkState() = 0;             //�����Ƿ�����
    virtual int GetRecCameraCount() = 0;        //��ȡ��ǰ¼��ͨ������
    virtual int GetVodCount() = 0;              //��ȡ��ǰ�㲥����
    virtual const char* GetRecordList() = 0;    //��ȡ��ǰ¼��ļ�ص�״̬

    virtual ~IPCNVR_Collector(){};
};

class IPCDVR_Collector                                      //PCDVR������
{
public:
    virtual int Get_PCDVR_AnalogChannel_Count() = 0;                //��ȡģ��ͨ����Ŀ
    virtual int Get_PCDVR_IPChannel_Count() = 0;                    //��ȡIPͨ����Ŀ
    virtual int Get_PCDVR_Disk_Count() = 0;                         //��ȡӲ����Ŀ
    virtual int Get_PCDVR_Channel_IsOFF(int channel) = 0;           //��ȡͨ������״̬
    virtual int Get_PCDVR_Channel_Record_State(int channel) = 0;    //��ȡͨ��¼��״̬
    virtual int Get_PCDVR_Channel_Connected_Count(int channel) = 0; //��ȡͨ��������

    virtual ~IPCDVR_Collector(){};
};

class IAlarmServer_Collector                    //����������
{
public:
    virtual int GetWorkState() = 0;             //�����Ƿ�����
    virtual int GetPoolThreadCount() = 0;       //��ǰ�������̳߳���Ŀ
    virtual int GetCurAlarmCount() = 0;         //��ǰ���յı�����

    virtual ~IAlarmServer_Collector(){};
};

class IDecodeServer_Collector                   //���������
{
public:
    virtual int GetWorkState() = 0;             //�����Ƿ�����
    virtual int GetRealPlayCount() = 0;         //��ȡ��ǰʵʱ����·��
    virtual int GetPlayBackCount() = 0;         //��ȡ��ǰ�ط�·��

    virtual ~IDecodeServer_Collector(){};
};

class IVideoWall_Collector                      //����ǽ������
{
public:
    virtual int GetWorkState() = 0;             //�����Ƿ�����
    virtual int GetDeviceCount() = 0;           //��ȡ����������
    virtual int GetServerCount() = 0;           //��ȡ�������������

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

class IVAG_Collector                    //Vag������
{
public:
    virtual int GetWorkState() = 0;             //�����Ƿ�����

    virtual ~IVAG_Collector(){};
};

class ISIP_Collector                    //SIP������
{
public:
    virtual int GetWorkState() = 0;             //�����Ƿ�����

    virtual ~ISIP_Collector(){};
};

#define MAX_INDEX_LEN 256

#endif
