/*****************************************************
*  Copyright 2009 Hikvision Digital Technology Co., Ltd.
*
*  FileName:
*      cmsws_platform.cpp
*
*  Description:
*      提供一些实用的跨平台函数，但仅定义 Windows 版本。
*
*  Modification History:
*    <version>    <time>        <author>      <desc>
*    1.0          2009-10-28    Hu Tonghai    Create
*****************************************************/
#include "cmsws_platform.h"

#if (defined(_WIN32) || defined(_WIN32_WCE))
#include <Windows.h>
#else
#include "iconv.h"
#endif


char* platform_mb2u8(cmsws_soap* soapEnv, const char* cont)
{
#if (defined(_WIN32) || defined(_WIN32_WCE))
    int num = MultiByteToWideChar(CP_ACP, NULL, cont, -1, NULL, NULL);
    wchar_t* buffw = new wchar_t[num];
    MultiByteToWideChar(CP_ACP, NULL, cont, -1, buffw, num);

    int len = WideCharToMultiByte(CP_UTF8, 0, buffw, num - 1, NULL, NULL, NULL, NULL); 
    char* lpsz = (char*)soap_malloc(soapEnv, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, buffw, num - 1, lpsz, len, NULL, NULL);
    lpsz[len] = '\0';
    delete[] buffw;

    return lpsz ;
#else
    char* outbuf = 0;

    if (cont && soapEnv)
    {
        iconv_t cd = iconv_open("UTF-8", "EUC-CN");
        if (cd)
        {
            size_t contlen = strlen(cont);
            size_t outbuflen = contlen * 3 + 1;
            outbuf = (char*)soap_malloc(soapEnv, outbuflen);
            memset(outbuf, 0, outbuflen);

            char* inptr = const_cast<char*>(cont);
            char* outptr = outbuf;
            size_t inlen = contlen;
            size_t outlen = outbuflen;

            if (iconv(cd, (const char**)&inptr, &inlen, &outptr, &outlen) == (size_t)(-1)) 
            {
                outbuf = 0;
            }

            iconv_close(cd); 
        }
    }

    return outbuf;
#endif
}

char* platform_u82mb(cmsws_soap* soapEnv, const char* cont)
{
#if (defined(_WIN32) || defined(_WIN32_WCE))
    if (!cont || !soapEnv)
    {
        return 0;
    }

    int num = MultiByteToWideChar(CP_UTF8, NULL, cont, -1, NULL, NULL);
	wchar_t* buffw = new(std::nothrow) wchar_t[num];
	if (buffw == NULL)
	{
		return NULL;
	}
    MultiByteToWideChar(CP_UTF8, NULL, cont, -1, buffw, num);

    int len = WideCharToMultiByte(CP_ACP, 0, buffw, num - 1, NULL, NULL, NULL, NULL); 
    char* lpsz = (char*)soap_malloc(soapEnv, len + 1);
    WideCharToMultiByte(CP_ACP, 0, buffw, num - 1, lpsz, len, NULL, NULL);
    lpsz[len] = '\0';
    delete[] buffw;

    return lpsz;
#else
    char* outbuf = 0;

    if (cont && soapEnv)
    {
        iconv_t cd = iconv_open("EUC-CN", "UTF-8");
        if (cd)
        {
            size_t contlen = strlen(cont);
            size_t outbuflen = contlen + 1;
            outbuf = (char*)soap_malloc(soapEnv, outbuflen);
            memset(outbuf, 0, outbuflen);

            char* inptr = const_cast<char*>(cont);
            char* outptr = outbuf;
            size_t inlen = contlen;
            size_t outlen = outbuflen;

            if (iconv(cd, (const char**)&inptr, &inlen, &outptr, &outlen) == (size_t)(-1)) 
            {
                outbuf = 0;
            }

            iconv_close(cd); 
        }
    }

    return outbuf;
#endif
}


wchar_t* platform_u82wc(cmsws_soap* soapEnv, const char* cont)
{
#if (defined(_WIN32) || defined(_WIN32_WCE))
    if (!cont || !soapEnv)
    {
        return 0;
    }

    int num = MultiByteToWideChar(CP_UTF8, NULL, cont, -1, NULL, NULL);
    wchar_t* lpwsz = (wchar_t*)soap_malloc(soapEnv, num + 1);
    MultiByteToWideChar(CP_UTF8, NULL, cont, -1, lpwsz, num);
    return lpwsz;
#else
    char* outbuf = 0;

    if (cont && soapEnv)
    {
        iconv_t cd = iconv_open("EUC-CN", "UTF-8");
        if (cd)
        {
            size_t contlen = strlen(cont);
            size_t outbuflen = contlen + 1;
            outbuf = (char*)soap_malloc(soapEnv, outbuflen);
            memset(outbuf, 0, outbuflen);

            char* inptr = const_cast<char*>(cont);
            char* outptr = outbuf;
            size_t inlen = contlen;
            size_t outlen = outbuflen;

            if (iconv(cd, (const char**)&inptr, &inlen, &outptr, &outlen) == (size_t)(-1)) 
            {
                outbuf = 0;
            }

            iconv_close(cd); 
        }
    }

    return outbuf;
#endif
}
