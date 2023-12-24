/*
 * =====================================================================================
 *
 *       Filename:  upgrade_ext.hpp
 *
 *    Description:  ����������
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
	char        fileName[32];		/* �ļ��� */
	UINT32      startOffset;		/* ��ʼλ�� */
	UINT32      fileLen;			/* �ļ����� */
	UINT32      checkSum;			/* У��� */
}UPGRADE_FILE_HEADER;

/* file header of digicap.mav */
typedef struct
{   /* 64 bytes */
    UINT32      magic_number;       /* 0x484b5753 */
    UINT32      header_check_sum;   /* �ļ�ͷУ��� */
    UINT32      header_length;      /* �ļ�ͷ���� */
    UINT32      file_nums;          /* �ļ����� */
    UINT32      language;           /* ���� */
    UINT32      device_class;       /* 1 �C DS9000 DVR */
    UINT32      oemCode;            /* 1 �C hikvision  */
    UINT8	    upgrade_version;	/* �����汾: 0xFE - ���ж�flash, ram, DSP ram ������*/
    UINT8       res_feature[15];    /* �����ֶ� */
    UINT8       res[12];
    UINT32      soft_version;           /* ����汾: V1.3.0 => 1<<24 + 3<<16 + 0<<0  */  
    UINT32      soft_builddate;         /* ��������: 160908 => 16<<16 + 9<<8 + 8<<0 */
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
#define KEY_CONF_PATH              "./key.conf"              //Ϊsipserver�����ſڻ��ĵ�¼���룬Ĭ���û���Ϊadmin
#define DIGICAP_SIZE_MAX           (0xf00000)
#define INTERVAL_FTP_CHECK_VERSION (1*60)                    //��ftp��������ȡ���°汾��ѯʱ��
#define INTERVAL_UPGRADE           (10*60)                    //���س�����ѯע����ſڻ�/���ڻ���ʱ��
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
