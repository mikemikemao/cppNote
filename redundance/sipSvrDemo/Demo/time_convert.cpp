/**
 * @file time_convert.cpp
 * @brief 时间格式转换
 * @author zhanghaifengyf1@hikvision.com.cn
 * @version 0.0.1
 * @date 2014-08-06 11:11:52
 */

#include <time.h>

#include<hpr/HPR_String.h>
#include<hpr/HPR_Utils.h>

#include"time_convert.h"

time_t time_sip2timet(const std::string& t)
{
    //2014-08-06T14:38:00
    std::string str_time = std::string(t);

    HPR_UINT32 year = HPR_Atoi32(str_time.substr(0,4).c_str()); 
    HPR_UINT32 month = HPR_Atoi32(str_time.substr(5,2).c_str());
    HPR_UINT32 day = HPR_Atoi32(str_time.substr(8,2).c_str());
    HPR_UINT32 hour = HPR_Atoi32(str_time.substr(11,2).c_str());
    HPR_UINT32 min = HPR_Atoi32(str_time.substr(14,2).c_str()); 
    HPR_UINT32 sec = HPR_Atoi32(str_time.substr(17,2).c_str());

    struct tm atm;
    atm.tm_sec = sec;
    atm.tm_min = min;
    atm.tm_hour = hour;
    atm.tm_mday = day;
    atm.tm_mon = month - 1;        // tm_mon is 0 based
    atm.tm_year = year - 1900;     // tm_year is 1900 based
    atm.tm_isdst = -1;

    return mktime(&atm);

}

HPR_VOID time_timet2sip(time_t timet, std::string& timestr)
{
    //2014-08-06T14:38:00
    if (timet <= 0)
    {
        return;
    }
    char t[128] = {0};
    HPR_Snprintf(t, 127, "%04d-%02d-%02dT%02d:%02d:%02d", 
        localtime(&timet)->tm_year + 1900, 
        localtime(&timet)->tm_mon + 1, 
        localtime(&timet)->tm_mday,
        localtime(&timet)->tm_hour, 
        localtime(&timet)->tm_min,
        localtime(&timet)->tm_sec);
    timestr = t;

    return;
}

std::string get_now_time_string()
{
    time_t timet = time(0);
    char timestr[128] = {0};
    HPR_Snprintf(timestr, 127, "%04d-%02d-%02d %02d:%02d:%02d", 
        localtime(&timet)->tm_year + 1900, 
        localtime(&timet)->tm_mon + 1, 
        localtime(&timet)->tm_mday,
        localtime(&timet)->tm_hour, 
        localtime(&timet)->tm_min,
        localtime(&timet)->tm_sec);

    return std::string(timestr);
}
