/*
 * =====================================================================================
 *
 *       Filename:  sdk_client.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2016-08-25 16:17:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yingc, yinguicai@hikvision.com
 *      Copyright:  Copyright (c) 2016, HIKVISION
 *
 * =====================================================================================
 */

#include<hpr/HPR_Socket.h>
#include<hpr/HPR_Thread.h>
#include<hpr/HPR_String.h>
#include<hpr/HPR_Utils.h>

#include "sdk_command.h"
#include "sdk_common.hpp"
#include "sdk_client.hpp"

#ifndef SOCK_SAFE_CLOSE
#define SOCK_SAFE_CLOSE(fd) do{if(fd != HPR_INVALID_SOCKET){HPR_ShutDown(fd, HPR_SHUT_RDWR);HPR_CloseSocket(fd);fd=HPR_INVALID_SOCKET;}}while(0)
#endif

/* 公钥指数 */
static const unsigned char RSA_PUB_KEY[] =
{
	0x01, 0x00, 0x01
};

/* 私钥指数 */
static const unsigned char RSA_PRI_KEY[] =
{
	0x68, 0x4D, 0x32, 0xAA, 0xE1, 0x3B, 0x28, 0xEA, 0x96, 0x48, 0x9A, 0x52, 0xCF, 0xD4, 0x11,
	0xBE, 0x8E, 0xC1, 0xC2, 0x36, 0xF2, 0x95, 0xB3, 0x66, 0x2E, 0x54, 0x49, 0xFD, 0xAE, 0xDC,
	0x1D, 0x8E, 0x86, 0xAA, 0xAD, 0x60, 0x5E, 0x82, 0xCD, 0x99, 0xA9, 0x96, 0x64, 0xB0, 0x70,
	0xA0, 0xC5, 0x3A, 0x78, 0x8B, 0x5F, 0x85, 0x7A, 0x31, 0x21, 0x95, 0xDD, 0xDC, 0x99, 0x0E,
	0x88, 0x4E, 0xA1, 0x3D, 0x8B, 0xF8, 0x58, 0xA1, 0x7C, 0xE8, 0x8C, 0x37, 0xE1, 0x1D, 0x59,
	0x76, 0x81, 0x48, 0xFC, 0xF0, 0x1C, 0x37, 0x5A, 0x39, 0x23, 0x05, 0xAB, 0xC1, 0x75, 0xC8,
	0x7F, 0x7A, 0xA6, 0xB9, 0x25, 0x9D, 0x36, 0xE7, 0x9E, 0xC5, 0xCE, 0x32, 0x45, 0x34, 0xE2,
	0xEC, 0xDF, 0xB1, 0xD1, 0x4D, 0xC9, 0x31, 0x55, 0xBA, 0x14, 0xB1, 0xD1, 0x09, 0x22, 0x69,
	0xCF, 0x09, 0xB9, 0xF6, 0xB6, 0x68, 0xA1, 0x49
};

/* 模数 */
static const unsigned char RSA_MOD[] =
{
	0xD7, 0x42, 0xCC, 0x97, 0x4D, 0x35, 0x1A, 0x8F, 0xB3, 0xAA, 0x42, 0xAA, 0x6D, 0x10, 0xEB,
	0x09, 0x58, 0xFA, 0xD2, 0xFB, 0x21, 0x0C, 0xDB, 0xBA, 0xB7, 0x22, 0x45, 0xE0, 0xF8, 0x1F,
	0x40, 0x26, 0xFD, 0x00, 0xAF, 0x83, 0x1B, 0x5C, 0xE5, 0x68, 0x7B, 0x3F, 0x81, 0x21, 0x9E,
	0xB4, 0x6B, 0x91, 0xCB, 0x5F, 0x2F, 0x6F, 0x18, 0xA6, 0x4B, 0xA0, 0x83, 0x33, 0x41, 0x7A,
	0x75, 0xE3, 0x4B, 0xF1, 0x23, 0xCC, 0xA5, 0x76, 0xD0, 0x58, 0x8F, 0x87, 0xE6, 0x4C, 0x66,
	0xB7, 0x83, 0x29, 0x16, 0xAE, 0x95, 0xE3, 0x76, 0x40, 0x0D, 0x54, 0xB8, 0x87, 0x0E, 0x8D,
	0x66, 0x0E, 0x0E, 0x1D, 0xC4, 0x16, 0xFD, 0x4F, 0xFA, 0xC4, 0xB9, 0x89, 0x5D, 0x01, 0x2D,
	0x86, 0x25, 0x44, 0x4B, 0x61, 0x31, 0xE2, 0xBD, 0x9A, 0xCD, 0x58, 0xE6, 0x6A, 0x94, 0xEC,
	0x94, 0x77, 0x64, 0x50, 0x8C, 0x04, 0xE8, 0x3F
};


