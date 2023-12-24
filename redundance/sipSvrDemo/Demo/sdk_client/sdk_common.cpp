/*
 * =====================================================================================
 *
 *       Filename:  sdk_common.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2016-08-25 11:34:21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yingc, yinguicai@hikvision.com
 *      Copyright:  Copyright (c) 2016, HIKVISION
 *
 * =====================================================================================
 */

#include <iostream>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/md5.h>

#include "sdk_common.hpp"
using namespace std;


/**
 * @brief 字符串安全拷贝
 *
 * @param  [in] dst : 目标缓冲区
 * @param  [in] src : 源字符串
 * @param  [in] n : dst目标缓冲区总长度
 *
 * @return  0 if ok
 */
size_t VisCore_Strlcpy(char *dst, const char *src, size_t n)
{
	const char *src0 = src;
	char dummy[1];

	if (!n) {
		dst = dummy;
	}
	else {
		--n;
	}

	while ((*dst = *src) != 0) {
		if (n) {
			--n;
			++dst;
		}
		++src;
	}

	return src - src0;
}
#if 0
/**
 * @brief  tcp连接封装
 *
 * @param  [in] ip : 远端地址
 * @param  [in] port
 *
 * @return  > 0 if ok
 */
int sdk_tcp_connect(const char *ip, unsigned short port)
{
	int sockfd = -1;
	int flag;
	struct sockaddr_in remote;
	struct in_addr addr;

	if(!(ip && port)) {
		return -1;
	}
	if (inet_aton(ip, &addr) == 0) {
		return -1;
	}

	memset(&remote, 0, sizeof(remote));
	remote.sin_addr.s_addr= addr.s_addr;
	//remote.sin_addr.s_addr= htonl(addr.s_addr);
	remote.sin_family = AF_INET;
	remote.sin_port = htons(port);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0 ) {
		return -1;
	}

	flag = 1;
	ioctl(sockfd, FIONBIO, &flag);

	if (0 > connect(sockfd, (struct sockaddr*)&remote, sizeof(remote))) {
		if (EINPROGRESS == errno) {
			int err = 0;
			int len = sizeof(int);
			fd_set set;
			struct timeval timeout;

			timeout.tv_sec = 1;
			timeout.tv_usec = 0;

			FD_ZERO(&set);
			FD_SET(sockfd,&set);

			if (select(sockfd + 1, NULL, &set, NULL, &timeout) > 0) {
				getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, (socklen_t*)&len);
				if (0 != err) {
					goto ERR;
				}
			}
			else {
				goto ERR;
			}
		}
	}

	flag = 0;
	ioctl(sockfd, FIONBIO, &flag);

	return sockfd;

