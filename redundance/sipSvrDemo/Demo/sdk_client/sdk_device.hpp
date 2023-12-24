/*
 * =====================================================================================
 *
 *       Filename:  sdk_device.hpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2016-08-26 11:21:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yingc, yinguicai@hikvision.com
 *   Organization:  HIKVISION
 *
 * =====================================================================================
 */

#ifndef __SDK_DEVICE_HPP__
#define __SDK_DEVICE_HPP__

#include <iostream>

#define NAME_LEN                32              /* 用户名长度 */
#define PASSWD_LEN              16              /* 密码长度 */

typedef enum {
	UPGRADE_STATUS_NONE       = 1,
	UPGRADE_STATUS_UPGRADING  = 2,
	UPGRADE_STATUS_ERROR      = 3,
	UPGRADE_STATUS_SUCCESS    = 4,
}UPGRADE_STATUS;


class CSdkClient;

class CSdkDevice {
	friend class CSdkClient;
public:
	CSdkDevice();
	virtual ~CSdkDevice();

public:
	void SetUsername(const char *username);
	void SetPasswd(const char *passwd);
	void SetIp(const char *ip);
	void SetPort(unsigned short port) { m_port = port; }
	void SetOnline(int online)  { m_online = online; };
	void SetUpgrading(int upgrade);
	void SetSockUpgrade(int sockfd) { m_sock_upgrade = sockfd; }
	void SetUpgradeStatus(UPGRADE_STATUS status) { m_upgrade_status = status; }

public:
	char*            GetUsername() { return m_username; }
	char*            GetPasswd() { return m_passwd; }
	char*            GetIp() { return m_ip; }
	unsigned short   GetPort() { return m_port; }
	int              IsOnline() { return m_online; }
	int              IsUpgrading() { return m_upgrading; }
	int              GetSockUpgrade() { return m_sock_upgrade; }
	UPGRADE_STATUS   GetUpgradeStatus() { return m_upgrade_status; }

private:
	char             m_ip[32];
	unsigned short   m_port;
	char             m_username[NAME_LEN];
	char             m_passwd[PASSWD_LEN];
	int              m_online;
	int              m_upgrading;
	int              m_sock_upgrade;
	UPGRADE_STATUS   m_upgrade_status;

	unsigned int     m_type;
	unsigned int     m_userid;
	/* softwareVersion softwareBuildDate */
	unsigned int     m_version;
	unsigned int     m_builddate;

};



#endif /* __SDK_DEVICE_HPP__ */