CSdkClient* CSdkClient::m_instance = NULL;

/**
 * @brief  construct
 *
 * @return
 */
CSdkClient::CSdkClient()
{
	m_rsa = NULL;
	m_pub = NULL;
	m_upgrade = NULL;

	m_init_ok = 0;
	m_upgrade_size = 0;
	memset(m_local_ip, 0, sizeof(m_local_ip));

	Init();
}

/**
 * @brief  deconstruct
 *
 * @return
 */
CSdkClient::~CSdkClient()
{
	if (m_rsa) {
		RSA_free(m_rsa);
	}
	if (m_pub) {
		OPENSSL_free(m_pub);
	}
}

/**
 * @brief  初始化rsa等资源
 *
 * @return
 */
int CSdkClient::Init()
{
	int ret = -1;
	int pub_len = 0;

	m_rsa = RSA_new();
	if (m_rsa) {
		m_rsa->e = BN_bin2bn(RSA_PUB_KEY, ARRAY_LEN(RSA_PUB_KEY), m_rsa->e);
		m_rsa->d = BN_bin2bn(RSA_PRI_KEY, ARRAY_LEN(RSA_PRI_KEY), m_rsa->d);
		m_rsa->n = BN_bin2bn(RSA_MOD, ARRAY_LEN(RSA_MOD), m_rsa->n);

		pub_len = i2d_RSAPublicKey(m_rsa, &m_pub);
		if (RSA_PUB_BIN_LEN == pub_len) {
			m_init_ok = 1;
			ret = 0;
		}
	}

	return ret;
}

/**
 * @brief 获取客户端实例
 *
 * @return 非空if ok
 */
CSdkClient* CSdkClient::GetInstance()
{
	if (!m_instance) {
		m_instance = new CSdkClient();
	}

	return m_instance;
}

/**
 * @brief 设置本地客户端ip和mac地址
 *
 * @param  [in] local_ip
 * @param  [in] local_mac
 *
 * @return  0 if ok
 */
int CSdkClient::SetLocal(const char *local_ip, const char *local_mac)
{
	int ret = -1;

	if (local_ip && local_mac) {
		VisCore_Strlcpy(m_local_ip, local_ip, sizeof(m_local_ip));
		VisCore_Strlcpy(m_local_mac, local_mac, sizeof(m_local_mac));
		ret = 0;
	}

	return ret;
}

/**
 * @brief 客户端添加设备
 *
 * @param  [in] ip : 设备ip
 *
 * @return  0 if ok
 */
int CSdkClient::AddDevice(const char *ip)
{
	int ret = -1;

	if (ip) {
		map<string, CSdkDevice*>::iterator it = m_mdev.find(ip);
		if (it == m_mdev.end()) {
			CSdkDevice *dev = new CSdkDevice();

			if (dev) {
				dev->SetIp(ip);
				dev->SetPort(SDK_PORT);
				m_mdev[ip] = dev;
				ret = 0;
			}
		}
	}

	return ret;
}

/**
 * @brief  客户端删除设备
 *
 * @param  [in] ip : 设备ip
 *
 * @return  0 if ok
 */
int CSdkClient::DelDevice(const char *ip)
{
	int ret = -1;

	if (ip) {
		map<string, CSdkDevice*>::iterator it = m_mdev.find(ip);
		if (it != m_mdev.end()) {
			CSdkDevice *dev = it->second;
			delete dev;
			m_mdev.erase(it);

			return 0;
		}
	}

	return ret;
}

/**
 * @brief  设置设备ip的用户名和密码
 *
 * @param  [in] ip : 设备ip
 * @param  [in] username
 * @param  [in] passwd
 *
 * @return  0 if ok
 */
int CSdkClient::SetDeviceLoginInfo(const char *ip, const char *username, const char *passwd)
{
	int ret = -1;

	if (ip && username && passwd) {
		map<string, CSdkDevice*>::iterator it = m_mdev.find(ip);
		if (it != m_mdev.end()) {
			CSdkDevice *dev = it->second;

			dev->SetUsername(username);
			dev->SetPasswd(passwd);
			ret = 0;
		}
	}

	return ret;
}

/**
 * @brief 构造设备raw请求
 *
 * @param  [in] dev : 设备实例
 * @param  [out]buf : 请求buf
 * @param  [in] buf_size
 * @param  [in] cmd : sdk命令字
 *
 * @return  0 if ok
 */
int CSdkClient::MakeRawSdkReq(CSdkDevice *dev, char *buf, const int buf_size, const unsigned int cmd)
{
	int send_size = 0;
	HPR_UINT32 addr;
	NETCMD_HEADER *req = NULL;

	send_size = sizeof(NETCMD_HEADER);
	if (buf_size < send_size) {
		SDK_ERR("buf_size is too short\n");
		return -1;
	}
	if (HPR_InetPton(m_local_ip, &addr) != 0) {
		return -1;
	}

	req = (NETCMD_HEADER*)buf;
	req->length = htonl(send_size);
	req->ifVer = NEW_NETSDK_INTERFACE;
	req->ipVer = 0;
	req->netCmd = htonl(cmd);
	req->clientIp = htonl(addr);
	req->userID = htonl(dev->m_userid);
	memcpy(req->clientMac, m_local_mac, sizeof(req->clientMac));

	return 0;
}