ERR:
	close(sockfd);
	return -1;
}


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
int sdk_writen(int connfd, const void *buf, const size_t n, int timeout_ms)
{
	int nleft, nwritten;
	char *ptr;
	struct timeval  timeout;
	fd_set wset;

	ptr = (char*)buf;
	nleft = n;
	timeout.tv_sec = timeout_ms/1000;
	timeout.tv_usec = 1000*(timeout_ms%1000);

	while (nleft > 0) {
		FD_ZERO(&wset);
		FD_SET(connfd, &wset);

		if (0 > select(connfd + 1, NULL, &wset, NULL, &timeout)) {
			return -1;
		}

		if (0 > (nwritten = send(connfd, ptr, nleft, 0))) {
			if ((errno == EINTR) || (errno == EAGAIN)) {
				nwritten = 0;
			}
			else {
				return -1;
			}
		}
		nleft -= nwritten;
		ptr   += nwritten;
	}

	return (n - nleft);
}

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
int sdk_readn(int connfd, void *buf, size_t n, int timeout_ms)
{
	int ret;
	int nleft;
	int nread;
	char *ptr;
	struct timeval  timeout;
	fd_set rset;

	ptr = (char*)buf;
	nleft = n;
	timeout.tv_sec = timeout_ms/1000;
	timeout.tv_usec = 1000*(timeout_ms%1000);

	while (nleft > 0) {
		FD_ZERO(&rset);
		FD_SET(connfd, &rset);
		ret = select(connfd+1, &rset, NULL, NULL, &timeout);
		if (ret < 0) {
			if ((errno == EINTR) || (errno == EAGAIN)) {
				continue;
			}
			else {
				return -1;
			}
		}
		else if (ret == 0) {
			/* time out */
			break;
		}

		if ((nread = recv(connfd, ptr, nleft, 0)) < 0) {
			if ((errno == EINTR) || (errno == EAGAIN)) {
				nread = 0;
			}
			else {
				return -2;
			}
		}
		nleft -= nread;
		ptr   += nread;
	}

	return (n - nleft);
}
#endif

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
void sdk_hamc_encrypt(unsigned char *in, int in_len, unsigned char *key, int key_len, unsigned char *digest)
{
	MD5_CTX context;
	unsigned char k_ipad[65] = {0};    /* inner padding - key XORd with ipad*/
	unsigned char k_opad[65] = {0};    /* outer padding - key XORd with opad*/
	unsigned char tk[16] = {0};
	int i = 0;
	MD5_CTX tctx;

	/* if key is longer than 64 bytes reset it to key=MD5(key) */
	if (key_len > 64) {
		MD5_Init(&tctx);
		MD5_Update(&tctx, key, key_len);
		MD5_Final(tk, &tctx);

		key = tk;
		key_len = 16;
	}

	/*
	 * the HMAC_MD5 transform looks like:
	 *
	 * MD5(K XOR opad, MD5(K XOR ipad, text))
	 *
	 * where K is an n byte key
	 * ipad is the byte 0x36 repeated 64 times
	 * opad is the byte 0x5c repeated 64 times
	 * and text is the data being protected
	 */

	/* start out by storing key in pads */
	memset( k_ipad,0, sizeof(k_ipad));
	memset( k_opad,0, sizeof(k_opad));
	memcpy( k_ipad, key,key_len);
	memcpy( k_opad, key,key_len);

	/* XOR key with ipad and opad values */
	for (i = 0; i < 64; i++) {
		k_ipad[i] ^= 0x36;
		k_opad[i] ^= 0x5c;
	}

	/*
	 * perform inner MD5
	 */
	MD5_Init(&context);                     /* init context for 1st pass */
	MD5_Update(&context, k_ipad, 64);       /* start with inner pad */
	MD5_Update(&context, in, in_len);   /* then text of datagram */
	MD5_Final(digest, &context);            /* finish up 1st pass */

	/*
	 * perform outer MD5
	 */
	MD5_Init(&context);                     /* init context for 2nd pass */
	MD5_Update(&context, k_opad, 64);       /* start with outer pad */
	MD5_Update(&context, digest, 16);       /* then results of 1st hash */
	MD5_Final(digest, &context);            /* finish up 2nd pass */

	return;
}

/*+++++++++++++++++++++++++
 * Function:       convertData
 * Description:   利用简单的异或进行数据变换，用于升级文件的打包和解包  
 * Input:          src - source data
                       len - data length
 * Output:     dst - destination  data
 * Return:     0 -- OK / -1 -- Failed      
 *-----------------------------*/
int convertData(char *src, char *dst, int len)
{
    /* 固定的幻数，用于异或变换 */
    unsigned char magic[] = {0xba, 0xcd, 0xbc, 0xfe, 0xd6, 0xca, 0xdd, 0xd3,
                    0xba, 0xb9, 0xa3, 0xab, 0xbf, 0xcb, 0xb5, 0xbe};
    int i, j;
    int magiclen, startmagic;

    if(src==NULL || dst==NULL)
    {
        return -1;
    }

    magiclen = sizeof(magic);
    

    i = startmagic = 0;
    while(i < len)
    {
        /* 用startmagic控制每次内循环magic的起始位置 */
        for(j=0; j<magiclen && i<len; j++, i++)
        {
            *dst++ = *src++ ^ magic[(startmagic+j)%magiclen];   /* 进行异或变换 */
        }
        startmagic = (startmagic+1) % magiclen;
    }
    return 0;
}

