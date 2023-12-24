/*
 * =====================================================================================
 *
 *       Filename:  sdk_command.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2016-08-25 15:16:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yingc, yinguicai@hikvision.com
 *   Organization:  HIKVISION
 *
 * =====================================================================================
 */

#ifndef __SDK_COMMAND_H__
#define __SDK_COMMAND_H__

#include "sdk_types.h"

/* network SDK version
    day  : bit 0 - 5
    month: bit 6 - 9
    year : bit 10 - 15
    minor: bit 16 - 23
    major: bit 24 - 31
*/
#define MAKE_NETSDK_VERSION(_major, _minor, _year, _month, _day)    \
    (   \
    (((_major)&0xff)<<24) |     \
    (((_minor)&0xff)<<16) | \
    ((((_year)-2000)&0x3f)<<10) |   \
    (((_month)&0xf)<<6) |   \
    ((_day)&0x3f)   \
    )

#define GET_NETSDK_MAJOR(version)   (((version)>>24)&0xff)
#define GET_NETSDK_MINOR(version)   (((version)>>16)&0xff)
#define GET_NETSDK_YEAR(version)    ((((version)>>10)&0x3f)+2000)
#define GET_NETSDK_MONTH(version)   (((version)>>6)&0xf)
#define GET_NETSDK_DAY(version)     ((version)&0x3f)

#define GET_NETSDK_BUILDDATE(version)   ((version)&0xffff)
#define RELOGIN_FLAG                    0x80000000
#define NETSDK_VERSION1_0   MAKE_NETSDK_VERSION(1, 0, 2004, 10, 4)
#define NETSDK_VERSION1_2   MAKE_NETSDK_VERSION(1, 2, 2005, 3, 15)      /* �û���/������ܣ�Ӳ����Ϊ16 */
#define NETSDK_VERSION1_3   MAKE_NETSDK_VERSION(1, 3, 2005, 4, 1)       /* �ƶ����/��Ƶ��ʧ/�ڵ���������ʱ��� */
#define NETSDK_VERSION1_4   MAKE_NETSDK_VERSION(1, 4, 2005, 5, 30)      /* Ԥ�õ���Ŀ 16 -> 128 */
#define NETSDK_VERSION1_5   MAKE_NETSDK_VERSION(1, 5, 2005,12, 28)      /* �û���Ȩ��ϸ����ͨ��(���ػطš�Զ�̻طš�Զ��Ԥ��)*/
#define NETSDK_VERSION2_0   MAKE_NETSDK_VERSION(2, 0, 2006,4, 27)
#define NETSDK_VERSION2_1   MAKE_NETSDK_VERSION(2, 1, 2006,8, 14)       /* ����汾���ϵ�SDK��Ҫ������֤ */
#define NETSDK_VERSION3_0   MAKE_NETSDK_VERSION(3, 0, 2008,2, 28)       
#define NETSDK_VERSION3_1   MAKE_NETSDK_VERSION(3, 1, 2009,7, 30)       
#define NETSDK_VERSION4_0   MAKE_NETSDK_VERSION(4, 0, 2010,3, 1)
#define NETSDK_VERSION5_1   MAKE_NETSDK_VERSION(5, 1, 2015,5, 11)

#define CURRENT_NETSDK_VERSION  NETSDK_VERSION4_0


#define SERIALNO_LEN        48   /* ���кų��� */
#define NAME_LEN                32              /* �û������� */
#define PASSWD_LEN              16              /* ���볤�� */
#define NEW_NETSDK_INTERFACE    90

//login/logout 
#define NETCMD_LOGIN            0x010000    /* login */
#define NETCMD_RELOGIN          0x010010    /* verify username/password again */
#define NETCMD_LOGOUT           0x010100    /* logout */
#define NETCMD_USEREXCHANGE     0x010200    /* user keep alive */
#define NETCMD_GET_DEVICECFG_V40    0x1110c2     /* ��ȡ��չ�豸���� */
#define NETCMD_UPGRADE          0x030b00    /* upgrade software */
#define NETCMD_REBOOT           0x030900    /* reboot DVR */

#define NETCMD_VIS_PRIVATE_VERIFIED    0x12600a     //����Ȩ����֤