/**
 * @brief 构造设备登录请求1
 * 注意: sdk登录分两部进行
 * @param  [out]buf : 请求buf
 * @param  [in] buf_size
 *
 * @return  0 if ok
 */
int CSdkClient::MakeLoginReqStep1(char *buf, const int buf_size)
{
	int send_size = 0;
	HPR_UINT32 addr;
	NET_LOGIN_REQ *req = NULL;

	send_size = sizeof(NET_LOGIN_REQ) + NAME_LEN + PASSWD_LEN + RSA_PUB_BIN_LEN;
	if (buf_size < send_size) {
		SDK_ERR("buf_size is too short\n");
		return -1;
	}
	if (HPR_InetPton(m_local_ip, &addr) != 0) {
		return -1;
	}

	req = (NET_LOGIN_REQ*)buf;
	req->length = htonl(send_size);
	req->ifVer = NEW_NETSDK_INTERFACE;
	req->ipVer = 0;
	req->netCmd = htonl(NETCMD_LOGIN);
	req->version = htonl(CURRENT_NETSDK_VERSION);
	req->clientIp = htonl(addr);
	req->reqEx = SUPPORT_EXTENSION | SUPPORT_BYTE4 | SUPPORT_RSA;
	memcpy(req->clientMac, m_local_mac, sizeof(req->clientMac));
	memcpy(buf + sizeof(NET_LOGIN_REQ) + NAME_LEN + PASSWD_LEN, (const void*)m_pub, RSA_PUB_BIN_LEN);

	return 0;
}

/**
 * @brief 构造设备登录请求2
 * 注意: sdk登录分两部进行
 * @param  [in] dev : 设备实例
 * @param  [out]buf : 请求buf
 * @param  [in] buf_size
 * @param  [in] key : 设备返回的挑战串
 * @param  [in] key_len : 挑战串长度
 *
 * @return  0 if ok
 */
int CSdkClient::MakeLoginReqStep2(CSdkDevice *dev, char *buf, const int buf_size, const char *key, const int key_len)
{
	int send_size = 0;
	HPR_UINT32 addr;
	NET_LOGIN_REQ *req = NULL;

	send_size = sizeof(NET_LOGIN_REQ) + NAME_LEN + PASSWD_LEN;
	if (buf_size < send_size) {
		SDK_ERR("buf_size is too short\n");
		return -1;
	}
	if (HPR_InetPton(m_local_ip, &addr) != 0) {
		return -1;
	}

	req = (NET_LOGIN_REQ*)buf;
	req->length = htonl(send_size);
	req->ifVer = NEW_NETSDK_INTERFACE;
	req->ipVer = 0;
	req->netCmd = htonl(NETCMD_LOGIN);
	req->version = htonl(CURRENT_NETSDK_VERSION);
	req->clientIp = htonl(addr);
	req->reqEx = SUPPORT_EXTENSION | SUPPORT_BYTE4 | SUPPORT_RSA;
	memcpy(req->clientMac, m_local_mac, sizeof(req->clientMac));

	sdk_hamc_encrypt((unsigned char*)dev->m_username, strlen(dev->m_username), (unsigned char*)key, key_len, (unsigned char*)(buf + sizeof(NET_LOGIN_REQ)));
	sdk_hamc_encrypt((unsigned char*)dev->m_passwd, strlen(dev->m_passwd), (unsigned char*)key, key_len, (unsigned char*)(buf + sizeof(NET_LOGIN_REQ) + NAME_LEN));

	return 0;
}


/**
 * @brief 构造操作权限验证请求
 * @param  [in] dev : 设备实例
 * @param  [out]buf : 请求buf
 * @param  [in] buf_size
 *
 * @return	0 if ok
 */
int CSdkClient::MakeVerifiedReq(CSdkDevice *dev, char *buf, const int buf_size)
{
	int send_size = 0;
	HPR_UINT32 addr;
	NETCMD_HEADER *req = NULL;
	NET_OPERATION_AUTH *auth = NULL;

	send_size = sizeof(NETCMD_HEADER)+sizeof(NET_OPERATION_AUTH);
	if (buf_size < send_size) {
		SDK_ERR("buf_size is too short\n");
		return -1;
	}
	if (HPR_InetPton(m_local_ip, &addr) != 0) {
		return -1;
	}

	req = (NETCMD_HEADER*)buf;
	req->length = htonl(send_size);
	req->ifVer = NEW_NETSDK_INTERFACE;
	req->ipVer = 0;
	req->netCmd = htonl(NETCMD_VIS_PRIVATE_VERIFIED);
	req->userID = htonl(dev->m_userid);
	req->clientIp = htonl(addr);
	memcpy(req->clientMac, m_local_mac, sizeof(req->clientMac));

	auth = (NET_OPERATION_AUTH *)(buf+sizeof(NETCMD_HEADER));
	auth->wLength = htons(sizeof(NET_OPERATION_AUTH));
	return 0;
}


