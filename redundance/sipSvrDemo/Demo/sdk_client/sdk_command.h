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
#define NETSDK_VERSION1_2   MAKE_NETSDK_VERSION(1, 2, 2005, 3, 15)      /* 用户名/密码加密，硬盘数为16 */
#define NETSDK_VERSION1_3   MAKE_NETSDK_VERSION(1, 3, 2005, 4, 1)       /* 移动侦测/视频丢失/遮挡报警布防时间段 */
#define NETSDK_VERSION1_4   MAKE_NETSDK_VERSION(1, 4, 2005, 5, 30)      /* 预置点数目 16 -> 128 */
#define NETSDK_VERSION1_5   MAKE_NETSDK_VERSION(1, 5, 2005,12, 28)      /* 用户的权限细化到通道(本地回放、远程回放、远程预览)*/
#define NETSDK_VERSION2_0   MAKE_NETSDK_VERSION(2, 0, 2006,4, 27)
#define NETSDK_VERSION2_1   MAKE_NETSDK_VERSION(2, 1, 2006,8, 14)       /* 这个版本以上的SDK需要二次认证 */
#define NETSDK_VERSION3_0   MAKE_NETSDK_VERSION(3, 0, 2008,2, 28)       
#define NETSDK_VERSION3_1   MAKE_NETSDK_VERSION(3, 1, 2009,7, 30)       
#define NETSDK_VERSION4_0   MAKE_NETSDK_VERSION(4, 0, 2010,3, 1)
#define NETSDK_VERSION5_1   MAKE_NETSDK_VERSION(5, 1, 2015,5, 11)

#define CURRENT_NETSDK_VERSION  NETSDK_VERSION4_0


#define SERIALNO_LEN        48   /* 序列号长度 */
#define NAME_LEN                32              /* 用户名长度 */
#define PASSWD_LEN              16              /* 密码长度 */
#define NEW_NETSDK_INTERFACE    90

//login/logout 
#define NETCMD_LOGIN            0x010000    /* login */
#define NETCMD_RELOGIN          0x010010    /* verify username/password again */
#define NETCMD_LOGOUT           0x010100    /* logout */
#define NETCMD_USEREXCHANGE     0x010200    /* user keep alive */
#define NETCMD_GET_DEVICECFG_V40    0x1110c2     /* 获取扩展设备参数 */
#define NETCMD_UPGRADE          0x030b00    /* upgrade software */
#define NETCMD_REBOOT           0x030900    /* reboot DVR */

#define NETCMD_VIS_PRIVATE_VERIFIED    0x12600a     //操作权限验证

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


//返回给客户端状态的头
typedef struct{
	UINT32  length;
	UINT32  checkSum;
	UINT32  retVal;
	UINT8   enable;  //使能某些功能,按位表示,0-不启用,1-启用
	//    enable & 0X01  表示设备是否启用对报文内容加密
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
	UINT8   enable;             /* 使能一些功能, 按位表示，0-不启用，1-启用 */
	// enable & 0x01  表示sdk是否启用对报文内容加密
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
	UINT8       supportProtocols;/*加支持的预览协议类型，按位*/
	UINT8   audioChanNums;/*语音对讲通道个数*/
	UINT8      maxIpcChanNums;/*设备支持的最大IPC个数*/
	UINT8   subProto;
	UINT8   zeroChanNum;        /* 零通道编码个数 */
	UINT8    supportAbility;    /* &0x1 表示是否支持智能搜索
								   &0x2表示是否支持远程备份
								   &0x4表示是否支持能力集
								   &0x8表示是否支持双网卡
								   &0x10表示是否支持远程SADP
								   &0x20表示支持Raid功能
								   &0x40表示是否支持IPSAN目录查找
								   &0x80表示支持rtp over rtsp*/
	UINT8   supportAbility1;    /* &0x2表示是否支持下载/ 回放流量区分  
								   &0x4表示是否支持布防优先级
								   &0x8表示智能设备是否支持布防时间段扩展
								   &0x10表示是否支持多磁盘数（超过33个）
								   */
	UINT8   supportAbility2;    /* 能力，位与结果为0表示不支持，非0表示支持			
								   bySupport2 & 0x1, 表示解码器是否支持通过URL取流解码
								   bySupport2 & 0x2,  表示支持FTPV40
								   */
	UINT16  devTypeEx;
	UINT8   supportAbility3;
	/*多码流是否支持rtsp协议取流,按位表示,0-不支持,1-支持*/
	/*byMultiStreamProto & 0x1, 码流3
	  byMultiStreamProto & 0x2, 码流4
	  byMultiStreamProto & 0x4, 码流5 */
	UINT8   byMultiStreamProto;

	UINT8   startIpChan;      //起始数字通道号
	UINT8   startIpTalkChan;  //起始数字对讲通道号，为了区分开模拟对讲通道号
	UINT8   byIpChanHigh;  /*数字通道高位*/
	UINT8   supportAbility4;	
	/*私有协议取流时，码流封装格式
	  码流包括设备支持的所有码流类型（主码流，子码流，三码流等）
	  按位表示，bit0-PS，bit1-TS，bit2-RTP */
	UINT8   byStreamPackType;
	UINT8   byLanguageType;// 支持语种能力,按位表示,每一位0-不支持,1-支持  
	//  byLanguageType 等于0 表示 老设备
	//  byLanguageType & 0x1表示支持中文
	//  byLanguageType & 0x2表示支持英文

	UINT8   res2[3];

	UINT8 bySupport6;       //能力字段，按位表示 0-不支持 1-支持
	//bySupport6 &0x20     表示支持获取物理磁盘信息扩展                                
	//bySupport6 &0x8 表示是否支持扩展智能侦测配置命令
	UINT8   ucPasswordLevel;   //0:无效密码  1:默认密码  2:密码正常
	UINT8   res3[5];
	UINT8 bySupport7;       //能力,按位表示,0-不支持,1-支持
	// bySupport7 & 0x1  表示设备是否支持 INTER_VCA_RULECFG_V42 扩展
	// bySupport7 & 0x2  表示设备是否支持 IPC HVT 模式扩展
	//bySupport7 & 0x04  表示设备是否支持返回锁定次数和锁定时间
	//bySupport7 &0x8   表示设置云台PTZ位置时，是否支持带通道号
	// bySupport7 & 0x10  表示设备是否支持 双系统升级备份
	//bySupport7 & 0x20  表示设备是否支持 OSD字符叠加V51
	//bySupport7 & 0x40  表示设备是否支持 主从跟踪(从摄像机)
	//bySupport7 & 0x80  表示设备是否支持 报文加密

	UINT8   ucRetryLoginTime;       //剩余可尝试登陆的次数，用户名，密码错误时，此参数有效
	UINT32  dwSurplusLockTime;    //剩余时间，单位秒，用户锁定时，此参数有效
}NET_LOGIN_RET;

