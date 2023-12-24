#include<hpr/HPR_Time.h>
#include<hpr/HPR_Guard.h>
#include<hlog/logdef.h>
#include"call_manager.h"
#include"addr_book_mgr.h"
#include"event_upload.h"

call_manager_t* call_manager_t::_instance = NULL;

call_manager_t::call_manager_t()
: work_thread_t("call_manager_t", config_t::instance()->get_max_no_rsp_time())
{

}

call_manager_t::~call_manager_t()
{

}

HPR_INT32 call_manager_t::create_instance()
{
    if (_instance == NULL)
    {
        _instance = new(std::nothrow)call_manager_t();
    }

    return (_instance == NULL ? -1:0);
}

HPR_VOID call_manager_t::destroy_instance()
{
    if (_instance != NULL)
    {
        delete _instance;
        _instance = NULL;
    }

    return;
}

HPR_INT32 call_manager_t::init()
{
    return start();
}

HPR_INT32 call_manager_t::finit()
{
    SIP_SVR_INFO("start finit call manager ...");
    //停止任务线程
    stop();

    //设置所有通话为INVALID_CALL_STATUS，并上传(写入数据库)
    HPR_Guard lock(&_call_map_lock);
    call_map::iterator itor = _call_map.begin();

    while (itor != _call_map.end())
    {
        itor->second._call_status = call_status_service_stop;
        if (itor->second._start_time != 0)
        {
            itor->second._stop_time = HPR_TimeNow();
        }
        else
        {
            itor->second._start_time = itor->second._invite_time;
            itor->second._stop_time = itor->second._invite_time;
        }

        //上传通话记录
        event_upload_t::instance()->upload_record(itor->second);
        ++itor;
    }

    _call_map.clear();

    SIP_SVR_INFO("call manager stopped!");

    return 0;
}

//>>  初始化一次呼叫（设置呼叫状态，判断呼叫合法性）
//call_id    呼叫id
//calling_phone   主叫方号码
//called_phone    被叫方号码
//error_code 错误码
HPR_INT32 call_manager_t::init_call(const char* call_id, device_t* calling_phone, device_t* called_phone, HPR_INT32& error_code)
{
    //先设置错误码为0
    error_code = UAS_STATUS_200;

    SIP_SVR_INFO("call_id:%s, calling:%s(%d), called:%s(%d)", call_id, calling_phone->get_phone_number(), 
    calling_phone->get_dev_status(), called_phone->get_phone_number(), called_phone->get_dev_status());

    //主叫方或被叫方不在线，禁止通话
    if ((calling_phone->is_online_status() == HPR_FALSE) ||
        (called_phone->is_online_status() == HPR_FALSE))
    {
        SIP_SVR_ERROR("calling or called phone is not online, calling:%s, called:%s", calling_phone->get_phone_number(), 
            called_phone->get_phone_number());
        error_code = UAS_STATUS_404;
        return -1;
    }

    if ((calling_phone->is_manager_center() == HPR_TRUE) &&\
        (called_phone->is_manager_center() == HPR_FALSE))
    {
        //检测主叫方是否已经在呼叫列表中，若已经在呼叫列表中，则从呼叫列表中删除
        check_phone_in_call(calling_phone->get_phone_number(), call_id);
        //主叫方为管理中心，此时不管管理中心处于何种状态，都应设置主叫方为忙线状态(离线状态无法设置，但支持主叫)
        calling_phone->set_busy_status();
    }
    else if((called_phone->is_manager_center() == HPR_TRUE) &&\
        (calling_phone->is_manager_center() == HPR_FALSE))
    {
        //被叫方为管理中心，由于在获取管理中心时已对被叫方进行状态设置(最大限度保证呼叫成功)，此处不需要进行处理
    }
	else if((called_phone->is_manager_center() == HPR_TRUE) &&\
        (calling_phone->is_manager_center() == HPR_TRUE))
    {
         calling_phone->set_busy_status();
		 called_phone->set_busy_status();
    }
    else
    {
        //主叫方与被叫方都不是管理中心或都是管理中心，此时不需要记录通话日志，直接返回
        return 0;
    }

    call_record_t call_info;

    call_info._call_id = call_id;
    call_info._calling_phone = calling_phone->is_manager_center() ? calling_phone->get_phone_number() : calling_phone->get_call_number();
    call_info._called_phone = called_phone->is_manager_center() ? called_phone->get_phone_number() : called_phone->get_call_number();
	SIP_SVR_INFO("calling phone : %s, called phone : %s.\n",call_info._calling_phone.c_str(),call_info._called_phone.c_str());
    //设置初始状态为正在呼叫
    call_info._call_status = call_status_recv_call;
    call_info._start_time = 0;
    call_info._stop_time = 0;
    call_info._invite_time = HPR_TimeNow();
    call_info._trying_time = 0;

    //HPR_Debug("wangfeid_debug:%s: add call %s:%s-->%s!", __FUNCTION__, call_info._call_id.c_str(), call_info._calling_phone.c_str(),\
    //call_info._called_phone.c_str());

    HPR_Guard lock(&_call_map_lock);
    _call_map.insert(call_map::value_type(call_info._call_id, call_info));

    return 0;
}