/* define status that return to client */
#define NETRET_QUALIFIED        1
#define NETRET_WRITEFLASHERROR  22
#define NETRET_UPGRADE_FAILED   23
#define NETRET_UPGRADING        24


enum {
	SUPPORT_EXTENSION = (1<<0),
	SUPPORT_BYTE4     = (1<<1),
	SUPPORT_CHECKSUM  = (1<<2),
	SUPPORT_RSA       = (1<<3),
};

typedef enum {
	SYS_UPG_PROC_STEP_GET = 0x00,
	SYS_UPG_PROC_READ_DATA = 0x01,
	SYS_UPG_PROC_UPGRADE = 0x02,
	SYS_UPG_PROC_BACKUP = 0x03,
	SYS_UPG_PROC_INVALID = 0xff,
}SYS_UPG_PROC_STEP;


//���ظ��ͻ���״̬��ͷ
typedef struct{
	UINT32  length;
	UINT32  checkSum;
	UINT32  retVal;
	UINT8   enable;  //ʹ��ĳЩ����,��λ��ʾ,0-������,1-����
	//    enable & 0X01  ��ʾ�豸�Ƿ����öԱ������ݼ���
	UINT8   res[3];
}NETRET_HEADER;

typedef struct {
	UINT32  length;
	UINT32  ret;
	UINT8   step;
	UINT8   percent_step;
	UINT8   res[2];
	UINT32  percent_total;
}UPGRADE_RET_HEADER;

typedef struct{
	UINT32  length;             /* total length */
	UINT8   ifVer;              /* version: 90 -- new interface/60 -- old interface */
	UINT8   ipVer;
	UINT8   res1[1];
	UINT8   enable;             /* ʹ��һЩ����, ��λ��ʾ��0-�����ã�1-���� */
	// enable & 0x01  ��ʾsdk�Ƿ����öԱ������ݼ���
	UINT32  checkSum;           /* checksum */
	UINT32  netCmd;             /* client request command */
	UINT32  clientIp;           /* clinet IP address */
	UINT32  userID;             /* user ID */
	UINT8   clientMac[6];
	UINT8   res[2];
}NETCMD_HEADER;

//login
typedef struct{
	UINT32  length;
	UINT8   ifVer;
	UINT8   ipVer;   /*0: IPv4; 1:IPv6*/
	UINT8   res1[2];
	UINT32  checkSum;
	UINT32  netCmd;
	UINT32  version;
	UINT8   res2[4];
	UINT32  clientIp;
	UINT8   clientMac[6];
	UINT8   reqEx;
	UINT8   res3;
}NET_LOGIN_REQ;

