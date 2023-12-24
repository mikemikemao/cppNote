/*
 * =====================================================================================
 *
 *       Filename:  upgrade_ext.cpp
 *
 *    Description:
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

#include "upgrade_ext.hpp"
#include "addr_book_mgr.h"
#include "db_manager.h"
#include<hpr/HPR_Netdb.h>

using namespace std;

HPR_VOIDPTR CALLBACK check_new_version(HPR_VOIDPTR arg);
HPR_VOIDPTR CALLBACK handle_upgrade_ext(HPR_VOIDPTR arg);

#define UPGRA_DEBUG

#ifdef UPGRA_DEBUG
#define UPGRA_INFO(...) \
	do {\
		printf("[UPGRADE INFO]:%-20s, %-10d:    ", __FUNCTION__, __LINE__); \
		printf(__VA_ARGS__); \
	}while(0)
#else
#define UPGRA_INFO(...) do{}while(0)
#endif

#define UPGRA_ERR(...) \
	do {\
		printf("[UPGRADE ERR]:%-20s, %-10d:    ", __FUNCTION__, __LINE__); \
		printf(__VA_ARGS__); \
	}while(0)


CUpgradeSrv* CUpgradeSrv::m_instance = NULL;

/**
 * @brief  construct
 *
 * @return
 */
CUpgradeSrv::CUpgradeSrv()
{
	m_start = 0;
	m_thread_check = HPR_INVALID_THREAD;
	m_thread_upgrade = HPR_INVALID_THREAD;
	memset(&m_extinfo, 0, sizeof(UpgradeExtInfo));
}

/**
 * @brief  deconstruct
 *
 * @return
 */
CUpgradeSrv::~CUpgradeSrv()
{

}

HPR_INT32 CUpgradeSrv::Init()
{
	HPR_INT32 ret = 0;

	if (!m_start) {
		m_extinfo.package = (char*)malloc(DIGICAP_SIZE_MAX);
		if (!m_extinfo.package) {
			ret = -1;
			UPGRA_ERR("malloc error!!!\n");
			goto err;
		}
		HPR_MutexCreate(&m_extinfo.mutex);
		HPR_CondCreate(&m_extinfo.cond);
		
		m_start = 1;

		m_thread_check = HPR_Thread_Create(check_new_version, this, 32*1024);
		m_thread_upgrade = HPR_Thread_Create(handle_upgrade_ext, this, 32*1024);

		return 0;

err:
		SAFE_FREE(m_extinfo.package);
		return -1;
	}

	return 0;
}

HPR_VOID  CUpgradeSrv::UnInit()
{
	/* 对于sipserver，为简单处理，直接重启，不作合理性处理 */
#if 0
	if (m_start) {
		m_start = 0;

		HPR_Thread_Wait(m_thread_check);
		HPR_Thread_Wait(m_thread_upgrade);
		m_thread_check = HPR_INVALID_THREAD;
		m_thread_upgrade = HPR_INVALID_THREAD;

		SAFE_FREE(m_extinfo.package);
		HPR_CondDestroy(&m_extinfo.cond);
		HPR_MutexDestroy(&m_extinfo.mutex);
	}
#endif
}

/**
 * @brief 获取客户端实例
 *
 * @return 非空if ok
 */
CUpgradeSrv* CUpgradeSrv::GetInstance()
{
	if (!m_instance) {
		m_instance = new CUpgradeSrv();
	}

	return m_instance;
}

/**
 * @brief 获取版本字符串
 *
 * @param  [in] version : 软件版本
 * @param  [in] builddate : 构建日期
 *
 * @return
 */
string CUpgradeSrv::get_version(unsigned int version, unsigned int builddate)
{
	int y, m, d;
	int major, minor, revision;
	char verstr[128] = {0};

	major = (version >> 24) & 0xff;
	minor = (version >> 16) & 0xff;
	revision = (version >> 0) & 0xff;
	y = (builddate >> 16) & 0xffff;
	m = (builddate >> 8) & 0xff;
	d = (builddate >> 0) & 0xff;
	HPR_Snprintf(verstr, sizeof(verstr), "V%d.%d.%d build %02d%02d%02d", major, minor, revision, y, m, d);
	UPGRA_INFO("version = %s\n", verstr);

	return verstr;
}

