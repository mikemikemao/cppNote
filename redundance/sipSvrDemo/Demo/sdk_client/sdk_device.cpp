/*
 * =====================================================================================
 *
 *       Filename:  sdk_device.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2016-08-26 11:21:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yingc, yinguicai@hikvision.com
 *      Copyright:  Copyright (c) 2016, HIKVISION
 *
 * =====================================================================================
 */

#include "sdk_command.h"
#include "sdk_common.hpp"

#include "sdk_device.hpp"
using namespace std;

CSdkDevice::CSdkDevice()
{
	m_port = 0;
	m_online = 0;
	m_upgrading = 0;
	m_type = 0;
	m_userid = 0;
	m_version = 0;
	m_builddate = 0;
	m_sock_upgrade = -1;
	m_upgrade_status = UPGRADE_STATUS_NONE;

	memset(m_ip, 0, sizeof(m_ip));
	memset(m_username, 0, sizeof(m_username));
	memset(m_passwd, 0, sizeof(m_passwd));
}

CSdkDevice::~CSdkDevice()
{

}

void CSdkDevice::SetIp(const char *ip)
{
	if (ip) {
		VisCore_Strlcpy(m_ip, ip, sizeof(m_ip));
	}
}

void CSdkDevice::SetUsername(const char *username)
{
	if (username) {
		VisCore_Strlcpy(m_username, username, sizeof(m_username));
	}
}

void CSdkDevice::SetPasswd(const char *passwd)
{
	if (passwd) {
		VisCore_Strlcpy(m_passwd, passwd, sizeof(m_passwd));
	}
}