/**
 * @brief  登录指定ip的设备
 *
 * @param  [in] ip
 *
 * @return  0 if ok
 */
int CSdkClient::LoginDevice(const char *ip)
{
	int ret = -1;
	char *buf = NULL;
	int buf_size = 0;
	HPR_SOCK_T sockfd = -1;
	HPR_ADDR_T hprAddr;
	NET_LOGIN_RET login_ret;
	char *chall_enc = NULL, *chall_dec = NULL;
	char recv_chall[sizeof(NET_LOGIN_CHALLENGE_HEADER) + ENCRYP_CHAN_LEN] = {0};
	int dec_len = 0;
	int len_step1 = 0, len_step2 = 0, len_verified = 0;

	if (!ip) {
		SDK_ERR("para err\n");
		return -1;
	}

	map<string, CSdkDevice*>::iterator it = m_mdev.find(ip);
	if (it != m_mdev.end()) {
		CSdkDevice *dev = it->second;
		if (dev->m_online) {
			/* already login */
			return 0;
		}
		sockfd = HPR_CreateSocket(AF_INET, SOCK_STREAM, 0);
		HPR_SetReuseAddr(sockfd, HPR_TRUE);
    	HPR_MakeAddrByString(AF_INET, dev->m_ip, dev->m_port, &hprAddr);
    	if (0 > (HPR_ConnectWithTimeOut(sockfd, &hprAddr, 1000))) {
			SDK_ERR("connect err\n");
			goto error;
    	}

		/* login step1 = req_header + name + pass + rsa_pub_key */
		buf_size = sizeof(NET_LOGIN_REQ) + NAME_LEN + PASSWD_LEN + RSA_PUB_BIN_LEN;
		buf = (char*)malloc(buf_size);
		if (!buf) {
			SDK_ERR("malloc err\n");
			goto error;
		}
		memset(buf, 0, buf_size);
		len_step1 = buf_size;
		if (0 != MakeLoginReqStep1(buf, buf_size)) {
			SDK_ERR("login req step1 is err\n");
			goto error;
		}

		if (len_step1 != HPR_Sendn(sockfd, buf, len_step1, 500)) {
			SDK_ERR("send login req step1 is err\n");
			goto error;
		}
		/*  read the challenge header + encryp_chall */
		ret = HPR_Recvn(sockfd, (void*)recv_chall, sizeof(recv_chall), 1000);
		if (ret != sizeof(recv_chall)) {
			SDK_ERR("read challenge header is err\n");
			goto error;
		}
		chall_enc = recv_chall + sizeof(NET_LOGIN_CHALLENGE_HEADER);

		/*  decode the challenge */
		chall_dec = (char*)malloc(1024);
		if (!chall_dec) {
			SDK_ERR("malloc err\n");
			goto error;
		}
		dec_len = RSA_private_decrypt(ENCRYP_CHAN_LEN, (unsigned char*)chall_enc, (unsigned char*)chall_dec, m_rsa, RSA_PKCS1_PADDING);
		if (dec_len <= 0) {
			SDK_ERR("decrypt err\n");
			goto error;
		}

		/* login step2 = req_header + name + pass */
		memset(buf, 0, buf_size);
		len_step2 = sizeof(NET_LOGIN_REQ) + NAME_LEN + PASSWD_LEN;
		if (0 != MakeLoginReqStep2(dev, buf, buf_size, chall_dec, dec_len)) {
			SDK_ERR("login req step2 is err\n");
			goto error;
		}
		if (len_step2 != HPR_Sendn(sockfd, buf, len_step2, 500)) {
			SDK_ERR("send login req step1 is err\n");
			goto error;
		}
		/*  read the login ret */
		memset(&login_ret, 0, sizeof(NET_LOGIN_RET));
		ret = HPR_Recvn(sockfd, &login_ret, sizeof(NET_LOGIN_RET), 1000);
		if (ret != sizeof(NET_LOGIN_RET)) {
			SDK_ERR("read challenge header is err\n");
			goto error;
		}
		if (login_ret.retVal != htonl(NETRET_QUALIFIED)) {
			SDK_ERR("login fail:ip = %s, ret =%d \n", ip, ntohl(login_ret.retVal));
			goto error;
		}
		else {
			dev->m_online = 1;
			dev->m_userid = ntohl(login_ret.userID);
			dev->m_type = ntohs(login_ret.devTypeEx);
			SDK_INFO("login success:ip = %s, userid = %u \n", ip, dev->m_userid);
		}

		SOCK_SAFE_CLOSE(sockfd);

		sockfd = HPR_CreateSocket(AF_INET, SOCK_STREAM, 0);
		HPR_SetReuseAddr(sockfd, HPR_TRUE);
    	HPR_MakeAddrByString(AF_INET, dev->m_ip, dev->m_port, &hprAddr);
    	if (0 > (HPR_ConnectWithTimeOut(sockfd, &hprAddr, 1000))) {
			SDK_ERR("connect err\n");
			goto error;
    	}

		memset(buf, 0, buf_size);
		len_verified = sizeof(NETCMD_HEADER)+sizeof(NET_OPERATION_AUTH);
		if (0 != MakeVerifiedReq(dev, buf, buf_size)) {
			SDK_ERR("make verified req is err\n");
			goto error;
		}
		
		if (len_verified != HPR_Sendn(sockfd, buf, len_verified, 500)) {
			SDK_ERR("send verified req is err\n");
			goto error;
		}
	}

	SAFE_FREE(buf);
	SAFE_FREE(chall_dec);
	SOCK_SAFE_CLOSE(sockfd);

	return 0;

error:
	SAFE_FREE(buf);
	SAFE_FREE(chall_dec);
	SOCK_SAFE_CLOSE(sockfd);

	return -1;
}