int CUpgradeSrv::is_need_upgrade(unsigned int new_v, unsigned int new_b, unsigned int old_v, unsigned int old_b)
{
	struct {
		int major;
		int minor;
		int revision;
		int y;
		int m;
		int d;
	}bases[] = {
		{
			1,
			0,
			0,
			15,
			1,
			14,
		},
		{
			1,
			0,
			1,
			15,
			3,
			4,
		},
		{
			1,
			0,
			2,
			15,
			3,
			28,
		},
		{
			1,
			1,
			0,
			15,
			6,
			4,
		},
		{
			1,
			2,
			1,
			15,
			9,
			18,
		},
		{
			1,
			3,
			0,
			16,
			1,
			14,
		},
		{
			1,
			4,
			0,
			16,
			8,
			31,
		},
		{
			1,
			4,
			10,
			16,
			11,
			29,
		},
		{
			1,
			4,
			11,
			16,
			12,
			26,
		},
		{
			1,
			4,
			20,
			17,
			3,
			4,
		},
		{
			1,
			4,
			22,
			17,
			4,
			26,
		},
		{
			1,
			4,
			23,
			17,
			6,
			20,
		},
		{
			1,
			4,
			24,
			17,
			8,
			31,
		}
	};
	int i;
	int is_need = 0;
	unsigned int builddate, soft_version;
	int count = sizeof(bases)/sizeof(bases[0]);

	for (i = 0; i < count; i++) {
		builddate = (bases[i].y<<16) + (bases[i].m<<8) +(bases[i].d<<0);
		soft_version = (bases[i].major<<24) + (bases[i].minor<<16) + (bases[i].revision<<0);
		/* is match */
		if ((soft_version == old_v) && (builddate == old_b)) {
			break;
		}
	}
	if (count == i) {
		return is_need;
	}

	if ((new_v > old_v) || (new_b > old_b)) {
		is_need = 1;
	}

	return is_need;
}