typedef struct{
	UINT32  length;
	UINT32  checkSum;
	UINT32  retVal;
	UINT32  devSdkVer;
	UINT32  userID;
	UINT8   serialno[SERIALNO_LEN];
	UINT8   devType;
	UINT8   channelNums;
	UINT8   firstChanNo;
	UINT8   alarmInNums;
	UINT8   alarmOutNums;
	UINT8   hdiskNums;
#define SUPPORT_PROTOCOLS_RTSP 0xF0
#define SUPPORT_PROTOCOLS_SDK 0x0F
	UINT8       supportProtocols;/*��֧�ֵ�Ԥ��Э�����ͣ���λ*/
	UINT8   audioChanNums;/*�����Խ�ͨ������*/
	UINT8      maxIpcChanNums;/*�豸֧�ֵ����IPC����*/
	UINT8   subProto;
	UINT8   zeroChanNum;        /* ��ͨ��������� */
	UINT8    supportAbility;    /* &0x1 ��ʾ�Ƿ�֧����������
								   &0x2��ʾ�Ƿ�֧��Զ�̱���
								   &0x4��ʾ�Ƿ�֧��������
								   &0x8��ʾ�Ƿ�֧��˫����
								   &0x10��ʾ�Ƿ�֧��Զ��SADP
								   &0x20��ʾ֧��Raid����
								   &0x40��ʾ�Ƿ�֧��IPSANĿ¼����
								   &0x80��ʾ֧��rtp over rtsp*/
	UINT8   supportAbility1;    /* &0x2��ʾ�Ƿ�֧������/ �ط���������  
								   &0x4��ʾ�Ƿ�֧�ֲ������ȼ�
								   &0x8��ʾ�����豸�Ƿ�֧�ֲ���ʱ�����չ
								   &0x10��ʾ�Ƿ�֧�ֶ������������33����
								   */
	UINT8   supportAbility2;    /* ������λ����Ϊ0��ʾ��֧�֣���0��ʾ֧��			
								   bySupport2 & 0x1, ��ʾ�������Ƿ�֧��ͨ��URLȡ������
								   bySupport2 & 0x2,  ��ʾ֧��FTPV40
								   */
	UINT16  devTypeEx;
	UINT8   supportAbility3;
	/*�������Ƿ�֧��rtspЭ��ȡ��,��λ��ʾ,0-��֧��,1-֧��*/
	/*byMultiStreamProto & 0x1, ����3
	  byMultiStreamProto & 0x2, ����4
	  byMultiStreamProto & 0x4, ����5 */
	UINT8   byMultiStreamProto;

	UINT8   startIpChan;      //��ʼ����ͨ����
	UINT8   startIpTalkChan;  //��ʼ���ֶԽ�ͨ���ţ�Ϊ�����ֿ�ģ��Խ�ͨ����
	UINT8   byIpChanHigh;  /*����ͨ����λ*/
	UINT8   supportAbility4;	
	/*˽��Э��ȡ��ʱ��������װ��ʽ
	  ���������豸֧�ֵ������������ͣ������������������������ȣ�
	  ��λ��ʾ��bit0-PS��bit1-TS��bit2-RTP */
	UINT8   byStreamPackType;
	UINT8   byLanguageType;// ֧����������,��λ��ʾ,ÿһλ0-��֧��,1-֧��  
	//  byLanguageType ����0 ��ʾ ���豸
	//  byLanguageType & 0x1��ʾ֧������
	//  byLanguageType & 0x2��ʾ֧��Ӣ��

	UINT8   res2[3];

	UINT8 bySupport6;       //�����ֶΣ���λ��ʾ 0-��֧�� 1-֧��
	//bySupport6 &0x20     ��ʾ֧�ֻ�ȡ���������Ϣ��չ                                
	//bySupport6 &0x8 ��ʾ�Ƿ�֧����չ���������������
	UINT8   ucPasswordLevel;   //0:��Ч����  1:Ĭ������  2:��������
	UINT8   res3[5];
	UINT8 bySupport7;       //����,��λ��ʾ,0-��֧��,1-֧��
	// bySupport7 & 0x1  ��ʾ�豸�Ƿ�֧�� INTER_VCA_RULECFG_V42 ��չ
	// bySupport7 & 0x2  ��ʾ�豸�Ƿ�֧�� IPC HVT ģʽ��չ
	//bySupport7 & 0x04  ��ʾ�豸�Ƿ�֧�ַ�����������������ʱ��
	//bySupport7 &0x8   ��ʾ������̨PTZλ��ʱ���Ƿ�֧�ִ�ͨ����
	// bySupport7 & 0x10  ��ʾ�豸�Ƿ�֧�� ˫ϵͳ��������
	//bySupport7 & 0x20  ��ʾ�豸�Ƿ�֧�� OSD�ַ�����V51
	//bySupport7 & 0x40  ��ʾ�豸�Ƿ�֧�� ���Ӹ���(�������)
	//bySupport7 & 0x80  ��ʾ�豸�Ƿ�֧�� ���ļ���

	UINT8   ucRetryLoginTime;       //ʣ��ɳ��Ե�½�Ĵ������û������������ʱ���˲�����Ч
	UINT32  dwSurplusLockTime;    //ʣ��ʱ�䣬��λ�룬�û�����ʱ���˲�����Ч
}NET_LOGIN_RET;

typedef struct {
	UINT32  length;         /* ���������ܳ��� */
	UINT32  checkSum;       /* ����У��� */
	UINT32  retVal;         /* ����״̬���ض��ı�ʶ��ʾ��9000�豸 */
	UINT32  devSdkVer;
} NET_LOGIN_CHALLENGE_HEADER;

