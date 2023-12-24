/*
 * =====================================================================================
 *
 *       Filename:  upgrade_ext.hpp
 *
 *    Description:  升级服务类
 *
 *        Version:  1.0
 *        Created:  2016-09-09 20:07:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yingc, yinguicai@hikvision.com
 *      Copyright:  Copyright (c) 2016, HIKVISION
 *
 * =====================================================================================
 */
#ifndef __UPGRADE_EXT_HPP__
#define __UPGRADE_EXT_HPP__

#include<hpr/HPR_Cond.h>
#include<hpr/HPR_FileSystem.h>
#include<hpr/HPR_Utils.h>
#include"definer.h"
#include"hash_map_def.h"
#include"device.h"
#include"read_write_lock.h"
#include"work_thread.h"
#include"net_manage_parser.h"
#include "sdk_client/sdk_common.hpp"
#include "sdk_client/sdk_command.h"
#include "sdk_client/sdk_client.hpp"

typedef struct{
	char        fileName[32];		/* 文件名 */
	UINT32      startOffset;		/* 起始位置 */
	UINT32      fileLen;			/* 文件长度 */
	UINT32      checkSum;			/* 校验和 */
}UPGRADE_FILE_HEADER;

/* file header of digicap.mav */
typedef struct
{   /* 64 bytes */
    UINT32      magic_number;       /* 0x484b5753 */
    UINT32      header_check_sum;   /* 文件头校验和 */
    UINT32      header_length;      /* 文件头长度 */
    UINT32      file_nums;          /* 文件个数 */
    UINT32      language;           /* 语言 */
    UINT32      device_class;       /* 1 – DS9000 DVR */
    UINT32      oemCode;            /* 1 – hikvision  */
    UINT8	    upgrade_version;	/* 升级版本: 0xFE - 需判断flash, ram, DSP ram 的容量*/
    UINT8       res_feature[15];    /* 保留字段 */
    UINT8       res[12];
    UINT32      soft_version;           /* 软件版本: V1.3.0 => 1<<24 + 3<<16 + 0<<0  */  
    UINT32      soft_builddate;         /* 构建日期: 160908 => 16<<16 + 9<<8 + 8<<0 */
    UPGRADE_FILE_HEADER  fileHeader[0];
}FIRMWARE_HEADER;

typedef struct {
	char            *package;
	int             package_size;
	unsigned int    version;
	unsigned int    builddate;
	int             new_version;
	HPR_MUTEX_T     mutex;
	HPR_COND_T      cond;
	int             start;
	int             quit;
}UpgradeExtInfo;

#define DIGICAP_NAME               "digicap.dav"
#define DIGICAP_DIR_PATH           "."
#define KEY_CONF_PATH              "./key.conf"              //为sipserver配置门口机的登录密码，默认用户名为admin
#define DIGICAP_SIZE_MAX           (0xf00000)
#define INTERVAL_FTP_CHECK_VERSION (1*60)                    //向ftp服务器获取最新版本轮询时间
#define INTERVAL_UPGRADE           (10*60)                    //本地程序轮询注册的门口机/室内机的时间
#define FTP_READ_BLOCK_SIZE        (256*1024)
#define UPGRADE_LOG_FILE_PATH      "./upgrade.log"
#define UPGRADE_LOG_SIZE_MAX       (256*1024)
#define UPGRADE_TIME_START         (0)
#define UPGRADE_TIME_END           (6)

class CUpgradeSrv
{
	friend HPR_VOIDPTR CALLBACK check_new_version(HPR_VOIDPTR arg);
	friend HPR_VOIDPTR CALLBACK handle_upgrade_ext(HPR_VOIDPTR arg);
public:

private:
	CUpgradeSrv ();
	virtual ~CUpgradeSrv ();
	string get_version(unsigned int version, unsigned int builddate);
	int is_need_upgrade(unsigned int new_v, unsigned int new_b, unsigned int old_v, unsigned int old_b);
	void GetRegInfo(map<string, string> &regs);
	int GetPasswd(string &passwd);

public:
	static CUpgradeSrv *GetInstance();
	HPR_INT32 Init();
	HPR_VOID UnInit();

private:

	HPR_INT32 m_start;
	UpgradeExtInfo m_extinfo;
	HPR_HANDLE m_thread_check;
	HPR_HANDLE m_thread_upgrade;
	static CUpgradeSrv* m_instance;
};

#endif /* __UPGRADE_EXT_HPP__ */
