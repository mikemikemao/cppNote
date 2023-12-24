#include"application.h"
#include<hpr/HPR_Guard.h>
#include<hpr/HPR_Thread.h>
#include<hpr/HPR_Utils.h>
#include<hlog/logdef.h>

#include"db_manager.h"
#include"config.h"
#include"addr_book_mgr.h"
#include"call_manager.h"
#include"event_upload.h"
#include"plat_sdk.h"
#include"sip_web_service.h"
#include"service_mgr.h"
#include"register_persistence.h"
#include"net_manager.h"
#include"upgrade_ext.hpp"

sip_application* sip_application::_instance = NULL;

sip_application::sip_application()
: _start(HPR_FALSE)
, _exit(HPR_FALSE)
, _restart_thrd(HPR_FALSE)
, _restarting(HPR_FALSE)
{
}

sip_application::~sip_application()
{
    try
    {
        //保证重启线程退出
        //可能存在，webservice创建重启线程后，hservice先调用了finit，此时
        //重启线程会继续执行,但不会启动服务
        HPR_BOOL is_restarting = HPR_TRUE;
        while (is_restarting == HPR_TRUE)
        {
            {
                HPR_Guard lock_app(&_app_mutex);
                if (_restarting == HPR_FALSE)
                {
                    is_restarting = HPR_FALSE;
                }
            }

            if (is_restarting == HPR_TRUE)
            {
                HPR_Sleep(100);
            }
        }
    }
    catch(...)
    {
    }
}

HPR_INT32 sip_application::init()
{
    SIP_SVR_INFO("start init...");

    HPR_INT32 ret = -1;

    HPR_Guard lock_app(&_app_mutex);
    do
    {
        //创建各模块对象
        if (db_manager_t::create_instance() != 0)
        {
            SIP_SVR_ERROR("create db mgr faild!");
            break;
        }

        if (config_t::create_instance() != 0)
        {
            SIP_SVR_ERROR("create config faild!");
            break;
        }

        //由于创建后面的对象需要一些配置信息，从数据库读取
        //因此，数据库和配置对象需要先进行初始化
        if(db_manager_t::instance()->init(sql_def_t::DB_SQLITE) != 0)
        {
            SIP_SVR_ERROR("init db mgr faild!");
            break;
        }

        if (config_t::instance()->init() != 0)
        {
            SIP_SVR_ERROR("init config faild!");
            break;
        }

        if (plat_sdk_t::create_instance() != 0)
        {
            SIP_SVR_ERROR("create plat sdk faild!");
            break;
        }

        if (register_persistence_t::creat_instance() != 0)
        {
            SIP_SVR_ERROR("create register persistence faild!");
            break;
        }

        if (addr_book_mgr_t::create_instance() != 0)
        {
            SIP_SVR_ERROR("create address book mgr faild!");
            break;
        }

        if (event_upload_t::create_instance() != 0)
        {
            SIP_SVR_ERROR("create event upload faild!");
            break;
        }

        if (call_manager_t::create_instance() != 0)
        {
            SIP_SVR_ERROR("create call mgr faild!");
            break;
        }

		if (NULL == CUpgradeSrv::GetInstance())
        {
            SIP_SVR_ERROR("create call CUpgradeSrv faild!");
            break;
        }

        if (sip_web_service_t::create_instance() != 0)
        {
            SIP_SVR_ERROR("create webservice faild!");
            break;
        }

        if (net_manager_t::instance(config_t::instance()->get_net_manager_device_port()) == NULL)
        {
            SIP_SVR_ERROR("create net manager failed.");
            break;
        }

        ret = 0;

    }while(0);

    if (ret != 0)
    {
        finit();
        SIP_SVR_ERROR("init sip server faild!");
    }
    else
    {
        SIP_SVR_INFO("init sip server success!");
    }

    return ret;
}

HPR_INT32 sip_application::finit()
{
    SIP_SVR_INFO("start finit ...");

    HPR_Guard lock_app(&_app_mutex);
    
    if (_start == HPR_TRUE)
    {
        stop_app();
    }

    //删除各对象
    sip_web_service_t::destroy_instance();
    call_manager_t::destroy_instance();
    event_upload_t::delete_instance();
	CUpgradeSrv::GetInstance()->UnInit();
    addr_book_mgr_t::destroy_instance();
    register_persistence_t::del_instance();
    plat_sdk_t::del_instance();
	//配置和数据库模块需要先进行反初始化
	config_t::instance()->finit();
	db_manager_t::instance()->finit();

    config_t::del_instance();
    db_manager_t::destroy_instance();
    service_mgr_t::del_instance();
    net_manager_t::del_instance();

    SIP_SVR_INFO("finit sucess"); 

    return 0;
}

