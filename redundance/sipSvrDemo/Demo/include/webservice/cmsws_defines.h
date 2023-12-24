#pragma once
/**************************************************************************	
 *  @file cmsws_defines.h
 *	@note HangZhou Hikvision System Technology Co., Ltd. All Right Reserved.
 *	@brief 这里填写本文件的摘要。
 *
 *	@author		Wang Zhaojun
 *	@date		2013/01/05
 *
 *	@note 历史记录：
 *	@note V1.0.0  创建
 *
 *	@warning 公共数据结构定义，如果变更，其它模块必须重新生成，不能直接引用头文件
**************************************************************************/

#if (defined(_WIN32) || defined(_WIN32_WCE))

    //
    // Summary:
    //     cmsws_web_service export / import
    //
    #ifdef HIK_CMSWS_DLL_EXPORTS
    #define CMSWS_EXTERN __declspec(dllexport)
    #else
    #define CMSWS_EXTERN __declspec(dllimport)
    #endif

    //
    // Summary:
    //     CMS WebService API 类型定义
    //
    #define CMSWS_API __stdcall

    //
    // Summary:
    //     TLS 声明宏
    //
    #define TLS_DECLARE __declspec(thread)

    //
    // Summary:
    //     base type
    //
    typedef short               cmsws_int16;
    typedef unsigned short      cmsws_uint16;
    typedef int                 cmsws_int32;
    typedef unsigned int        cmsws_uint32;
    typedef __int64             cmsws_int64;
    typedef unsigned __int64    cmsws_uint64;

    typedef cmsws_int32         cmsws_bool;
    #define cmsws_true          1
    #define cmsws_false         0

#elif (defined(__linux__) || defined(__APPLE__))

    //
    // Summary:
    //     cmsws_web_service export / import
    //
    #define CMSWS_EXTERN extern "C" 

    //
    // Summary:
    //     CMS WebService API 类型定义
    //
    #define CMSWS_API 

    //
    // Summary:
    //     TLS 声明宏
    //
    #define TLS_DECLARE __thread

    //
    // Summary:
    //     base type
    //
    typedef short                   cmsws_int16;
    typedef unsigned short          cmsws_uint16;
    typedef int                     cmsws_int32;
    typedef unsigned int            cmsws_uint32;
    #if defined(__LP64__)
        typedef signed long         cmsws_int64;
        typedef unsigned long       cmsws_uint64;
    #else
        typedef signed long long    cmsws_int64;
        typedef unsigned long long  cmsws_uint64;
    #endif	//defined(__LP64__)

    typedef cmsws_int32         cmsws_bool;
    #define cmsws_true          1
    #define cmsws_false         0

#endif


//
// Summary:
//     include stdsoap2.h
//
#include "stdsoap2.h"


//
// Summary:
//     typedef cmsws_soap
//
typedef struct soap cmsws_soap;

//
// Summary:
//     typedef cmsws_namespace
//
typedef struct Namespace cmsws_namespace;


//
// Summary:
//     NULL
//
#ifndef NULL
#define NULL 0
#endif

//
// Summary:
//     __cdecl
//
#if (!defined(_MSC_VER) && !defined(__cdecl))
#define __cdecl
#endif


//
// Summary:
//     cmsws 返回值定义。
//
#define CMSWS_SUCCESS   1   // 操作成功
#define CMSWS_FAILURE   0   // 操作失败


//
// Summary:
//     cmsws service stub 错误定义。
//
#define CMSWS_SOAP_PARAM_ERROR          0x01000001      // 传入参数无效
#define CMSWS_SOAP_MALLOC_ERROR         0x01000002      // 分配内存错误
#define CMSWS_SOAP_RESPONSE_NIL         0x01000003      // 服务器回复的 XML 为空
#define CMSWS_SOAP_ERROR_CODE_NIL       0x01000004      // 服务器回复的 errorCode 为空

//
// Summary:
//     cmsws service skel 错误定义。
//
#define CMSWS_SOAP_SKEL_NO_SERVICE_ENTRY    0x02000001  // 服务函数入口不存在


//
// Summary:
//     调用 CMS WebService 返回的 errorCode 定义
//
#define CMS_SUCCESSED                   0                //操作成功
#define CMS_FAILED                      10000            //操作失败

