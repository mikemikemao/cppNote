/**
 * @file time_convert.h
 * @brief 时间格式转换
 * @author zhanghaifengyf1@hikvision.com.cn
 * @version 0.0.1
 * @date 2014-08-06 11:10:24
 */

#ifndef TIME_CONVERT_WV6RNZ67
#define TIME_CONVERT_WV6RNZ67

#include<string>

time_t      time_sip2timet(const std::string& t);
HPR_VOID    time_timet2sip(time_t timet, std::string& timestr);

std::string get_now_time_string();

#endif /* end of include guard: TIME_CONVERT_WV6RNZ67 */

