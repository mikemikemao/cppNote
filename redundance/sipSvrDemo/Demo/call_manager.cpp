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
    //ֹͣ�����߳�
    stop();

    //��������ͨ��ΪINVALID_CALL_STATUS�����ϴ�(д�����ݿ�)
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

        //�ϴ�ͨ����¼
        event_upload_t::instance()->upload_record(itor->second);
        ++itor;
    }

    _call_map.clear();

    SIP_SVR_INFO("call manager stopped!");

    return 0;
}

//>>  ��ʼ��һ�κ��У����ú���״̬���жϺ��кϷ��ԣ�
//call_id    ����id
//calling_phone   ���з�����
//called_phone    ���з�����
//error_code ������
HPR_INT32 call_manager_t::init_call(const char* call_id, device_t* calling_phone, device_t* called_phone, HPR_INT32& error_code)
{
    //�����ô�����Ϊ0
    error_code = UAS_STATUS_200;

    SIP_SVR_INFO("call_id:%s, calling:%s(%d), called:%s(%d)", call_id, calling_phone->get_phone_number(), 
    calling_phone->get_dev_status(), called_phone->get_phone_number(), called_phone->get_dev_status());

    //���з��򱻽з������ߣ���ֹͨ��
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
        //������з��Ƿ��Ѿ��ں����б��У����Ѿ��ں����б��У���Ӻ����б���ɾ��
        check_phone_in_call(calling_phone->get_phone_number(), call_id);
        //���з�Ϊ�������ģ���ʱ���ܹ������Ĵ��ں���״̬����Ӧ�������з�Ϊæ��״̬(����״̬�޷����ã���֧������)
        calling_phone->set_busy_status();
    }
    else if((called_phone->is_manager_center() == HPR_TRUE) &&\
        (calling_phone->is_manager_center() == HPR_FALSE))
    {
        //���з�Ϊ�������ģ������ڻ�ȡ��������ʱ�ѶԱ��з�����״̬����(����޶ȱ�֤���гɹ�)���˴�����Ҫ���д���
    }
	else if((called_phone->is_manager_center() == HPR_TRUE) &&\
        (calling_phone->is_manager_center() == HPR_TRUE))
    {
         calling_phone->set_busy_status();
		 called_phone->set_busy_status();
    }
    else
    {
        //���з��뱻�з������ǹ������Ļ��ǹ������ģ���ʱ����Ҫ��¼ͨ����־��ֱ�ӷ���
        return 0;
    }

    call_record_t call_info;

    call_info._call_id = call_id;
    call_info._calling_phone = calling_phone->is_manager_center() ? calling_phone->get_phone_number() : calling_phone->get_call_number();
    call_info._called_phone = called_phone->is_manager_center() ? called_phone->get_phone_number() : called_phone->get_call_number();
	SIP_SVR_INFO("calling phone : %s, called phone : %s.\n",call_info._calling_phone.c_str(),call_info._called_phone.c_str());
    //���ó�ʼ״̬Ϊ���ں���
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
        //����ʧ��ʱ����������״̬����ɣ���Ҫ���б���ɾ��
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

    //���ù����״̬Ϊ����
    device_t* device = NULL;
    addr_book_mgr_t::instance()->lock_address_book_r();

    //�������з�Ϊ����״̬
    device = addr_book_mgr_t::instance()->get_address_book(call_info._called_phone.c_str());
   
    if (device != NULL)
    {
        device->set_idle_status();
    }
    //���ñ��з�Ϊ����״̬
    device = addr_book_mgr_t::instance()->get_address_book(call_info._calling_phone.c_str());
	
    if (device != NULL)
    {
        device->set_idle_status();
    }

    addr_book_mgr_t::instance()->unlock_address_book_r();
    //�ϴ�ͨ����־��CMS
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

    //���ù����״̬Ϊ����
    device_t* device = NULL;
    addr_book_mgr_t::instance()->lock_address_book_r();

    //�������з�Ϊ����״̬
    device = addr_book_mgr_t::instance()->get_address_book(call_rcd._called_phone.c_str());
    if (device != NULL)
    {
        device->set_idle_status();
    }
    //���ñ��з�Ϊ����״̬
    device = addr_book_mgr_t::instance()->get_address_book(call_rcd._calling_phone.c_str());
    if (device != NULL)
    {
        device->set_idle_status();
    }

    addr_book_mgr_t::instance()->unlock_address_book_r();
    //�ϴ�ͨ����־��CMS
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
            //���ô˴�ͨ����ʱ����
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
    //����Ƿ��г�ʱ��ͨ��
    HPR_TIME_T t = HPR_TimeNow();

    HPR_Guard lock(&_call_map_lock);
    call_map::iterator itor = _call_map.begin();

    while (itor != _call_map.end())
    {
        if (itor->second._trying_time == 0)
        {
            if ( (t - itor->second._invite_time) >= (config_t::instance()->get_max_no_rsp_time()*HPR_US_PER_SEC) )
            {
                //δ�ӳ�ʱ������ʱ������󷵻�ʱ��ʱ��ɾ��ͨ����¼
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
                //�������ֻ����ͨ��˫��ͬʱ�쳣��˫����û�з�bye�������ACK��ʧ
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

    //���������ֱ�ӷ��سɹ�
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
