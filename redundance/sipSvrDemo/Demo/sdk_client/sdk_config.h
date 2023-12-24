/*
 * =====================================================================================
 *
 *       Filename:  sdk_config.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2016-08-25 15:04:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yingc, yinguicai@hikvision.com
 *   Organization:  HIKVISION
 *
 * =====================================================================================
 */

#ifndef __SDK_CONFIG_H__
#define __SDK_CONFIG_H__
#if 0
#if !defined(__BEGIN_DECLS) || !defined(__END_DECLS)
# if defined(__cplusplus)
#  define __BEGIN_DECLS extern "C" {
#  define __END_DECLS   }
# else
#  define __BEGIN_DECLS
#  define __END_DECLS
# endif
#endif

#else

#if !defined(__BEGIN_DECLS) || !defined(__END_DECLS)
# if defined(__cplusplus)
#  define __BEGIN_DECLS
#  define __END_DECLS
# else
#  define __BEGIN_DECLS
#  define __END_DECLS
# endif
#endif

#endif


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#endif /* __SDK_CONFIG_H__ */