typedef struct{
	UINT32  userID;
	UINT8   serialno[SERIALNO_LEN];
	UINT8   devType;
	UINT8   channelNums;
	UINT8   firstChanNo;
	UINT8   alarmInNums;
	UINT8   alarmOutNums;
	UINT8   hdiskNums;
	UINT8   res2[2];
}NET_LOGIN_RETDATA;


typedef struct{
	UINT32  length;
	UINT8   DVRName[NAME_LEN];
	UINT32  deviceID;
	UINT32  recycleRecord;
	UINT8   serialno[SERIALNO_LEN];
	UINT32  softwareVersion;
	UINT32  softwareBuildDate;
	UINT32  dspSoftwareVersion;
	UINT32  dspSoftwareBuildDate;
	UINT32  panelVersion;
	UINT32  hardwareVersion;
	UINT8   alarmInNums;
	UINT8   alarmOutNums;
	UINT8   rs232Nums;
	UINT8   rs485Nums;
	UINT8   netIfNums;
	UINT8   hdiskCtrlNums;
	UINT8   hdiskNums;
	UINT8   devType;
	UINT8   channelNums;
	UINT8   firstChanNo;
	UINT8   decodeChans;
	UINT8   vgaNums;
	UINT8   usbNums;
	UINT8   auxOutNum;
	UINT8   audioNum;
	UINT8   ipChanNum;
	UINT8   zeroChanNum;        /* ��ͨ��������� */
	UINT8   supportAbility;     /* ������λ����Ϊ0��ʾ��֧�֣�1��ʾ֧�֣�
								   supportAbility & 0x1, ��ʾ�Ƿ�֧����������
								   supportAbility & 0x2, ��ʾ�Ƿ�֧��һ������
								   supportAbility & 0x4, ��ʾ�Ƿ�֧��ѹ������������ȡ
								   supportAbility & 0x8, ��ʾ�Ƿ�֧��˫���� 
								   supportAbility & 0x10,��ʾ֧��Զ��SADP 
								   supportAbility & 0x20 ��ʾ֧��Raid����
								   supportAbility & 0x40 ��ʾ֧��IPSANĿ¼����
								   supportAbility & 0x80��  ��ʾ֧��rtp over rtsp*/

	UINT8   bESataFun;          /* eSATA��Ĭ����;��0-Ĭ��¼��1-Ĭ�ϱ��� */
	UINT8   bEnableIPCPnp;      /* 0-��֧�ּ��弴�ã�1-֧�ּ��弴�� */
	UINT8 cbEnalbeQuato;    /*�Ƿ�ʹ�ܴ������*/
	UINT8    supportAbility1;   /* ������λ����Ϊ0��ʾ��֧�֣�1��ʾ֧�֣�
								   supportAbility1 & 0x2, ֧�����ֻطź�����*/
	UINT16  devTypeEx;
	UINT8   devTypeName[24];
	UINT8   res2[16];            /* ���� */
}NETPARAM_DEVICE_CFG_V40;


//����Ȩ����֤����
typedef struct tagNET_DVR_OPERATION_AUTH
{
    UINT16  wLength;    //�ṹ����
    UINT8   byVersion;  //�ߵ�λ�ֱ����ߵͰ汾���������ݰ汾�ͳ��Ƚ�����չ����ͬ�İ汾�ĳ��Ƚ�������
    UINT8   byRes1;
    UINT8   byPassword[PASSWD_LEN]; //��֤����
    UINT8   byRes[128]; //����
}NET_OPERATION_AUTH,*LPNET_OPERATION_AUTH;

#ifndef NETRET_DS_KH8302_A
#define NETRET_DS_KH8302_A      (602)  //���ڻ�
#endif

#ifndef NETRET_DS_KD8101_2
#define NETRET_DS_KD8101_2      (603)  //�ſڻ�
#endif

#ifndef NETRET_DS_KM8301
#define NETRET_DS_KM8301        (604)  //�����
#endif

#ifndef NETRET_DS_KVXXXX_XX
#define NETRET_DS_KVXXXX_XX     (605)  //�����ſڻ�
#endif

#endif /* __SDK_COMMAND_H__ */

