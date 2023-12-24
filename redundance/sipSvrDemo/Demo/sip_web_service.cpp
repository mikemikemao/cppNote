#include"sip_web_service.h"
#include"sip_xml.h"
#include<webservice/gsoap_server.h>
#include<webservice/sipws_service.h>
#include"base64.h"
#include<hlog/logdef.h>
#include<hpr/HPR_Thread.h>
#include"addr_book_mgr.h"
#include"db_manager.h"
#include"application.h"
#if (defined(__linux__) || defined(__APPLE__) || defined(__SYMBIAN32__) || defined(ANDROID))
#include<time.h>
#endif

//extern sip_application app;

sip_web_service_t* sip_web_service_t::_instance = NULL;

sip_web_service_t::sip_web_service_t()
{

}

sip_web_service_t::~sip_web_service_t()
{

}

HPR_INT32 sip_web_service_t::create_instance()
{
    if (_instance == NULL)
    {
        _instance = new(std::nothrow)sip_web_service_t();
    }

    return (_instance == NULL ? -1 : 0);
}

HPR_VOID sip_web_service_t::destroy_instance()
{
    if (_instance != NULL)
    {
        delete _instance;
        _instance = NULL;
    }

    return;
}

HPR_INT32 sip_web_service_t::start_server(HPR_UINT16 port)
{
    //启动监听服务
    HPR_INT32 ret = -1;
    do
    {
        //初始化服务
        if (CMSWS_InitServer() != CMSWS_SUCCESS)
        {
            SIP_SVR_ERROR("Init webservice server faild!");
            break;
        }

        //设置启动参数
        ServerStartParam ws_start_param;
        ws_start_param.ip = 0;
        ws_start_param.port = config_t::instance()->get_webservice_port();
        ws_start_param.backlog = 10;   //socket通讯等待队列
        ws_start_param.sendTimeout = 60;
        ws_start_param.recvTimeout = 60;
        ws_start_param.maxResponseThread = 2;
        ws_start_param.maxRequestQueue = 10;
        ws_start_param.maxKeepAlive = 100;

        //启动监听服务
        if (CMSWS_StartServer(&ws_start_param) != CMSWS_SUCCESS)
        {
            SIP_SVR_ERROR("Start webservice server faild!");
            break;
        }

        //设置服务回调接口
        sipws_services_entry_t sip_service_entry;
        sip_service_entry.AddDevice = add_device;
        sip_service_entry.RemoteCfg = remote_config;
        sip_service_entry.ClrDevice = clear_device;
        sip_service_entry.ModifyPsw = modify_psw;
        sip_service_entry.SyncTime = sync_time;

        SIPWS_SetServiceEntry(&sip_service_entry);

        //启动服务
        if (SIPWS_StartService(config_t::instance()->get_sipws_server_uri()) != CMSWS_SUCCESS)
        {
            SIP_SVR_ERROR("start sip ws %s faild!", config_t::instance()->get_sipws_server_uri());
            break;
        }

        ret = 0;

    }while(0);

    if (ret != 0)
    {
        stop_server();
    }

    return ret;
}

HPR_VOID sip_web_service_t::stop_server()
{
    SIP_SVR_INFO("begin sip web service stop!");
    SIPWS_StopService();
    CMSWS_StopServer();
    CMSWS_UninitServer();

    SIP_SVR_INFO("sip web service stopped!");
    return;
}

HPR_INT32 CALLBACK sip_web_service_t::add_device(const char* user_name, const char* password, const char* content)
{
    VT_PHONE_LIST phone_add;
    VT_PHONE_LIST phone_update;
    std::list<std::string> phone_del;

    //对用户名，密码进行校验
    if (check_user_psw_base64(user_name, password) != 0)
    {
        SIP_SVR_ERROR("User or psw error!");
        return -1;
    }

    if (sip_xml::parse_addrbook_add(content, phone_add, phone_update, phone_del) != 0)
    {
        SIP_SVR_ERROR("Parse xml faild!xml:%s", content);
        return -1;
    }

    if(phone_add.size() > 0)
    {
        if (addr_book_mgr_t::instance()->add_phone_batch(phone_add) != 0)
        {
            SIP_SVR_ERROR("Add phone to mem faild!");
            return -1;
        }
        else
        {
            return db_manager_t::instance()->add_phone_sync(phone_add);
        }
    }
    else if (phone_update.size() > 0)
    {
        if (addr_book_mgr_t::instance()->update_phone_batch(phone_update) != 0)
        {
            SIP_SVR_ERROR("Update phone to mem faid!");
            return -1;
        }
        else
        {
            return db_manager_t::instance()->update_phone_sync(phone_update);
        }
    }
    else if(phone_del.size() > 0)
    {
        if (addr_book_mgr_t::instance()->delete_phone_batch(phone_del) != 0)
        {
            SIP_SVR_ERROR("Delete phone to mem faild!");
            return -1;
        }
        else
        {
            return db_manager_t::instance()->del_phone_sync(phone_del);
        }
    }

    return 0;
}

