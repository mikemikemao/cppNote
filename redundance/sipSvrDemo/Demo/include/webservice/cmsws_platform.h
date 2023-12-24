#pragma once
//
// Copyright (c) 2009 Hikvision Digital Technology Co., Ltd.
//
// Summary:
//     提供一些实用的跨平台函数，但仅定义 Windows 版本。
//
// Modification History:
//   2009-10-28:
//     V1.0 / 胡通海 / 创建
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