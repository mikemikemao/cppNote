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
 * @brief 字符串安全拷贝
 *
 * @param  [in] dst : 目标缓冲区
 * @param  [in] src : 源字符串
 * @param  [in] n : dst目标缓冲区总长度
 *
 * @return  0 if ok
 */
size_t VisCore_Strlcpy(char *dst, const char *src, size_t n);

/**
 * @brief  tcp连接封装
 *
 * @param  [in] ip : 远端地址
 * @param  [in] port
 *
 * @return  > 0 if ok
 */
int sdk_tcp_connect(const char *ip, unsigned short port);

/**
 * @brief  套接字写封装
 *
 * @param  [in] connfd : connect返回的套接字
 * @param  [in] buf : 向套接字写入的数据
 * @param  [in] n
 * @param  [in] timeout_ms : 超时时间,毫秒
 *
 * @return  < 0 if err, 否则返回写入的数据量
 */
int sdk_writen(int connfd, const void *buf, const size_t n, int timeout_ms);

/**
 * @brief  套接字读封装
 *
 * @param  [in] connfd : connect返回的套接字
 * @param  [in] buf : 缓冲区
 * @param  [in] n : 读取字节数
 * @param  [in] timeout_ms : 超时时间,毫秒
 *
 * @return  < 0 if err, 否则返回读的数据量
 */
int sdk_readn(int connfd, void *buf, size_t n, int timeout_ms);

/**
 * @brief hamc算法
 *
 * @param  [in] in : 要加密的数据
 * @param  [in] in_len : 加密数据大小
 * @param  [in] key : 秘钥
 * @param  [in] key_len : 秘钥大小
 * @param  [out]digest : hamc生成摘要
 * @return void
 */
void sdk_hamc_encrypt(unsigned char *in, int in_len, unsigned char *key, int key_len, unsigned char *digest);

int convertData(char *src, char *dst, int len);

__END_DECLS


#endif /* __SDK_COMMON_HPP__ */

