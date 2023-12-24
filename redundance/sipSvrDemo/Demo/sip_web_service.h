/** @file:   sip_web_service.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  webservice服务
 *
 *  @author: wangfeida@hikvision.com.cn
 *  @data:   2014/08/01
 *  
 *  @note:   
 *
 *  @Modification History:
 *  <version>       <time>          <author>        <desc>
 *    V1.0        2014/08/01        wangfeida       Create
 *
 *  @warning: <No waining>        
*/

#ifndef _SIP_WEB_SERVICE_H_
#define _SIP_WEB_SERVICE_H_

#include<hpr/HPR_Hpr.h>
#include<list>

class sip_web_service_t
{
private:

    sip_web_service_t();
    virtual ~sip_web_service_t();

public:

    static HPR_INT32 create_instance();
    static HPR_VOID destroy_instance();

    inline static sip_web_service_t* instance()
    {
        return _instance;
    }

    //启动服务
    HPR_INT32 start_server(HPR_UINT16 port);
    //停止服务
    HPR_VOID stop_server();

private:

    static HPR_INT32 CALLBACK add_device(const char* user_name, const char* password, const char* content);
    static HPR_INT32 CALLBACK remote_config(const char* user_name, const char* password, const char* content);
    static HPR_INT32 CALLBACK clear_device(const char* user_name, const char* password, const char* content);
    static HPR_INT32 CALLBACK modify_psw(const char* user_name, const char* old_psw, const char* new_psw);
    static HPR_INT32 CALLBACK sync_time(time_t t);

    static HPR_INT32 check_user_psw_base64(const char* user_name, const char* password);
    
private:

    static sip_web_service_t* _instance;

};

#endif
