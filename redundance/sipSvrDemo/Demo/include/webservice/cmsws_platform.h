#pragma once
//
// Copyright (c) 2009 Hikvision Digital Technology Co., Ltd.
//
// Summary:
//     �ṩһЩʵ�õĿ�ƽ̨�������������� Windows �汾��
//
// Modification History:
//   2009-10-28:
//     V1.0 / ��ͨ�� / ����
//
#include "cmsws_defines.h"

//
// Summary:
//     utf8 -> mb
//
char* platform_u82mb(cmsws_soap* soapEnv, const char* cont);

//
// Summary:
//     mb -> utf8
//
char* platform_mb2u8(cmsws_soap* soapEnv, const char* cont);

//
// Summary:
//     utf8 -> widechar
//
wchar_t* platform_u82wc(cmsws_soap* soapEnv, const char* cont);