HPR_INT32 CALLBACK sip_web_service_t::remote_config(const char* user_name, const char* password, const char* content)
{
    if (check_user_psw_base64(user_name, password) != 0)
    {
        SIP_SVR_ERROR("User or password error!");
        return -2;
    }

    remote_config_t config;
    if (sip_xml::parse_remote_config(content, config) != 0)
    {
        SIP_SVR_ERROR("Parse xml faild!xml:%s", content);
        return -1;
    }

    //更新配置信息至数据库
    if (db_manager_t::instance()->set_remote_config(config) != 0)
    {
        SIP_SVR_ERROR("write config to db faild!");
        return -3;
    }

    //重启服务器
    sip_application::instance()->startup_restart();

    SIP_SVR_INFO("remote config sucess!");

    return 0;
}

HPR_INT32 CALLBACK sip_web_service_t::clear_device(const char* user_name, const char* password, const char* content)
{
    if (check_user_psw_base64(user_name, password) != 0)
    {
        SIP_SVR_ERROR("User or password error!");
        return -1;
    }

    //内存中的数据先清除
    if(addr_book_mgr_t::instance()->clear_phone() != 0)
    {
        SIP_SVR_ERROR("clear phone mem faild!");
        return -1;
    }

    //清除数据库中的数据
    if(db_manager_t::instance()->trancate_addrbook() != 0)
    {
        SIP_SVR_ERROR("clear phone db faild!");
        return -1;
    }

    SIP_SVR_INFO("clear address book sucess!");
    return 0;
}

HPR_INT32 CALLBACK sip_web_service_t::modify_psw(const char* user_name, const char* old_psw, const char* new_psw)
{
    SIP_SVR_INFO("modify password!");

    if (check_user_psw_base64(user_name, old_psw) != 0)
    {
        SIP_SVR_ERROR("User or password error!");
        return -1;
    }

    return db_manager_t::instance()->modify_psw(new_psw);
}

HPR_INT32 sip_web_service_t::check_user_psw_base64(const char* user_name, const char* password)
{
    //将本地用户名加密
    char* base64_data = NULL;
    HPR_INT32 ret = -1;

    do
    {
        //校验用户名
        std::string local_user = config_t::instance()->get_user_name();
		if (strcmp(user_name, local_user.c_str()) != 0)
		{
			break;
		}
#if 0
        HPR_INT32 encode_len = base64_encode(local_user.c_str(), local_user.size(), &base64_data);

        if (base64_data == NULL)
        {
            break;
        }

        if (strcmp(base64_data, user_name) != 0)
        {
            break;
        }

        delete[] base64_data;
        base64_data = NULL;
#endif
        //校验密码
        local_user = config_t::instance()->get_password();
		if (strcmp(password, local_user.c_str()) != 0)
		{
			break;
		}
#if 0
        encode_len = base64_encode(local_user.c_str(), local_user.size(), &base64_data);

        if (base64_data == NULL)
        {
            break;
        }

        if (strcmp(base64_data, password) != 0)
        {
            break;
        }

        delete[] base64_data;
        base64_data = NULL;
#endif
        ret = 0;

    }while(0);

    if (ret != 0)
    {
        if (base64_data != NULL)
        {
            delete base64_data;
        }
    }

    return ret;
}

HPR_INT32 CALLBACK sip_web_service_t::sync_time( time_t t )
{
#if (defined(_WIN32) || defined(_WIN32_WCE) || defined(_WIN64))
    struct tm& local_time = *(localtime(&t));
    SYSTEMTIME local_st;
    memset(&local_st, 0, sizeof(SYSTEMTIME));
    local_st.wYear = (WORD)(local_time.tm_year + 1900);
    local_st.wMonth = (WORD)(local_time.tm_mon + 1);
    local_st.wDay = (WORD)(local_time.tm_mday);
    local_st.wHour = (WORD)(local_time.tm_hour);
    local_st.wMinute = (WORD)(local_time.tm_min);
    local_st.wSecond = (WORD)(local_time.tm_sec);
    if( SetLocalTime(&local_st) )
    {
        SIP_SVR_INFO("Set Time:%04d-%02d-%02d %02d:%02d:%02d", local_st.wYear, local_st.wMonth, local_st.wDay, local_st.wHour, local_st.wMinute, local_st.wSecond);
        return 0;
    }
    else
    {
        SIP_SVR_ERROR("Sync time failed, err:%d", GetLastError());
        return -1;
    }
#elif (defined(__linux__) || defined(__APPLE__) || defined(__SYMBIAN32__) || defined(ANDROID))
    if (-1 == stime(t) )
    {
        SIP_SVR_ERROR("Sync time failed.");
        return -1;
    }
    return 0;
#else
    #error OS Not Implement Yet.
#endif	// end #if (defined(WIN32) || defined(_WIN32_WCE))
}