HPR_VOIDPTR CALLBACK check_new_version(HPR_VOIDPTR arg)
{
	int ret = -1;
	FIRMWARE_HEADER firm_enc = {0};
	FIRMWARE_HEADER firm_dec = {0};
	int header_size = sizeof(FIRMWARE_HEADER);
	char path[64];
	int interval;
	HPR_HANDLE fd = HPR_INVALID_FILE;
	HPR_HANDLE key_fd = HPR_INVALID_FILE;
	CUpgradeSrv *srv = (CUpgradeSrv*)arg;
	UpgradeExtInfo *extinfo = &srv->m_extinfo;
	
	HPR_USleep(20*1000*1000);
	HPR_Snprintf(path, sizeof(path), "%s/%s", DIGICAP_DIR_PATH, DIGICAP_NAME);
	for (;;) {
		HPR_UINT32 count = 0;

		{
			HPR_MutexLock(&extinfo->mutex);
			key_fd = HPR_OpenFile(KEY_CONF_PATH, HPR_READ, 0);
			if (HPR_INVALID_FILE == key_fd) {
				UPGRA_ERR("key.conf open error, sipserver upgrade not open yet\n");
				extinfo->start = 0;
			} else {
				HPR_CloseFile(key_fd);
				if (!extinfo->start) {
					extinfo->start = 1;
					HPR_CondSignal(&extinfo->cond);
				}
				UPGRA_INFO("sipserver upgrade opened\n");
			}
			HPR_MutexUnlock(&extinfo->mutex);
		}


		HPR_MutexLock(&extinfo->mutex);
		fd = HPR_OpenFile(path, HPR_READ, 0);
		if (HPR_INVALID_FILE == fd) {
			UPGRA_ERR("open file failed!!!\n");
			goto error;
		}

		HPR_ReadFile(fd, (char *)&firm_enc, header_size, &count);
		if (count != header_size) {
			UPGRA_ERR("read file header failed!!!\n");
			goto error;
		}

		if (HPR_OK != convertData((char *)&firm_enc, (char *)&firm_dec, header_size)) {
			UPGRA_ERR("convert firmHeader error!!!\n");
			goto error;
		}
		firm_dec.soft_version = ntohl(firm_dec.soft_version);
		firm_dec.soft_builddate = ntohl(firm_dec.soft_builddate);

		if (((0 == extinfo->version) && (0 == extinfo->builddate)) ||
				((firm_dec.soft_version > extinfo->version) || (firm_dec.soft_builddate > extinfo->builddate))) {

			memset(extinfo->package, 0, DIGICAP_SIZE_MAX);
			extinfo->package_size = 0;
			memcpy(extinfo->package, &firm_enc, header_size);

			{
				extinfo->package_size += header_size;
				if (HPR_OK != HPR_ReadFile(fd, extinfo->package+header_size, DIGICAP_SIZE_MAX-header_size, &count)) {
					extinfo->package_size = 0;
					UPGRA_ERR("read file header failed!!!\n");
					goto error;
				}

				extinfo->package_size += count;
				extinfo->new_version = 1;
				extinfo->version = firm_dec.soft_version;
				extinfo->builddate = firm_dec.soft_builddate;
				UPGRA_INFO("new version = %-10u, new builddate = %-10u\n", extinfo->version, extinfo->builddate);
				srv->get_version(extinfo->version, extinfo->builddate);
				HPR_CondSignal(&extinfo->cond);
				UPGRA_INFO(".....get package success, size = %10d.....\n", extinfo->package_size);
			}
		}

error:
		HPR_MutexUnlock(&extinfo->mutex);
		if (HPR_INVALID_FILE != fd) {
			HPR_CloseFile(fd);
		}
		/* can be rewrite if have enough time */
		interval = INTERVAL_FTP_CHECK_VERSION;
		while((interval--) && srv->m_start) {
			HPR_USleep(1*1000*1000);
		}
		if (!srv->m_start) {
			/* uninit */
			break;
		}
	}

quit:

	return NULL;
}

void CUpgradeSrv::GetRegInfo(map<string, string> &regs)
{
    string device_ip;
    read_write_lock_t::read_lock_t r_lock(&addr_book_mgr_t::instance()->_addrbook_rwlock);
    addr_book_mgr_t::addr_book_map::iterator it = addr_book_mgr_t::instance()->_addressbook_map.begin();

    while (it != addr_book_mgr_t::instance()->_addressbook_map.end()) {
        it->second->get_ip(device_ip);

        if ( !((0 == strcmp("127.0.0.1", device_ip.c_str())) ||
                    (0 == strcmp("0.0.0.0", device_ip.c_str())) ||
                    	(it->second->get_dev_status() < 16 /* <16 - FALSE, >16 - TURE */ ) )) {
            regs[device_ip] = it->second->get_phone_number();
        }
		++it;
    }

    return;
}

int CUpgradeSrv::GetPasswd(string &passwd)
{
	int ret = -1;
	HPR_HANDLE fd = HPR_INVALID_FILE;
	char cipher[64] = {0};
	char origin[64] = {0};
	HPR_UINT32 count = 0;

	fd = HPR_OpenFile(KEY_CONF_PATH, HPR_READ, 0);
	if (HPR_INVALID_FILE == fd) {
		UPGRA_ERR("open file failed!!!\n");
		goto err;
	}
	
	if (HPR_OK != HPR_ReadFile(fd, cipher, sizeof(cipher), &count)) {
		UPGRA_ERR("read file header failed!!!\n");
		goto err;
	}

	if (HPR_OK != convertData((char *)&cipher, (char *)&origin, count)) {
		UPGRA_ERR("convert cipher error!!!\n");
		goto err;
	}
	passwd = origin;
	ret = 0;

err:
	if (HPR_INVALID_FILE != fd) {
		HPR_CloseFile(fd);
	}	
	return ret;
}

