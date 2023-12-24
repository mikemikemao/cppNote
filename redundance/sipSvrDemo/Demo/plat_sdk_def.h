/** @file:   plat_sdk_def.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  平台相关接口定义
 *
 *  @author: wangfeida@hikvision.com.cn
 *  @data:   2014/08/04
 *  
 *  @note:   
 *
 *  @Modification History:
 *  <version>       <time>          <author>        <desc>
 *    V1.0        2014/08/04        wangfeida       Create
 *
 *  @warning: <No waining>        
*/

#ifndef _PLAT_SDK_DEF_H_
#define _PLAT_SDK_DEF_H_


#if (defined(_WIN32) || defined(_WIN64))
#  define SIP_PLAT_SDK_EXTERN extern "C" __declspec(dllexport)
#  define SIP_PLAT_SDK_API __stdcall
#elif defined(__linux__)
#  define SIP_PLAT_SDK_EXTERN extern "C"
#  define SIP_PLAT_SDK_API
#else
#  define SIP_PLAT_SDK_EXTERN
#  define SIP_PLAT_SDK_API
#endif

typedef int (SIP_PLAT_SDK_API *sip_sdk_init)(const char*);

typedef void (SIP_PLAT_SDK_API *sip_sdk_finit)(void);

typedef char* (SIP_PLAT_SDK_API *sip_sdk_upload_record)(const char*, const char*, const char*);

typedef int (SIP_PLAT_SDK_API *sip_sdk_get_last_error)(void);

typedef const char* (SIP_PLAT_SDK_API *sip_sdk_version)(void);

typedef int (SIP_PLAT_SDK_API *sip_sdk_free_temporary_data)(void);

#endif