HPR_INT32 sip_application::start_app()
{
	SIP_SVR_INFO("start sip server ...");
    HPR_INT32 ret = -1;

    HPR_Guard lock_app(&_app_mutex);
    do 
    {
        if (_start == HPR_TRUE)
        {
            ret = 0;
            break;
        }

        //if(db_manager_t::instance()->init(sql_def_t::DB_SQLITE) != 0)
        //{
        //    SIP_SVR_ERROR("init db mgr faild!");
        //    break;
        //}

        //if (config_t::instance()->init() != 0)
        //{
        //    SIP_SVR_ERROR("init config faild!");
        //    break;
        //}

        if(plat_sdk_t::instance()->init() != 0)
        {
            SIP_SVR_ERROR("init plat sdk faild!");
            break;
        }

        if (register_persistence_t::instance()->init() != 0)
        {
            SIP_SVR_ERROR("init register persistence faild!");
            break;
        }

        if (addr_book_mgr_t::instance()->init() != 0)
        {
            SIP_SVR_ERROR("init addressbook mgr faild!");
            break;
        }

        if (event_upload_t::instance()->init() != 0)
        {
            SIP_SVR_ERROR("init event upload obj faild!");
            break;
        }

        if (call_manager_t::instance()->init() != 0)
        {
            SIP_SVR_ERROR("init call mgr faild!");
            break;
        }
		if (CUpgradeSrv::GetInstance()->Init() != 0)
        {
            SIP_SVR_ERROR("init call CUpgradeSrv faild!");
            break;
        }

        if (sip_web_service_t::instance()->start_server(config_t::instance()->get_webservice_port()) != 0)
        {
            SIP_SVR_ERROR("start web service server faild!");
            break;
        }

        //初始化服务器管理类，创建所有服务
        if (HPR_ERROR == service_mgr_t::instance()->init() )
        {
            SIP_SVR_ERROR("start services failed!");
            break;
        }

        //加载通信录
        if (addr_book_mgr_t::instance()->load_addressbook() != 0)
        {
            SIP_SVR_ERROR("load address book faild!");
            break;
        }

        //加载网管
        if (net_manager_t::instance()->init() != 0)
        {
            SIP_SVR_ERROR("init net manage failed.");
            break;
        }

        ret = 0;
        _start = HPR_TRUE;

    } while (0);

    if (ret != 0)
    {
        service_mgr_t::instance()->fini();
        sip_web_service_t::instance()->stop_server();
        call_manager_t::instance()->finit();
        event_upload_t::instance()->finit();
        addr_book_mgr_t::instance()->finit();
        register_persistence_t::instance()->finit();
        plat_sdk_t::instance()->finit();
        net_manager_t::instance()->fini();
        //config_t::instance()->finit();
        //db_manager_t::instance()->finit();

        SIP_SVR_ERROR("start sip server faild!");
    }
    else
    {
        SIP_SVR_INFO("start sip server sucess!");
    }

    return ret;
}

HPR_INT32 sip_application::stop_app()
{
    HPR_Guard lock_app(&_app_mutex);
    if (_start == HPR_FALSE)
    {
        return 0;
    }

    SIP_SVR_INFO("stop sip server ...");
    net_manager_t::instance()->fini();
    service_mgr_t::instance()->fini();
    service_mgr_t::del_instance();
    sip_web_service_t::instance()->stop_server();
    call_manager_t::instance()->finit();
    event_upload_t::instance()->finit();
    addr_book_mgr_t::instance()->finit();
    register_persistence_t::instance()->finit();
    plat_sdk_t::instance()->finit();
    //config_t::instance()->finit();
    //db_manager_t::instance()->finit();

    _start = HPR_FALSE;

    SIP_SVR_INFO("stop sip server sucess");

    return 0;
}

HPR_INT32 sip_application::restart()
{
    HPR_INT32 ret = -1;
    HPR_Guard lock_app(&_restart_mutex);

    do
    {

        //若设置了退出标志，直接退出
        if (_exit == HPR_TRUE)
        {
            return 0;
        }

        stop_app();
        finit();

        if(init() != 0)
        {
            break;
        }
        if(start_app() != 0)
        {
            break;
        }

        ret = 0;

    }while(0);

    if (ret != 0)
    {
        stop_app();
        finit();
    }

    //设置标志，表明重启线程结束
    _restarting = HPR_FALSE;

    return ret;
}

HPR_INT32 sip_application::startup_restart()
{
    //对重启过程加锁
	if (HPR_TRUE == _restarting)
	{
		SIP_SVR_ERROR("Restart already!\n");
		return 0;
	}
    HPR_Guard lock_app(&_restart_mutex);
   
    _restart_thrd = HPR_ThreadDetached_Create(restart_thread, this, 0);
    if (_restart_thrd == HPR_FALSE)
    {
        SIP_SVR_ERROR("startup restart thread faild!");
        _restarting = HPR_FALSE;

        return -1;
    }

    _restarting = HPR_TRUE;

    return 0;
}

HPR_VOIDPTR CALLBACK sip_application::restart_thread(HPR_VOIDPTR thread_param)
{
    sip_application* p_app = static_cast<sip_application*>(thread_param);

    p_app->restart();

    return NULL;
}

HPR_INT32 sip_application::set_exit()
{
    HPR_Guard lock_app(&_app_mutex);
    _exit = HPR_TRUE;

    return 0;
}

HPR_INT32 sip_application::create_instance()
{
    if (_instance == NULL)
    {
        _instance = new(std::nothrow)sip_application();
    }

    return (_instance == NULL ? -1 : 0);
}

HPR_VOID sip_application::del_instance()
{
    if (_instance != NULL)
    {
        delete _instance;
        _instance = NULL;
    }

    return;
}

sip_application* sip_application::instance()
{
    return _instance;
}
