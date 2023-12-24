/*
 * =====================================================================================
 *
 *       Filename:  sdk_client.hpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2016-08-25 16:18:48
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yingc, yinguicai@hikvision.com
 *   Organization:  HIKVISION
 *
 * =====================================================================================
 */

#ifndef __SDK_CLIENT_HPP__
#define __SDK_CLIENT_HPP__

#include <iostream>
#include <map>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include "sdk_device.hpp"

using namespace std;

#define RSA_PUB_BIN_LEN (140)
#define ENCRYP_CHAN_LEN (128)
#define SDK_PORT (8000)
#define SEND_BLOCK_SIZE (256*1024)

class CSdkClient {
public:
	/**
	 * @brief 获取客户端实例
	 *
	 * @return 非空if ok
	 */
	static CSdkClient *GetInstance();

public:
	/**
	 * @brief 设置本地客户端ip和mac地址
	 *
	 * @param  [in] local_ip
	 * @param  [in] local_mac
	 *
	 * @return  0 if ok
	 */
	int SetLocal(const char *local_ip, const char *local_mac);

	/**
	 * @brief 客户端添加设备
	 *
	 * @param  [in] ip : 设备ip
	 *
	 * @return  0 if ok
	 */
	int AddDevice(const char *ip);

	/**
	 * @brief  客户端删除设备
	 *
	 * @param  [in] ip : 设备ip
	 *
	 * @return  0 if ok
	 */
	int DelDevice(const char *ip);

	/**
	 * @brief  设置设备ip的用户名和密码
	 *
	 * @param  [in] ip : 设备ip
	 * @param  [in] username
	 * @param  [in] passwd
	 *
	 * @return  0 if ok
	 */
	int SetDeviceLoginInfo(const char *ip, const char *username, const char *passwd);

	/**
	 * @brief  设置升级文件
	 *
	 * @param  [in] buf : 升级文件缓存
	 * @param  [in] size : 升级文件大小
	 *
	 * @return  0 if ok
	 */
	int SetUpgradeFile(const char *buf, const int size);

	/**
	 * @brief  登录指定ip的设备
	 *
	 * @param  [in] ip
	 *
	 * @return  0 if ok
	 */
	int LoginDevice(const char *ip);

	/**
	 * @brief 重启指定ip的设备
	 *
	 * @param  [in] ip
	 *
	 * @return  0 if ok
	 */
	int RebootDevice(const char *ip);

	/**
	 * @brief 重启指定的设备
	 *
	 * @param  [in] dev
	 *
	 * @return  0 if ok
	 */
	int RebootDevice(CSdkDevice *dev);

	/**
	 * @brief  登出指定ip的设备
	 *
	 * @param  [in] ip
	 *
	 * @return 0 if ok
	 */
	int LoginOutDevice(const char *ip);

	/**
	 * @brief  登出指定的设备
	 *
	 * @param  [in] dev
	 *
	 * @return 0 if ok
	 */
	int LoginOutDevice(CSdkDevice *dev);

	/**
	 * @brief  升级指定ip的设备
	 *
	 * @param  [in] ip
	 *
	 * @return  0 if ok
	 */
	int UpgradeDevice(const char *ip);

	/**
	 * @brief  升级指定的设备
	 *
	 * @param  [in] dev
	 *
	 * @return  0 if ok
	 */
	int UpgradeDevice(CSdkDevice *dev);

	/**
	 * @brief 获取指定设备ip的软件版本和构建日期
	 *
	 * @param  [in] ip
	 * @param  [out]version
	 * @param  [out]builddate
	 *
	 * @return 0 if ok
	 */
	int GetDeviceVersion(const char *ip, unsigned int &version, unsigned int &builddate);

	/**
	 * @brief 获取指定设备ip的类型
	 *
	 * @param  [in] ip
	 * @param  [out] type
	 *
	 * @return 0 if ok
	 */
	int GetDeviceType(const char *ip, unsigned int &type);
#if 0
	int StartUpgrade();
	int StopUpgrade();

	int AddUpgradeJob(const char *ip);
	int RemoveUpgradeJob(const char *ip);
	int GetDeviceVersion(const char *ip);
#endif

private:
	/**
	 * @brief  construct
	 *
	 * @return
	 */
	CSdkClient();

	/**
	 * @brief  deconstruct
	 *
	 * @return
	 */
	virtual ~CSdkClient();

	/**
	 * @brief  初始化rsa等资源
	 *
	 * @return
	 */
	int Init();

	/**
	 * @brief 构造设备登录请求1
	 * 注意: sdk登录分两部进行
	 * @param  [out]buf : 请求buf
	 * @param  [in] buf_size
	 *
	 * @return  0 if ok
	 */
	int MakeLoginReqStep1(char *buf, const int buf_size);

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
	int MakeLoginReqStep2(CSdkDevice *dev, char *buf, const int buf_size, const char *key, const int key_len);

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
	int MakeRawSdkReq(CSdkDevice *dev, char *buf, const int buf_size, const unsigned int cmd);

	
	/**
	 * @brief 构造操作权限验证请求
	 * @param  [in] dev : 设备实例
	 * @param  [out]buf : 请求buf
	 * @param  [in] buf_size
	 *
	 * @return	0 if ok
	 */
	int MakeVerifiedReq(CSdkDevice *dev, char *buf, const int buf_size);

	/**
	 * @brief 获取指定设备ip的配置参数
	 *
	 * @param  [in] ip
	 *
	 * @return  0 if ok
	 */
	int GetDeviceCfg(const char *ip);

	/**
	 * @brief 获取指定设备的配置参数
	 *
	 * @param  [in] dev
	 *
	 * @return  0 if ok
	 */
	int GetDeviceCfg(CSdkDevice *dev);

private:
	static CSdkClient *m_instance;

private:
	int m_init_ok;
	char m_local_ip[32];
	char m_local_mac[6];
	map<string, CSdkDevice*> m_mdev;

	//rsa dec & enc
	RSA *m_rsa;
	unsigned char *m_pub;

	//upgrade file info
	char *m_upgrade;
	int m_upgrade_size;
};



#endif /* __SDK_CLIENT_HPP__ */

