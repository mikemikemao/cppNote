#pragma once
/**************************************************************************	
 *  @file cmsws_defines.h
 *	@note HangZhou Hikvision System Technology Co., Ltd. All Right Reserved.
 *	@brief ������д���ļ���ժҪ��
 *
 *	@author		Wang Zhaojun
 *	@date		2013/01/05
 *
 *	@note ��ʷ��¼��
 *	@note V1.0.0  ����
 *
 *	@warning �������ݽṹ���壬������������ģ������������ɣ�����ֱ������ͷ�ļ�
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
    //     CMS WebService API ���Ͷ���
    //
    #define CMSWS_API __stdcall

    //
    // Summary:
    //     TLS ������
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
    //     CMS WebService API ���Ͷ���
    //
    #define CMSWS_API 

    //
    // Summary:
    //     TLS ������
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
//     cmsws ����ֵ���塣
//
#define CMSWS_SUCCESS   1   // �����ɹ�
#define CMSWS_FAILURE   0   // ����ʧ��


//
// Summary:
//     cmsws service stub �����塣
//
#define CMSWS_SOAP_PARAM_ERROR          0x01000001      // ���������Ч
#define CMSWS_SOAP_MALLOC_ERROR         0x01000002      // �����ڴ����
#define CMSWS_SOAP_RESPONSE_NIL         0x01000003      // �������ظ��� XML Ϊ��
#define CMSWS_SOAP_ERROR_CODE_NIL       0x01000004      // �������ظ��� errorCode Ϊ��

//
// Summary:
//     cmsws service skel �����塣
//
#define CMSWS_SOAP_SKEL_NO_SERVICE_ENTRY    0x02000001  // ��������ڲ�����


//
// Summary:
//     ���� CMS WebService ���ص� errorCode ����
//
#define CMS_SUCCESSED                   0                //�����ɹ�
#define CMS_FAILED                      10000            //����ʧ��

//�û���¼������
#define CMS_USERNAME_ERROR              10001            //�û���������
#define CMS_PWD_ERROR                   10002            //�������
#define CMS_USER_HAS_LOGIN              10003            //�û��Ѿ���¼
#define CMS_USER_NOT_EXCLUSIVE          10004            //�û����Ʋ�Ψһ
#define CMS_USER_EXPIRED                10005            //�û��Ѿ�����
#define CMS_USER_DELETE                 10006            //�û���ɾ��
#define CMS_MAC_WRONG					10007			//�û���MAC��ַ����
#define CMS_USER_NOT_ACTIVE		        10008			//�û�δ������
#define CMS_USER_NORIGHT				10009				//�û�û��Ȩ��
#define CMS_USERSESSION_NOT_EXIST       10010            //�û��ỰID������
#define CMS_USER_RIGHT_EXPIRED		    10011			//�û�Ȩ���ѹ���		[guchen 20120221]

//��Ӵ�����
#define CMS_INDEX_CODE_HAS_EXIST        20001            //���������Ѿ�����
#define CMS_PARENT_ID_ERROR             20002            //��ID����
#define CMS_ID_NOT_EXIST                20003            //IDֵ������
#define CMS_USER_NAME_EXIST             20004            //�û����Ѿ�����
#define CMS_INTERNAL_ERROR              20005            //ϵͳ�ڲ�����

//���ݿ��ѯ������
#define CMS_SELECT_NONE                 30001            //��ѯ��������

//License��ش��� 20121009
#define CMS_NO_LICENSE 40001 // ϵͳû��license
#define CMS_LICENSE_EXPIRED 40002 // license�ѹ���
#define CMS_DOG_EXPIRED 40003 // ���ܹ�����������	

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
//     ��� runtime environment ��
//
#define GET_SOAP_ENV(rtn) \
    cmsws_soap* soapEnv = SoapWrapper::GetSoapEnv(); \
    if (!soapEnv) \
    { \
        return (rtn); \
    }

//
// Summary:
//     ����ռ䣬������ (char*)psz ���ݡ�
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
//     �������Ƿ�������������������ô�����Ϣ�󷵻ء�
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
//     ΪӦ�� gSoap �Կյ�ָ���������л�Ϊ����һ����Ԫ�ص����飬�ʶԴ��������������ĳ��ȡ�
//
#define CHECK_ARRAY_SIZE(pointer_array) \
    if (!(pointer_array) || ((pointer_array ## Length) == 1 && !(*(pointer_array)))) \
    { \
        (pointer_array ## Length) = 0; \
    }

//
// Summary:
//     ������������
//
#define CHECK_PARAM(param, condition, pszFault, rtn) \
    CHECK_PARAM_AND_SET_ERROR_AND_RETURN(param, condition, CMSWS_SOAP_PARAM_ERROR, pszFault, NULL, rtn)

//
// Summary:
//     ����ڴ���䡣
//
#define CHECK_MALLOC(param, pszFault, rtn) \
    CHECK_PARAM_AND_SET_ERROR_AND_RETURN(param, NULL, CMSWS_SOAP_MALLOC_ERROR, pszFault, NULL, rtn)

//
// Summary:
//     ��� axis2 webservice response ������
//
#define CHECK_RESPONSE(response, rtn) \
    CHECK_PARAM_AND_SET_ERROR_AND_RETURN(response->return_, NULL, CMSWS_SOAP_RESPONSE_NIL, NULL, NULL, rtn)

//
// Summary:
//     ��� NVR Web Service ��ڡ�
//
#define CHECK_SERVICE_ENTRY(entry, pszFault, rtn) \
    CHECK_PARAM_AND_SET_ERROR_AND_RETURN(entry, NULL, CMSWS_SOAP_SKEL_NO_SERVICE_ENTRY, pszFault, NULL, rtn)


//
// Summary:
//     ��ȡ service endpoint uri ��
//
#define GET_ENDPOINT_URI(newUri) \
    ((newUri) ? (newUri) : SoapWrapper::GetEndpointUri())


//
// Summary:
//     ��ȡ string* Ϊ const char* ��
//
// Remarks:
//     ����ʹ�� STL string ������ char* ��
//
//     ��Ҫ�Դ��ݵ��ַ������� UTF-8 �� MB ת�롣
//
// #define GET_C_STR(string_pointer) ((string_pointer) ? (string_pointer)->c_str() : 0)
// #define GET_C_STR(string_pointer) ((const char*)(string_pointer))
#define GET_C_STR(string_pointer) (platform_u82mb((soapEnv), (const char*)(string_pointer)))

//
// Summary:
//     ��ȡ *(std_type_pointer) �� value ��
//
#define GET_STV_DEFAULT(std_type_pointer, default_value) \
    ((std_type_pointer) ? *(std_type_pointer) : (default_value))


//
// Summary:
//     ����ռ䣬������ (char*)psz ���ݡ�
//
// Remarks:
//     ��Ҫ�Ի�ȡ���ַ������� MB �� UTF-8 ת�롣
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
//     ����ռ䣬������ std_type �� value ��
//
#define CHECK_COPY_STV(std_type_dst_pointer, std_type_size, std_type_src, pszFault, rtn) \
    soap_malloc((soapEnv), (std_type_size)); \
    CHECK_MALLOC((std_type_dst_pointer), (pszFault), (rtn)); \
    memcpy((std_type_dst_pointer), &(std_type_src), (std_type_size))

//
// Summary:
//     ���� n �������ռ䣬���� 0 ��
//
#define CHECK_AND_ZERO_MALLOC_N(dst_pointer, dst_type_size, n, pszFault, rtn) \
    soap_malloc((soapEnv), (dst_type_size) * (n)); \
    CHECK_MALLOC((dst_pointer), (pszFault), (rtn)); \
    ZeroMemory((dst_pointer), (dst_type_size) * (n))

//
// Summary:
//     ����ռ䣬���� 0 ��
#define CHECK_AND_ZERO_MALLOC(dst_pointer, dst_type_size, pszFault, rtn) \
    CHECK_AND_ZERO_MALLOC_N(dst_pointer, dst_type_size, 1, pszFault, rtn)