HPR_INT32 call_manager_t::set_call_status(const char* call_id, dev_call_status call_status)
{
    call_record_t call_info;
    std::string str_call_id(call_id);

    {
        HPR_Guard lock(&_call_map_lock);
        call_map::iterator itor = _call_map.find(str_call_id);
        if (itor == _call_map.end())
        {
            SIP_SVR_ERROR("call id %s not find!", call_id);
            return -1;
        }

        call_info = itor->second;
        //呼叫失败时，表明呼叫状态已完成，需要从列表中删除
        _call_map.erase(itor);
    }

    call_info._call_status = call_status;
    if (call_info._start_time != 0)
    {
        call_info._stop_time = HPR_TimeNow();
    }
    else
    {
        call_info._start_time = call_info._invite_time;
        call_info._stop_time = call_info._invite_time;
    }

    //设置管理机状态为空闲
    device_t* device = NULL;
    addr_book_mgr_t::instance()->lock_address_book_r();

    //设置主叫方为空闲状态
    device = addr_book_mgr_t::instance()->get_address_book(call_info._called_phone.c_str());
   
    if (device != NULL)
    {
        device->set_idle_status();
    }
    //设置被叫方为空闲状态
    device = addr_book_mgr_t::instance()->get_address_book(call_info._calling_phone.c_str());
	
    if (device != NULL)
    {
        device->set_idle_status();
    }

    addr_book_mgr_t::instance()->unlock_address_book_r();
    //上传通话日志到CMS
    HPR_Debug("wangfeida_debug:%s: finit call %s! finit status:%d", __FUNCTION__, call_info._call_id.c_str(),\
        call_info._call_status);
    event_upload_t::instance()->upload_record(call_info);

    return 0;
}

HPR_INT32 call_manager_t::set_call_status( call_record_t& call_rcd, dev_call_status call_status )
{
    call_rcd._call_status = call_status;
    if (call_rcd._start_time != 0)
    {
        call_rcd._stop_time = HPR_TimeNow();
    }
    else
    {
        call_rcd._start_time = call_rcd._invite_time;
        call_rcd._stop_time = call_rcd._invite_time;
    }

    //设置管理机状态为空闲
    device_t* device = NULL;
    addr_book_mgr_t::instance()->lock_address_book_r();

    //设置主叫方为空闲状态
    device = addr_book_mgr_t::instance()->get_address_book(call_rcd._called_phone.c_str());
    if (device != NULL)
    {
        device->set_idle_status();
    }
    //设置被叫方为空闲状态
    device = addr_book_mgr_t::instance()->get_address_book(call_rcd._calling_phone.c_str());
    if (device != NULL)
    {
        device->set_idle_status();
    }

    addr_book_mgr_t::instance()->unlock_address_book_r();
    //上传通话日志到CMS
    HPR_Debug("wangfeida_debug:%s: finit call %s! finit status:%d", __FUNCTION__, call_rcd._call_id.c_str(),\
        call_rcd._call_status);
    event_upload_t::instance()->upload_record(call_rcd);

    return 0;
}

