/*
 * =====================================================================================
 *
 *       Filename:  sdk_test.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2016-09-06 16:08:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yingc, yinguicai@hikvision.com
 *      Copyright:  Copyright (c) 2016, HIKVISION
 *
 * =====================================================================================
 */

#include <iostream>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "sdk_client.hpp"
using namespace std;

#define SDK_USERNAME "admin"
#define SDK_PASSWD "Hik12345"
#define SDK_IP "10.15.3.70"
#define SDK_LOCA_IP "10.1.14.142"
#define SDK_LOCA_MAC "005056a208f7"

#define DIGICAP_FILE_PATH "/workteam/yinguicai/data/nfs/digicap.dav"

int main()
{
	int size;
	int fd = -1;
	unsigned int version = 0, builddate = 0;
	void *map = NULL;

	CSdkClient::GetInstance()->SetLocal(SDK_LOCA_IP, SDK_LOCA_MAC);
	CSdkClient::GetInstance()->AddDevice(SDK_IP);
	CSdkClient::GetInstance()->SetDeviceLoginInfo(SDK_IP, SDK_USERNAME, SDK_PASSWD);
	CSdkClient::GetInstance()->LoginDevice(SDK_IP);
	CSdkClient::GetInstance()->LoginOutDevice(SDK_IP);
	CSdkClient::GetInstance()->LoginDevice(SDK_IP);

	//CSdkClient::GetInstance()->GetDeviceCfg(SDK_IP);
	CSdkClient::GetInstance()->GetDeviceVersion(SDK_IP, version, builddate);
	printf("...ip =%20s, version = %-10u, builddate = %-10u...\n", SDK_IP, version, builddate);

	fd = open(DIGICAP_FILE_PATH, O_RDONLY);
	size = lseek(fd, 0, SEEK_END);
	map = (char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);

	CSdkClient::GetInstance()->SetUpgradeFile((const char*)map, size);
	CSdkClient::GetInstance()->UpgradeDevice(SDK_IP);
	CSdkClient::GetInstance()->RebootDevice(SDK_IP);
	munmap(map, size);
	close(fd);

	return 0;
}