typedef struct {
	UINT32  length;         /* 返回数据总长度 */
	UINT32  checkSum;       /* 数据校验和 */
	UINT32  retVal;         /* 返回状态，特定的标识表示是9000设备 */
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
	UINT8   zeroChanNum;        /* 零通道编码个数 */
	UINT8   supportAbility;     /* 能力，位与结果为0表示不支持，1表示支持，
								   supportAbility & 0x1, 表示是否支持智能搜索
								   supportAbility & 0x2, 表示是否支持一键备份
								   supportAbility & 0x4, 表示是否支持压缩参数能力获取
								   supportAbility & 0x8, 表示是否支持双网卡 
								   supportAbility & 0x10,表示支持远程SADP 
								   supportAbility & 0x20 表示支持Raid功能
								   supportAbility & 0x40 表示支持IPSAN目录查找
								   supportAbility & 0x80，  表示支持rtp over rtsp*/

	UINT8   bESataFun;          /* eSATA的默认用途，0-默认录像，1-默认备份 */
	UINT8   bEnableIPCPnp;      /* 0-不支持即插即用，1-支持即插即用 */
	UINT8 cbEnalbeQuato;    /*是否使能磁盘配额*/
	UINT8    supportAbility1;   /* 能力，位与结果为0表示不支持，1表示支持，
								   supportAbility1 & 0x2, 支持区分回放和下载*/
	UINT16  devTypeEx;
	UINT8   devTypeName[24];
	UINT8   res2[16];            /* 保留 */
}NETPARAM_DEVICE_CFG_V40;


//操作权限验证参数
typedef struct tagNET_DVR_OPERATION_AUTH
{
    UINT16  wLength;    //结构长度
    UINT8   byVersion;  //高低位分别代表高低版本，后续根据版本和长度进行扩展，不同的版本的长度进行限制
    UINT8   byRes1;
    UINT8   byPassword[PASSWD_LEN]; //验证密码
    UINT8   byRes[128]; //保留
}NET_OPERATION_AUTH,*LPNET_OPERATION_AUTH;

#ifndef NETRET_DS_KH8302_A
#define NETRET_DS_KH8302_A      (602)  //室内机
#endif

#ifndef NETRET_DS_KD8101_2
#define NETRET_DS_KD8101_2      (603)  //门口机
#endif

#ifndef NETRET_DS_KM8301
#define NETRET_DS_KM8301        (604)  //管理机
#endif

#ifndef NETRET_DS_KVXXXX_XX
#define NETRET_DS_KVXXXX_XX     (605)  //别墅门口机
#endif

#endif /* __SDK_COMMAND_H__ */