HPR_INT32 call_manager_t::check_phone_in_call(const char* phone_num, const char* call_id)
{
    HPR_Guard lock(&_call_map_lock);

    std::string str_call_id(call_id);
    call_map::iterator itor = _call_map.begin();

    while (itor != _call_map.end())
    {
        if ((itor->second._calling_phone == phone_num) ||
            (itor->second._called_phone == phone_num))
        {
            if (itor->first == str_call_id)
            {
                SIP_SVR_WARN("ManageCenter calling resend:%s", str_call_id.c_str());
                break;
            }
            //设置此次通话超时结束
            set_call_status(itor->second, call_status_max_talking);
            _call_map.erase(itor);

            break;
        }

        ++itor;
    }

    return 0;
}

HPR_VOID call_manager_t::work(HPR_VOID)
{
    //检测是否有超时的通话
    HPR_TIME_T t = HPR_TimeNow();

    HPR_Guard lock(&_call_map_lock);
    call_map::iterator itor = _call_map.begin();

    while (itor != _call_map.end())
    {
        if (itor->second._trying_time == 0)
        {
            if ( (t - itor->second._invite_time) >= (config_t::instance()->get_max_no_rsp_time()*HPR_US_PER_SEC) )
            {
                //未接超时，请求时间和请求返回时间差超时，删除通话记录
                SIP_SVR_ERROR("Invite time out, call-id:%s.", itor->first.c_str());
                set_call_status(itor->second, call_status_faild);
                _call_map.erase(itor++);
            }
            else
            {
                ++itor;
            }
        }
        else if (itor->second._stop_time == 0)
        {
            if ( (itor->second._start_time != 0) && ( (t - itor->second._start_time) >= (config_t::instance()->get_max_talking_time()*HPR_US_PER_SEC) ) )
            {
                //这种情况只有在通话双方同时异常即双方都没有发bye信令或者ACK丢失
                SIP_SVR_ERROR("Talking time out, call-id:%s.", itor->first.c_str());
                set_call_status(itor->second, call_status_max_talking);
                _call_map.erase(itor++);
            }
            else
            {
                ++itor;
            }
        }
        else
        {
            ++itor;
        }
    }

    return;
}

HPR_INT32 call_manager_t::get_called_phone_number( const char* call_id, std::string& phone_num )
{
    call_record_t call_info;
    std::string str_call_id(call_id);

    //管理机号码直接返回成功
    if (strcmp(phone_num.c_str(), config_t::instance()->get_mgr_phone_num()) != 0)
    {
        return HPR_OK;
    }

    {
        HPR_Guard lock(&_call_map_lock);
        call_map::iterator itor = _call_map.find(str_call_id);
        if (itor == _call_map.end())
        {
            SIP_SVR_ERROR("call id %s not find!", call_id);
            phone_num = "";
            return HPR_ERROR;
        }

        call_info = itor->second;
    }

    phone_num = call_info._called_phone;
    return HPR_OK;
}

HPR_INT32 call_manager_t::set_rcd_start_time_now( const char* call_id )
{
    std::string str_call_id(call_id);
    HPR_TIME_T t = HPR_TimeNow();

    {
        HPR_Guard lock(&_call_map_lock);
        call_map::iterator itor = _call_map.find(str_call_id);
        if (itor == _call_map.end())
        {
            SIP_SVR_ERROR("call id %s not find!", call_id);
            return HPR_ERROR;
        }

        itor->second._start_time = t;
    }

    return HPR_OK;
}

HPR_INT32 call_manager_t::set_rcd_stop_time_now( const char* call_id )
{
    std::string str_call_id(call_id);
    HPR_TIME_T t = HPR_TimeNow();

    {
        HPR_Guard lock(&_call_map_lock);
        call_map::iterator itor = _call_map.find(str_call_id);
        if (itor == _call_map.end())
        {
            SIP_SVR_ERROR("call id %s not find!", call_id);
            return HPR_ERROR;
        }

        itor->second._stop_time = t;
    }

    return HPR_OK;
}

HPR_INT32 call_manager_t::set_trying_time_now( const char* call_id )
{
    std::string str_call_id(call_id);
    HPR_TIME_T t = HPR_TimeNow();

    {
        HPR_Guard lock(&_call_map_lock);
        call_map::iterator itor = _call_map.find(str_call_id);
        if (itor == _call_map.end())
        {
            SIP_SVR_ERROR("call id %s not find!", call_id);
            return HPR_ERROR;
        }

        itor->second._trying_time = t;
    }

    return HPR_OK;
}
