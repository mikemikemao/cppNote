#ifndef _LOG_DEF_H_
#define _LOG_DEF_H_


#include "hlog/hlog.h"

#define SIPSVR_LOG_MODULE    "SIP_SVR"

/* 定义日志输出宏函数 */
#if (defined (_WIN32))
#define SIP_SVR_ERROR(fmt, ...)         hlog_format(HLOG_LEVEL_ERROR, SIPSVR_LOG_MODULE, "[%s-%d] "fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define SIP_SVR_WARN(fmt, ...)          hlog_format(HLOG_LEVEL_WARN,  SIPSVR_LOG_MODULE, "[%s-%d] "fmt, __FILE__, __LINE__, ##__VA_ARGS__)    
#define SIP_SVR_INFO(fmt, ...)          hlog_format(HLOG_LEVEL_INFO,  SIPSVR_LOG_MODULE, "[%s-%d] "fmt, __FILE__, __LINE__, ##__VA_ARGS__)    
#define SIP_SVR_DEBUG(fmt, ...)         hlog_format(HLOG_LEVEL_DEBUG, SIPSVR_LOG_MODULE, "[%s-%d] "fmt, __FILE__, __LINE__, ##__VA_ARGS__)    
#define SIP_SVR_TRACE(fmt, ...)         hlog_format(HLOG_LEVEL_TRACE, SIPSVR_LOG_MODULE, "[%s-%d] "fmt, __FILE__, __LINE__, ##__VA_ARGS__)   
#else
#define SIP_SVR_ERROR(fmt, args...)     hlog_format(HLOG_LEVEL_ERROR, SIPSVR_LOG_MODULE, "[%s-%d] "fmt, __FILE__, __LINE__, ##args)
#define SIP_SVR_WARN(fmt, args...)      hlog_format(HLOG_LEVEL_WARN,  SIPSVR_LOG_MODULE, "[%s-%d] "fmt, __FILE__, __LINE__, ##args)
#define SIP_SVR_INFO(fmt, args...)      hlog_format(HLOG_LEVEL_INFO,  SIPSVR_LOG_MODULE, "[%s-%d] "fmt, __FILE__, __LINE__, ##args)
#define SIP_SVR_DEBUG(fmt, args...)     hlog_format(HLOG_LEVEL_DEBUG, SIPSVR_LOG_MODULE, "[%s-%d] "fmt, __FILE__, __LINE__, ##args)
#define SIP_SVR_TRACE(fmt, args...)     hlog_format(HLOG_LEVEL_TRACE, SIPSVR_LOG_MODULE, "[%s-%d] "fmt, __FILE__, __LINE__, ##args)
#endif


#endif