/**
 * @brief 获取指定设备ip的配置参数
 *
 * @param  [in] ip
 *
 * @return  0 if ok
 */
int CSdkClient::GetDeviceCfg(const char *ip)
{
	CSdkDevice *dev = NULL;

	if (!ip) {
		SDK_ERR("para err\n");
		return -1;
	}

	map<string, CSdkDevice*>::iterator it = m_mdev.find(ip);
	if (it != m_mdev.end()) {
		dev = it->second;
		return GetDeviceCfg(dev);
	}

	return -1;
}

/**
 * @brief 获取指定设备的配置参数
 *
 * @param  [in] dev
 *
 * @return  0 if ok
 */
int CSdkClient::GetDeviceCfg(CSdkDevice *dev)
{
	int ret = -1;
	char *buf = NULL;
	int buf_size = 0;
	HPR_SOCK_T sockfd = -1;
	HPR_ADDR_T hprAddr;
	NETCMD_HEADER req = {0};
	int req_size = sizeof(req);
	NETRET_HEADER *header = NULL;
	NETPARAM_DEVICE_CFG_V40 *cfg = NULL;

	if (!dev) {
		SDK_ERR("para err\n");
		return -1;
	}

	if (!dev->m_online) {
		/* not online yet */
		return -1;
	}

	sockfd = HPR_CreateSocket(AF_INET, SOCK_STREAM, 0);
	HPR_SetReuseAddr(sockfd, HPR_TRUE);
	HPR_MakeAddrByString(AF_INET, dev->m_ip, dev->m_port, &hprAddr);
	if (0 > (HPR_ConnectWithTimeOut(sockfd, &hprAddr, 1000))) {
		SDK_ERR("connect err\n");
		goto error;
	}

	buf_size = sizeof(NETRET_HEADER) + sizeof(NETPARAM_DEVICE_CFG_V40);
	buf = (char*)malloc(buf_size);
	if (!buf) {
		SDK_ERR("malloc err\n");
		goto error;
	}
	memset(buf, 0, buf_size);

	if (0 != MakeRawSdkReq(dev, (char*)&req, req_size, NETCMD_GET_DEVICECFG_V40)) {
		SDK_ERR("connect err\n");
		goto error;
	}

	if (req_size != HPR_Sendn(sockfd, &req, req_size, 500)) {
		SDK_ERR("send req is err\n");
		goto error;
	}

	/*  read the ret */
	ret = HPR_Recvn(sockfd, buf, buf_size, 1000);
	if (ret != buf_size) {
		SDK_ERR("read cfg ret is err\n");
		goto error;
	}
	header = (NETRET_HEADER*)buf;
	cfg = (NETPARAM_DEVICE_CFG_V40*)(buf + sizeof(NETRET_HEADER));

	if (header->retVal != htonl(NETRET_QUALIFIED)) {
		SDK_ERR("get cfg fail:ip = %s, ret =%d \n", dev->m_ip, ntohl(header->retVal));
		goto error;
	}
	else {
		dev->m_version = ntohl(cfg->softwareVersion);
		dev->m_builddate = ntohl(cfg->softwareBuildDate);
		SDK_INFO("get cfg success:version = %u, date = %u\n", dev->m_version, dev->m_builddate);
#if 1
		{
			int major, minor, revision;
			int y, m, d;
			char verstr[128] = {0};

			major = (dev->m_version >> 24) & 0xff;
			minor = (dev->m_version >> 16) & 0xff;
			revision = (dev->m_version >> 0) & 0xff;
			y = (dev->m_builddate >> 16) & 0xffff;
			m = (dev->m_builddate >> 8) & 0xff;
			d = (dev->m_builddate >> 0) & 0xff;
			HPR_Snprintf(verstr, sizeof(verstr), "V%d.%d.%d build %02d%02d%02d", major, minor, revision, y, m, d);
			SDK_INFO("version = %s\n", verstr);
		}
#endif
	}

	SAFE_FREE(buf);
	SOCK_SAFE_CLOSE(sockfd);

	return 0;

error:
	SAFE_FREE(buf);
	SOCK_SAFE_CLOSE(sockfd);

	return -1;
}

