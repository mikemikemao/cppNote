/*
 * =====================================================================================
 *
 *       Filename:  sdk_common.hpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2016-08-25 11:33:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yingc, yinguicai@hikvision.com
 *   Organization:  HIKVISION
 *
 * =====================================================================================
 */

#ifndef __SDK_COMMON_HPP__
#define __SDK_COMMON_HPP__

#include "sdk_config.h"
#include "sdk_types.h"

#ifndef ARRAY_LEN
#define ARRAY_LEN(array) ((sizeof(array)) / (sizeof(array[0])))
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(p) do{if(p){free(p);(p=NULL);}}while(0)
#endif

#ifndef SAFE_CLOSE
#define SAFE_CLOSE(fd) do{if(fd > 0){close(fd);fd=-1;}}while(0)
#endif

#ifndef HK_MIN
#define HK_MIN(a,b) ((a) > (b) ? (b) : (a))
#endif

#ifndef HK_MAX
#define HK_MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#define SDK_DEBUG

#if 1
#ifdef SDK_DEBUG
#define SDK_INFO(...) \
	do {\
		printf("[SDKC INFO]:%-20s, %-10d:    ", __FUNCTION__, __LINE__); \
		printf(__VA_ARGS__); \
	}while(0)
#else
#define SDK_INFO(...) do{}while(0)
#endif

#define SDK_ERR(...) \
	do {\
		printf("[SDKC ERR]:%-20s, %-10d:    ", __FUNCTION__, __LINE__); \
		printf(__VA_ARGS__); \
	}while(0)
#else


#ifdef SDK_DEBUG
#define SDK_INFO(...) do{}while(0)
#else
#define SDK_INFO(...) do{}while(0)
#endif

#define SDK_ERR(...) do{}while(0)

#endif

__BEGIN_DECLS

/**
 * @brief �ַ�����ȫ����
 *
 * @param  [in] dst : Ŀ�껺����
 * @param  [in] src : Դ�ַ���
 * @param  [in] n : dstĿ�껺�����ܳ���
 *
 * @return  0 if ok
 */
size_t VisCore_Strlcpy(char *dst, const char *src, size_t n);

/**
 * @brief  tcp���ӷ�װ
 *
 * @param  [in] ip : Զ�˵�ַ
 * @param  [in] port
 *
 * @return  > 0 if ok
 */
int sdk_tcp_connect(const char *ip, unsigned short port);

/**
 * @brief  �׽���д��װ
 *
 * @param  [in] connfd : connect���ص��׽���
 * @param  [in] buf : ���׽���д�������
 * @param  [in] n
 * @param  [in] timeout_ms : ��ʱʱ��,����
 *
 * @return  < 0 if err, ���򷵻�д���������
 */
int sdk_writen(int connfd, const void *buf, const size_t n, int timeout_ms);

/**
 * @brief  �׽��ֶ���װ
 *
 * @param  [in] connfd : connect���ص��׽���
 * @param  [in] buf : ������
 * @param  [in] n : ��ȡ�ֽ���
 * @param  [in] timeout_ms : ��ʱʱ��,����
 *
 * @return  < 0 if err, ���򷵻ض���������
 */
int sdk_readn(int connfd, void *buf, size_t n, int timeout_ms);

/**
 * @brief hamc�㷨
 *
 * @param  [in] in : Ҫ���ܵ�����
 * @param  [in] in_len : �������ݴ�С
 * @param  [in] key : ��Կ
 * @param  [in] key_len : ��Կ��С
 * @param  [out]digest : hamc����ժҪ
 * @return void
 */
void sdk_hamc_encrypt(unsigned char *in, int in_len, unsigned char *key, int key_len, unsigned char *digest);

int convertData(char *src, char *dst, int len);

__END_DECLS


#endif /* __SDK_COMMON_HPP__ */