//用户登录错误码
#define CMS_USERNAME_ERROR              10001            //用户名不存在
#define CMS_PWD_ERROR                   10002            //密码错误
#define CMS_USER_HAS_LOGIN              10003            //用户已经登录
#define CMS_USER_NOT_EXCLUSIVE          10004            //用户名称不唯一
#define CMS_USER_EXPIRED                10005            //用户已经过期
#define CMS_USER_DELETE                 10006            //用户被删除
#define CMS_MAC_WRONG					10007			//用户绑定MAC地址出错
#define CMS_USER_NOT_ACTIVE		        10008			//用户未被激活
#define CMS_USER_NORIGHT				10009				//用户没有权限
#define CMS_USERSESSION_NOT_EXIST       10010            //用户会话ID不存在
#define CMS_USER_RIGHT_EXPIRED		    10011			//用户权限已过期		[guchen 20120221]

//添加错误码
#define CMS_INDEX_CODE_HAS_EXIST        20001            //索引编码已经存在
#define CMS_PARENT_ID_ERROR             20002            //夫ID错误
#define CMS_ID_NOT_EXIST                20003            //ID值不存在
#define CMS_USER_NAME_EXIST             20004            //用户名已经存在
#define CMS_INTERNAL_ERROR              20005            //系统内部错误

//数据库查询错误码
#define CMS_SELECT_NONE                 30001            //查询不到内容

//License相关错误 20121009
#define CMS_NO_LICENSE 40001 // 系统没有license
#define CMS_LICENSE_EXPIRED 40002 // license已过期
#define CMS_DOG_EXPIRED 40003 // 加密狗超出试用期	

//
//
// Summary:
//     ZeroMemory
//
#ifndef ZeroMemory
#define ZeroMemory(dst, size) memset(dst, 0, size)
#endif


//
// Summary:
//     检查 runtime environment 。
//
#define GET_SOAP_ENV(rtn) \
    cmsws_soap* soapEnv = SoapWrapper::GetSoapEnv(); \
    if (!soapEnv) \
    { \
        return (rtn); \
    }

//
// Summary:
//     申请空间，并拷贝 (char*)psz 内容。
//
#define COPY_PSZ(pszDst, pszSrc) \
    { \
        (pszDst) = 0; \
        if ((void*)(pszSrc)) \
        { \
            int size = strlen((pszSrc)); \
            if (size > 0) \
            { \
                char* tmp = (char*)soap_malloc((soapEnv), size + 1); \
                if (tmp) \
                { \
                    memcpy(tmp, (pszSrc), size); \
                    *(tmp + size) = '\0'; \
                    (pszDst) = tmp; \
                } \
            } \
        } \
    }

//
// Summary:
//     检查参数是否符合条件，若否，则设置错误信息后返回。
//
#define CHECK_PARAM_AND_SET_ERROR_AND_RETURN(param, condition, nError, pszFault, pszDetail, rtn) \
    if ((condition) == (param)) \
    { \
        char* faultString = 0; \
        COPY_PSZ(faultString, pszFault) \
        char* detailString = 0; \
        COPY_PSZ(detailString, pszDetail); \
        soap_set_sender_error((soapEnv), faultString, detailString, (nError)); \
        return (rtn); \
    }