/**
 * @brief  设置升级文件
 *
 * @param  [in] buf : 升级文件缓存
 * @param  [in] size : 升级文件大小
 *
 * @return  0 if ok
 */
int CSdkClient::SetUpgradeFile(const char *buf, const int size)
{
	int ret = -1;

	if (buf && (size > 0)) {
		m_upgrade = (char*)buf;
		m_upgrade_size = size;

		ret = 0;
	}

	return ret;
}

/**
 * @brief 升级进度, 线程处理函数
 *
 * @param  [in] arg
 *
 * @return
 */
static HPR_VOIDPTR CALLBACK upgrade_progress(HPR_VOIDPTR arg)
{
	int ret;
	CSdkDevice *dev = (CSdkDevice*)arg;
	UPGRADE_RET_HEADER header = {0};
	UPGRADE_RET_HEADER pre_header = {0};
	int sockfd = dev->GetSockUpgrade();

	while (UPGRADE_STATUS_UPGRADING == dev->GetUpgradeStatus()) {
		/* read the ret */
		ret = HPR_Recvn(sockfd, (char*)&header, sizeof(UPGRADE_RET_HEADER), 2000);
		if (ret != sizeof(UPGRADE_RET_HEADER)) {
			SDK_ERR("...step = %2d, per_step = %10d, per_total = %10d...\n", pre_header.step, pre_header.percent_step, pre_header.percent_total);
			SDK_ERR("read upgrade ret is err, ret = %d\n", ret);
			goto error;
		}

		if (header.ret != htonl(NETRET_UPGRADING)) {
			SDK_ERR("read upgrade fail:ip = %s, ret =%d \n", dev->GetIp(), ntohl(header.ret));
			goto error;
		}
		else {
			header.percent_total = ntohl(header.percent_total);
			if ((SYS_UPG_PROC_UPGRADE == header.step) &&
				(100 == header.percent_step) &&
				(100 == header.percent_total)) {
				SDK_INFO("upgrade status success\n");
				dev->SetUpgradeStatus(UPGRADE_STATUS_SUCCESS);
				break;
			}
			memcpy(&pre_header, &header, sizeof(UPGRADE_RET_HEADER));
			//SDK_INFO("...step = %2d, per_step = %10d, per_total = %10d...\n", header.step, header.percent_step, header.percent_total);
		}
	}

	return NULL;

error:
	dev->SetUpgradeStatus(UPGRADE_STATUS_ERROR);
	return NULL;
}

/**
 * @brief  升级指定ip的设备
 *
 * @param  [in] ip
 *
 * @return  0 if ok
 */
int CSdkClient::UpgradeDevice(const char *ip)
{
	CSdkDevice *dev = NULL;

	if (!ip) {
		SDK_ERR("para err\n");
		return -1;
	}

	map<string, CSdkDevice*>::iterator it = m_mdev.find(ip);
	if (it != m_mdev.end()) {
		dev = it->second;
		return UpgradeDevice(dev);
	}

	return -1;
}

/**
 * @brief  升级指定的设备
 *
 * @param  [in] dev
 *
 * @return  0 if ok
 */
