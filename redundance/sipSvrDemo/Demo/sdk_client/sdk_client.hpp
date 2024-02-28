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
	 * @brief ��ȡ�ͻ���ʵ��
	 *
	 * @return �ǿ�if ok
	 */
	static CSdkClient *GetInstance();

public:
	/**
	 * @brief ���ñ��ؿͻ���ip��mac��ַ
	 *
	 * @param  [in] local_ip
	 * @param  [in] local_mac
	 *
	 * @return  0 if ok
	 */
	int SetLocal(const char *local_ip, const char *local_mac);

	/**
	 * @brief �ͻ��������豸
	 *
	 * @param  [in] ip : �豸ip
	 *
	 * @return  0 if ok
	 */
	int AddDevice(const char *ip);

	/**
	 * @brief  �ͻ���ɾ���豸
	 *
	 * @param  [in] ip : �豸ip
	 *
	 * @return  0 if ok
	 */
	int DelDevice(const char *ip);

	/**
	 * @brief  �����豸ip���û���������
	 *
	 * @param  [in] ip : �豸ip
	 * @param  [in] username
	 * @param  [in] passwd
	 *
	 * @return  0 if ok
	 */
	int SetDeviceLoginInfo(const char *ip, const char *username, const char *passwd);

	/**
	 * @brief  ���������ļ�
	 *
	 * @param  [in] buf : �����ļ�����
	 * @param  [in] size : �����ļ���С
	 *
	 * @return  0 if ok
	 */
	int SetUpgradeFile(const char *buf, const int size);

	/**
	 * @brief  ��¼ָ��ip���豸
	 *
	 * @param  [in] ip
	 *
	 * @return  0 if ok
	 */
	int LoginDevice(const char *ip);

	/**
	 * @brief ����ָ��ip���豸
	 *
	 * @param  [in] ip
	 *
	 * @return  0 if ok
	 */
	int RebootDevice(const char *ip);

	/**
	 * @brief ����ָ�����豸
	 *
	 * @param  [in] dev
	 *
	 * @return  0 if ok
	 */
	int RebootDevice(CSdkDevice *dev);

	/**
	 * @brief  �ǳ�ָ��ip���豸
	 *
	 * @param  [in] ip
	 *
	 * @return 0 if ok
	 */
	int LoginOutDevice(const char *ip);

	/**
	 * @brief  �ǳ�ָ�����豸
	 *
	 * @param  [in] dev
	 *
	 * @return 0 if ok
	 */
	int LoginOutDevice(CSdkDevice *dev);

	/**
	 * @brief  ����ָ��ip���豸
	 *
	 * @param  [in] ip
	 *
	 * @return  0 if ok
	 */
	int UpgradeDevice(const char *ip);

	/**
	 * @brief  ����ָ�����豸
	 *
	 * @param  [in] dev
	 *
	 * @return  0 if ok
	 */
	int UpgradeDevice(CSdkDevice *dev);

	/**
	 * @brief ��ȡָ���豸ip�������汾�͹�������
	 *
	 * @param  [in] ip
	 * @param  [out]version
	 * @param  [out]builddate
	 *
	 * @return 0 if ok
	 */
	int GetDeviceVersion(const char *ip, unsigned int &version, unsigned int &builddate);

	/**
	 * @brief ��ȡָ���豸ip������
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
	 * @brief  ��ʼ��rsa����Դ
	 *
	 * @return
	 */
	int Init();

	/**
	 * @brief �����豸��¼����1
	 * ע��: sdk��¼����������
	 * @param  [out]buf : ����buf
	 * @param  [in] buf_size
	 *
	 * @return  0 if ok
	 */
	int MakeLoginReqStep1(char *buf, const int buf_size);

	/**
	 * @brief �����豸��¼����2
	 * ע��: sdk��¼����������
	 * @param  [in] dev : �豸ʵ��
	 * @param  [out]buf : ����buf
	 * @param  [in] buf_size
	 * @param  [in] key : �豸���ص���ս��
	 * @param  [in] key_len : ��ս������
	 *
	 * @return  0 if ok
	 */
	int MakeLoginReqStep2(CSdkDevice *dev, char *buf, const int buf_size, const char *key, const int key_len);

	/**
	 * @brief �����豸raw����
	 *
	 * @param  [in] dev : �豸ʵ��
	 * @param  [out]buf : ����buf
	 * @param  [in] buf_size
	 * @param  [in] cmd : sdk������
	 *
	 * @return  0 if ok
	 */
	int MakeRawSdkReq(CSdkDevice *dev, char *buf, const int buf_size, const unsigned int cmd);

	
	/**
	 * @brief �������Ȩ����֤����
	 * @param  [in] dev : �豸ʵ��
	 * @param  [out]buf : ����buf
	 * @param  [in] buf_size
	 *
	 * @return	0 if ok
	 */
	int MakeVerifiedReq(CSdkDevice *dev, char *buf, const int buf_size);

	/**
	 * @brief ��ȡָ���豸ip�����ò���
	 *
	 * @param  [in] ip
	 *
	 * @return  0 if ok
	 */
	int GetDeviceCfg(const char *ip);

	/**
	 * @brief ��ȡָ���豸�����ò���
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