//
// Summary:
//     为应对 gSoap 对空的指针数组序列化为具有一个空元素的数组，故对此种情况纠正数组的长度。
//
#define CHECK_ARRAY_SIZE(pointer_array) \
    if (!(pointer_array) || ((pointer_array ## Length) == 1 && !(*(pointer_array)))) \
    { \
        (pointer_array ## Length) = 0; \
    }

//
// Summary:
//     检查输入参数。
//
#define CHECK_PARAM(param, condition, pszFault, rtn) \
    CHECK_PARAM_AND_SET_ERROR_AND_RETURN(param, condition, CMSWS_SOAP_PARAM_ERROR, pszFault, NULL, rtn)

//
// Summary:
//     检查内存分配。
//
#define CHECK_MALLOC(param, pszFault, rtn) \
    CHECK_PARAM_AND_SET_ERROR_AND_RETURN(param, NULL, CMSWS_SOAP_MALLOC_ERROR, pszFault, NULL, rtn)

//
// Summary:
//     检查 axis2 webservice response 参数。
//
#define CHECK_RESPONSE(response, rtn) \
    CHECK_PARAM_AND_SET_ERROR_AND_RETURN(response->return_, NULL, CMSWS_SOAP_RESPONSE_NIL, NULL, NULL, rtn)

//
// Summary:
//     检查 NVR Web Service 入口。
//
#define CHECK_SERVICE_ENTRY(entry, pszFault, rtn) \
    CHECK_PARAM_AND_SET_ERROR_AND_RETURN(entry, NULL, CMSWS_SOAP_SKEL_NO_SERVICE_ENTRY, pszFault, NULL, rtn)


//
// Summary:
//     获取 service endpoint uri 。
//
#define GET_ENDPOINT_URI(newUri) \
    ((newUri) ? (newUri) : SoapWrapper::GetEndpointUri())


//
// Summary:
//     提取 string* 为 const char* 。
//
// Remarks:
//     不再使用 STL string ，改用 char* 。
//
//     需要对传递的字符串进行 UTF-8 到 MB 转码。
//
// #define GET_C_STR(string_pointer) ((string_pointer) ? (string_pointer)->c_str() : 0)
// #define GET_C_STR(string_pointer) ((const char*)(string_pointer))
#define GET_C_STR(string_pointer) (platform_u82mb((soapEnv), (const char*)(string_pointer)))

//
// Summary:
//     提取 *(std_type_pointer) 的 value 。
//
#define GET_STV_DEFAULT(std_type_pointer, default_value) \
    ((std_type_pointer) ? *(std_type_pointer) : (default_value))


//
// Summary:
//     申请空间，并拷贝 (char*)psz 内容。
//
// Remarks:
//     需要对获取的字符串进行 MB 到 UTF-8 转码。
//
//#define CHECK_COPY_PSZ(pszDst, pszSrc, pszFault, rtn) \
//    { \
//        (pszDst) = 0; \
//        if ((pszSrc)) \
//        { \
//            int size = strlen((pszSrc)); \
//            if (size > 0) \
//            { \
//                char* tmp = (char*)soap_malloc((soapEnv), size + 1); \
//                CHECK_MALLOC(tmp, (pszFault), (rtn)); \
//                memcpy(tmp, (pszSrc), size); \
//                *(tmp + size) = '\0'; \
//                (pszDst) = tmp; \
//            } \
//        } \
//    }
#define CHECK_COPY_PSZ(pszDst, pszSrc, pszFault, rtn) \
    { \
        (pszDst) = ""; \
        if ((void*)(pszSrc)) \
        { \
            int size = strlen((pszSrc)); \
            if (size > 0) \
            { \
                pszDst = platform_mb2u8((soapEnv), pszSrc); \
                CHECK_MALLOC((pszDst), (pszFault), (rtn)); \
            } \
        } \
    }

//
// Summary:
//     申请空间，并拷贝 std_type 的 value 。
//
#define CHECK_COPY_STV(std_type_dst_pointer, std_type_size, std_type_src, pszFault, rtn) \
    soap_malloc((soapEnv), (std_type_size)); \
    CHECK_MALLOC((std_type_dst_pointer), (pszFault), (rtn)); \
    memcpy((std_type_dst_pointer), &(std_type_src), (std_type_size))

//
// Summary:
//     申请 n 个连续空间，并置 0 。
//
#define CHECK_AND_ZERO_MALLOC_N(dst_pointer, dst_type_size, n, pszFault, rtn) \
    soap_malloc((soapEnv), (dst_type_size) * (n)); \
    CHECK_MALLOC((dst_pointer), (pszFault), (rtn)); \
    ZeroMemory((dst_pointer), (dst_type_size) * (n))

//
// Summary:
//     申请空间，并置 0 。
#define CHECK_AND_ZERO_MALLOC(dst_pointer, dst_type_size, pszFault, rtn) \
    CHECK_AND_ZERO_MALLOC_N(dst_pointer, dst_type_size, 1, pszFault, rtn)