int CSdkClient::UpgradeDevice(CSdkDevice *dev)
{
	int ret = -1;
	unsigned int len = 0;
	HPR_SOCK_T sockfd = -1;
	HPR_ADDR_T hprAddr;
	NETCMD_HEADER req = {0};
	int req_size = sizeof(req);
	int left = 0, send_size = 0, send_sum = 0;
	NETRET_HEADER header_ret = {0};
	HPR_HANDLE th;

	if (!dev) {
		SDK_ERR("para err\n");
		return -1;
	}
	if (!(m_upgrade && m_upgrade_size)) {
		SDK_ERR("upgrade file err\n");
		return -1;
	}

	if (!dev->m_online) {
		/* not online yet */
		return -1;
	}

	dev->m_sock_upgrade = sockfd = HPR_CreateSocket(AF_INET, SOCK_STREAM, 0);
	HPR_SetReuseAddr(sockfd, HPR_TRUE);
	HPR_MakeAddrByString(AF_INET, dev->m_ip, dev->m_port, &hprAddr);
	if (0 > (HPR_ConnectWithTimeOut(sockfd, &hprAddr, 1000))) {
		SDK_ERR("connect err\n");
		goto error;
	}

	if (0 != MakeRawSdkReq(dev, (char*)&req, req_size, NETCMD_UPGRADE)) {
		SDK_ERR("raw cmd req err\n");
		goto error;
	}

	if (req_size != HPR_Sendn(sockfd, &req, req_size, 500)) {
		SDK_ERR("send req is err\n");
		goto error;
	}

	/* read the ret */
	ret = HPR_Recvn(sockfd, &header_ret, sizeof(header_ret), 1000);
	if (ret != sizeof(header_ret)) {
		SDK_ERR("read challenge header is err\n");
		goto error;
	}
	if (header_ret.retVal != htonl(NETRET_QUALIFIED)) {
		SDK_ERR("header ret fail:ip = %s, ret =%d \n", dev->m_ip, ntohl(header_ret.retVal));
		goto error;
	}
	dev->SetUpgradeStatus(UPGRADE_STATUS_UPGRADING);

	/* send the size=4+m_upgrade_size */
	len = htonl(4 + m_upgrade_size);
	if (sizeof(len) != HPR_Sendn(sockfd, (char*)&len, sizeof(len), 500)) {
		SDK_ERR("send req is err\n");
		goto error;
	}

    th = HPR_Thread_Create(upgrade_progress, dev, 32*1024);
    if (th == HPR_INVALID_THREAD) {
        SDK_ERR("HPR_Thread_Create fail.");
        goto error;
    }	

	/* send the real upgrade data */
	left = m_upgrade_size;
	while (left > 0) {
		send_size = left < SEND_BLOCK_SIZE ? left : SEND_BLOCK_SIZE;

		if (send_size != HPR_Sendn(sockfd, m_upgrade + send_sum, send_size, 1000)) {
			SDK_ERR("send req is err\n");
			dev->SetUpgradeStatus(UPGRADE_STATUS_ERROR);
			break;
		}
		send_sum += send_size;
		left -= send_size;

		if (UPGRADE_STATUS_ERROR == dev->GetUpgradeStatus()) {
			SDK_ERR("upgrade err\n");
			break;
		}
	}

	/* wait for the child thread run over */
	HPR_Thread_Wait(th);

	/*
	 * after upgrade the device, we will reboot the device
	 * then we should wait the device status change for NETRET_DVR_OPER_FAILED
	 */
	HPR_USleep(500000);

	if (UPGRADE_STATUS_SUCCESS != dev->GetUpgradeStatus()) {
		SDK_ERR("upgrade err\n");
		goto error;
	}

	SOCK_SAFE_CLOSE(sockfd);

	return 0;

error:
	SOCK_SAFE_CLOSE(sockfd);

	return -1;
}

/**
 * @brief 重启指定ip的设备
 *
 * @param  [in] ip
 *
 * @return  0 if ok
 */
int CSdkClient::RebootDevice(const char *ip)
{
	CSdkDevice *dev = NULL;

	if (!ip) {
		SDK_ERR("para err\n");
		return -1;
	}

	map<string, CSdkDevice*>::iterator it = m_mdev.find(ip);
	if (it != m_mdev.end()) {
		dev = it->second;
		return RebootDevice(dev);
	}

	return -1;
}

/**
 * @brief 重启指定的设备
 *
 * @param  [in] dev
 *
 * @return  0 if ok
 */
int CSdkClient::RebootDevice(CSdkDevice *dev)
{
	int ret = -1;
	HPR_SOCK_T sockfd = -1;
	HPR_ADDR_T hprAddr;
	NETCMD_HEADER req = {0};
	int req_size = sizeof(req);
	NETRET_HEADER header_ret = {0};

	if (!dev) {
		SDK_ERR("para err\n");
		return -1;
	}

	if (!dev->m_online) {
		/* not online yet */
		return -1;
	}

	sockfd = HPR_CreateSocket(AF_INET, SOCK_STREAM, 0);
	HPR_SetReuseAddr(sockfd, HPR_TRUE);
	HPR_MakeAddrByString(AF_INET, dev->m_ip, dev->m_port, &hprAddr);
	if (0 > (HPR_ConnectWithTimeOut(sockfd, &hprAddr, 1000))) {
		SDK_ERR("connect err\n");
		goto error;
	}

	/* reboot device */
	if (0 != MakeRawSdkReq(dev, (char*)&req, req_size, NETCMD_REBOOT)) {
		SDK_ERR("raw cmd req err\n");
		goto error;
	}
	if (req_size != HPR_Sendn(sockfd, &req, req_size, 500)) {
		SDK_ERR("send req is err\n");
		goto error;
	}

	/* read the ret */
	ret = HPR_Recvn(sockfd, &header_ret, sizeof(header_ret), 1000);
	if (ret != sizeof(header_ret)) {
		SDK_ERR("read ret header is err\n");
		goto error;
	}
	if (header_ret.retVal != htonl(NETRET_QUALIFIED)) {
		SDK_ERR("header ret fail:ip = %s, ret =%d \n", dev->m_ip, ntohl(header_ret.retVal));
		goto error;
	}
	SDK_INFO("reboot dev is ok, ip = %s\n", dev->m_ip);

	SOCK_SAFE_CLOSE(sockfd);

	return 0;

error:
	SOCK_SAFE_CLOSE(sockfd);

	return -1;
}