/**
 * @brief 设备升级处理函数
 * @return
 */
HPR_VOIDPTR CALLBACK handle_upgrade_ext(HPR_VOIDPTR arg)
{
	int i = 0;
	int len = 0;
	CUpgradeSrv *srv = (CUpgradeSrv*)arg;
	UpgradeExtInfo *extinfo = &srv->m_extinfo;
	
	HPR_USleep(20*1000*1000);
	for (;;) {
		static int fsize = 0;
		struct stat fsta = {0};
		map<string, string> regs;
		HPR_HANDLE flog = HPR_INVALID_FILE;

		{
			HPR_TIME_T now;
			HPR_TIME_EXP_T exp_time;

			now = HPR_TimeNow();
			if(0 == HPR_ExpTimeFromTimeLocal(now, &exp_time)) {
				if (!((exp_time.tm_hour > UPGRADE_TIME_START) && (exp_time.tm_hour < UPGRADE_TIME_END))) {
					UPGRA_INFO("upgrade time not arrive\n");
					HPR_USleep(1*60*1000*1000);
					continue;
				}
			}
		}

		flog = HPR_OpenFile(UPGRADE_LOG_FILE_PATH, HPR_WRITE|HPR_CREATE|HPR_APPEND, 0);
		if (HPR_INVALID_FILE == flog) {
			UPGRA_ERR("open file failed!!!\n");
			goto quit;
		}

		/* has new version or timeout */
		HPR_MutexLock(&extinfo->mutex);
		if (!extinfo->new_version) {
			HPR_CondTimedWait(&extinfo->cond, &extinfo->mutex, INTERVAL_UPGRADE*1000);
		}
		extinfo->new_version = 0;

		HPR_MutexUnlock(&extinfo->mutex);

		srv->GetRegInfo(regs);
	    map<string, string>::iterator it = regs.begin();
		while (it != regs.end()) {
			unsigned int version = 0, builddate = 0;
			char local_ip[32] = {0};
			char local_mac[8] = {0};
			char log[256] = {0};
			int room, log_len;
			string v_old, v_new;
			string passwd;
			int mac_len = sizeof(local_mac);
			unsigned int type = 0;
			HPR_TIME_T now;
			HPR_TIME_EXP_T exp_time;
			int need_upgrade = 0;
			char ftime[32] = {0};
			HPR_UINT32 write_num = 0;
			const char *dev_ip = it->first.c_str();
			const char *call_num = it->second.c_str();

			HPR_MutexLock(&extinfo->mutex);
			while (!(extinfo->package && extinfo->package_size && extinfo->start)) {
				HPR_CondWait(&extinfo->cond, &extinfo->mutex);
			}
			/* 是否为门口机 */
			if (strncmp(call_num + strlen(call_num) - 5, "000", 3)) {
				goto release;
			}

			if (0 != srv->GetPasswd(passwd)) {
				goto release;
			}

			room = atoi(call_num + strlen(call_num) - 3);

			db_manager_t::instance()->get_local_ip_str(local_ip);
			if (HPR_OK != HPR_GetMacAddr(local_ip, (unsigned char*)local_mac, &mac_len)) {
				goto release;
			}
			printf("locap_ip = %s, mac = %s, len =%d\n", local_ip, local_mac, mac_len);
			
			now = HPR_TimeNow();
			if(0 == HPR_ExpTimeFromTimeLocal(now, &exp_time)) {
	            HPR_Snprintf(ftime, sizeof(ftime), "%04d-%02d-%02d %02d:%02d:%02d", exp_time.tm_year+1900,\
	                exp_time.tm_mon+1, exp_time.tm_mday,  exp_time.tm_hour, exp_time.tm_min, exp_time.tm_sec);
			}

			UPGRA_INFO("call_num = %s, ip = %s upgrade begin\n", call_num, dev_ip);
			CSdkClient::GetInstance()->SetLocal(local_ip, local_mac);
			CSdkClient::GetInstance()->AddDevice(dev_ip);
			CSdkClient::GetInstance()->SetDeviceLoginInfo((const char*)dev_ip, "admin", passwd.c_str());
			if (0 != CSdkClient::GetInstance()->LoginDevice(dev_ip)) {
				UPGRA_ERR("call_num = %s, ip = %s login failed\n", call_num, dev_ip);
				log_len = HPR_Snprintf(log, sizeof(log), "[%s][SDKC_UPGRADE][ERROR]: call_num[ %s ][%16s] %s\n", ftime, call_num, dev_ip, "login error, may be not online");
				HPR_WriteFile(flog, log , log_len, &write_num);

				goto release;
			}
			CSdkClient::GetInstance()->GetDeviceType(dev_ip, type);
			if (NETRET_DS_KD8101_2 != type) {
				UPGRA_ERR("ip =%20s, type = %-10u\n", dev_ip, type);
				goto release;
			}

			if (0 != CSdkClient::GetInstance()->GetDeviceVersion(dev_ip, version, builddate)) {
				UPGRA_ERR("call_num = %s, ip = %s get version failed\n", call_num, dev_ip);
				log_len = HPR_Snprintf(log, sizeof(log), "[%s][SDKC_UPGRADE][ERROR]: call_num[ %s ][%16s] %s\n", ftime, call_num, dev_ip, "get version error");
				HPR_WriteFile(flog, log , log_len, &write_num);

				goto release;
			}
			UPGRA_INFO("ip =%20s, version = %-10u, builddate = %-10u\n", dev_ip, version, builddate);

			/* new version, upgrade the device */
			need_upgrade = srv->is_need_upgrade(extinfo->version, extinfo->builddate, version, builddate);
			if (need_upgrade) {
				v_old = srv->get_version(version, builddate);
				v_new = srv->get_version(extinfo->version, extinfo->builddate);
				log_len = HPR_Snprintf(log, sizeof(log), "[%s][SDKC_UPGRADE][ INFO]: call_num[ %s ][%16s] old version = %s, new version = %s\n", \
						ftime, call_num, dev_ip, v_old.c_str(), v_new.c_str());
				HPR_WriteFile(flog, log , log_len, &write_num);

				CSdkClient::GetInstance()->SetUpgradeFile((const char*)extinfo->package, extinfo->package_size);
				if (0 != CSdkClient::GetInstance()->UpgradeDevice(dev_ip)) {
					UPGRA_ERR("upgrade failed, unknown reason, should sleep!!!\n");
					HPR_USleep(2*60*1000*1000);
				}
				CSdkClient::GetInstance()->RebootDevice(dev_ip);
				log_len = HPR_Snprintf(log, sizeof(log), "[%s][SDKC_UPGRADE][ INFO]: call_num[ %s ][%16s] %s\n", ftime, call_num, dev_ip, "upgrade success");
				HPR_WriteFile(flog, log , log_len, &write_num);
				UPGRA_INFO("call_num = %s, ip = %s success\n", call_num, dev_ip);
			} else {
					UPGRA_INFO("call_num = %s, ip = %s version same\n", call_num, dev_ip);
					log_len = HPR_Snprintf(log, sizeof(log), "[%s][SDKC_UPGRADE][ INFO]: call_num[ %s ][%16s] %s\n", ftime, call_num, dev_ip, "version is same");
					HPR_WriteFile(flog, log , log_len, &write_num);
			}

release:
			HPR_MutexUnlock(&extinfo->mutex);
			CSdkClient::GetInstance()->LoginOutDevice(dev_ip);
			CSdkClient::GetInstance()->DelDevice(dev_ip);
			UPGRA_INFO("call_num = %s, ip = %s upgrade end\n", call_num, dev_ip);

			++it;
		}

		if (HPR_INVALID_FILE != flog) {
			HPR_CloseFile(flog);
		}

		/* is necessary, it will be reupgrade if has a new version after upgrading */
		HPR_USleep(10*1000*1000);
	}

quit:

	return NULL;
}