/**
 * @brief  登出指定ip的设备
 *
 * @param  [in] ip
 *
 * @return 0 if ok
 */
int CSdkClient::LoginOutDevice(const char *ip)
{
	CSdkDevice *dev = NULL;

	if (!ip) {
		SDK_ERR("para err\n");
		return -1;
	}

	map<string, CSdkDevice*>::iterator it = m_mdev.find(ip);
	if (it != m_mdev.end()) {
		dev = it->second;
		return LoginOutDevice(dev);
	}

	return -1;
}

/**
 * @brief  登出指定的设备
 *
 * @param  [in] dev
 *
 * @return 0 if ok
 */
int CSdkClient::LoginOutDevice(CSdkDevice *dev)
{
	int ret = -1;
	HPR_SOCK_T sockfd = -1;
	HPR_ADDR_T hprAddr;
	NETCMD_HEADER req = {0};
	int req_size = sizeof(req);
	NETRET_HEADER header_ret = {0};

	if (!dev) {
		SDK_ERR("para err\n");
		return -1;
	}

	if (!dev->m_online) {
		/* not online yet */
		return -1;
	}

	sockfd = HPR_CreateSocket(AF_INET, SOCK_STREAM, 0);
	HPR_SetReuseAddr(sockfd, HPR_TRUE);
	HPR_MakeAddrByString(AF_INET, dev->m_ip, dev->m_port, &hprAddr);
	if (0 > (HPR_ConnectWithTimeOut(sockfd, &hprAddr, 1000))) {
		SDK_ERR("connect err\n");
		goto error;
	}

	/* reboot device */
	if (0 != MakeRawSdkReq(dev, (char*)&req, req_size, NETCMD_LOGOUT)) {
		SDK_ERR("raw cmd req err\n");
		goto error;
	}

	if (req_size != HPR_Sendn(sockfd, &req, req_size, 500)) {
		SDK_ERR("send req is err\n");
		goto error;
	}

	/* read the ret */
	ret = HPR_Recvn(sockfd, &header_ret, sizeof(header_ret), 1000);
	if (ret != sizeof(header_ret)) {
		SDK_ERR("read header is err\n");
		goto error;
	}
	if (header_ret.retVal != htonl(NETRET_QUALIFIED)) {
		SDK_ERR("header ret fail:ip = %s, ret =%d \n", dev->m_ip, ntohl(header_ret.retVal));
		goto error;
	} else {
		dev->m_online = 0;
		dev->m_userid = 0;
		SDK_INFO("login out is ok, ip = %s\n", dev->m_ip);
	}

	SOCK_SAFE_CLOSE(sockfd);

	return 0;

error:
	SOCK_SAFE_CLOSE(sockfd);

	return -1;
}

/**
 * @brief 获取指定设备ip的软件版本和构建日期
 *
 * @param  [in] ip
 * @param  [out]version
 * @param  [out]builddate
 *
 * @return 0 if ok
 */
int CSdkClient::GetDeviceVersion(const char *ip, unsigned int &version, unsigned int &builddate)
{
	int ret = -1;
	CSdkDevice *dev = NULL;

	if (!ip) {
		SDK_ERR("para err\n");
		return -1;
	}

	map<string, CSdkDevice*>::iterator it = m_mdev.find(ip);
	if (it != m_mdev.end()) {
		dev = it->second;

		ret = GetDeviceCfg(dev);
		if (0 == ret) {
			version = dev->m_version;
			builddate = dev->m_builddate;
		}
		return ret;
	}

	return -1;
}


/**
 * @brief 获取指定设备ip的类型
 *
 * @param  [in] ip
 * @param  [out] type
 *
 * @return 0 if ok
 */
int CSdkClient::GetDeviceType(const char *ip, unsigned int &type)
{
	CSdkDevice *dev = NULL;

	if (!ip) {
		SDK_ERR("para err\n");
		return -1;
	}

	map<string, CSdkDevice*>::iterator it = m_mdev.find(ip);
	if (it != m_mdev.end()) {
		dev = it->second;

		if (dev->m_type) {
			type = dev->m_type;
			return 0;
		}
	}

	return -1;
}


