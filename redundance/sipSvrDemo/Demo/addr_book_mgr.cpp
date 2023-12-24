
#include<hpr/HPR_Error.h>
#include<hlog/logdef.h>
#include<hpr/HPR_Time.h>

#include"addr_book_mgr.h"
#include"device_factory.h"
#include"db_manager.h"
#include"register_persistence.h"
#include"call_manager.h"
#include"device.h"


addr_book_mgr_t* addr_book_mgr_t::_instance = NULL;

addr_book_mgr_t::addr_book_mgr_t()
: work_thread_t("addr_book_mgr_t", config_t::instance()->get_heart_beat_timeout())
#if(defined(__linux__))
, _addressbook_map(10000)
#endif
,_b_exit(HPR_TRUE)
{
}

addr_book_mgr_t::~addr_book_mgr_t()
{
}

HPR_INT32 addr_book_mgr_t::create_instance()
{
    if (_instance == NULL)
    {
        _instance = new(std::nothrow)addr_book_mgr_t();
    }

    return (_instance == NULL? -1:0);
}

HPR_VOID addr_book_mgr_t::destroy_instance()
{
    if (_instance != NULL)
    {
        delete _instance;
        _instance = NULL;
    }

    return;
}

HPR_INT32 addr_book_mgr_t::init()
{
    //��־Ϊδ�˳�
    _b_exit = HPR_FALSE;

    return start();
}

HPR_VOID addr_book_mgr_t::finit()
{
    //�ȱ��Ϊ�˳�״̬�����ټ������ݿ�ȹ��̵��˳�
    _b_exit = HPR_TRUE;

    SIP_SVR_INFO("start finit address book manager ...");
    //ֹͣ�����߳�
    stop();

    //��ͨѶ¼д�����ݿ�	
	write_addressbook_to_db(); 
	//ɾ������ͨѶ¼
    clear_phone();

    SIP_SVR_INFO("address book manager stopped!");
    return;
}

HPR_VOID addr_book_mgr_t::work(HPR_VOID)
{
    //��������Ƿ�ʱ
    HPR_TIME_T t = HPR_TimeNow()/HPR_US_PER_SEC;
    read_write_lock_t::read_lock_t r_lock(&_addrbook_rwlock);

    addr_book_map::iterator itor = _addressbook_map.begin();

    while (itor != _addressbook_map.end())
    {
#if defined _DEBUG
        HPR_Debug("wangfeida_debug:dev status debug(%s): dev %s status is %d!", __FUNCTION__,\
            itor->first.c_str(), itor->second->get_dev_status());
#endif

        //ֻ�����ߵ��豸���м��
        if(itor->second->is_online_status() == HPR_TRUE)
        {
            itor->second->check_hb_timeout(t);
        }

        ++itor;
    }

    return;
}

HPR_VOID  addr_book_mgr_t::show()
{
    std::string device_ip;
    read_write_lock_t::read_lock_t r_lock(&_addrbook_rwlock);

	SIP_SVR_INFO("-----manage_center_list_nr:%d-----\n", _mgr_center_list.size());
    manage_center_list::iterator manage_center_list_itor = _mgr_center_list.begin();
    while (manage_center_list_itor != _mgr_center_list.end())
    {
    	(*manage_center_list_itor)->get_ip(device_ip);
        SIP_SVR_INFO("dev:%s status:%-2d, ip:%-16s, port:%-5d, net_zone_id:%-2d, priority:%-3d, update_time:%llu, type:%u,%u!\n", 
            (*manage_center_list_itor)->get_phone_number(), (*manage_center_list_itor)->get_dev_status(), device_ip.c_str(), (*manage_center_list_itor)->get_port(), (*manage_center_list_itor)->get_net_zone_id(),
            (*manage_center_list_itor)->get_priority(), (*manage_center_list_itor)->get_update_time(),
            (*manage_center_list_itor)->get_main_type(), (*manage_center_list_itor)->get_sub_type());
        ++manage_center_list_itor;
    }
	
	SIP_SVR_INFO("-----addressbook_map_nr:%d-----\n", _addressbook_map.size());
    addr_book_map::iterator addr_book_map_itor = _addressbook_map.begin();
    while (addr_book_map_itor != _addressbook_map.end())
    {
		addr_book_map_itor->second->get_ip(device_ip);
        SIP_SVR_INFO("dev:%s status:%-2d, ip:%-16s, port:%-5d, net_zone_id:%-2d, priority:%-3d, update_time:%llu, type:%u,%u!\n", 
            addr_book_map_itor->second->get_phone_number(), addr_book_map_itor->second->get_dev_status(), device_ip.c_str(), addr_book_map_itor->second->get_port(), addr_book_map_itor->second->get_net_zone_id(),
            addr_book_map_itor->second->get_priority(), addr_book_map_itor->second->get_update_time(),
            addr_book_map_itor->second->get_main_type(), addr_book_map_itor->second->get_sub_type());

        ++addr_book_map_itor;
    }

    return;
}

HPR_VOID  addr_book_mgr_t::show(char *phone_num)
{
    std::string device_ip;
    read_write_lock_t::read_lock_t r_lock(&_addrbook_rwlock);

	if (!phone_num)
	{
		SIP_SVR_ERROR("the phone num is NULL.");
		return;
	}

	SIP_SVR_INFO("-----addressbook_map_nr:%d-----\n", _addressbook_map.size());

    addr_book_map::iterator addr_book_map_itor = _addressbook_map.begin();
    while (addr_book_map_itor != _addressbook_map.end())
    {
    	if (!strcmp(phone_num, addr_book_map_itor->second->get_phone_number()))
    	{
			addr_book_map_itor->second->get_ip(device_ip);
	        SIP_SVR_INFO("dev:%s status:%-2d, ip:%-16s, port:%-5d, net_zone_id:%-2d, priority:%-3d, update_time:%llu, type:%u,%u!\n", 
	            addr_book_map_itor->second->get_phone_number(), addr_book_map_itor->second->get_dev_status(), device_ip.c_str(), addr_book_map_itor->second->get_port(), addr_book_map_itor->second->get_net_zone_id(),
	            addr_book_map_itor->second->get_priority(), addr_book_map_itor->second->get_update_time(),
	            addr_book_map_itor->second->get_main_type(), addr_book_map_itor->second->get_sub_type());
    	}
        ++addr_book_map_itor;
    }

    return;
}

HPR_INT32 addr_book_mgr_t::load_addressbook()
{
    HPR_INT32 read_page_size = 1000;
    HPR_INT32 read_count = 0;
    HPR_INT32 read_offset = 0;
    HPR_INT32 read_ret = -1;
    VT_PHONE_LIST phone_list;

    while (HPR_TRUE)
    {
        if (db_manager_t::instance()->load_addressbook(phone_list, read_page_size, read_offset, read_count) != 0)
        {
            SIP_SVR_ERROR("load addressbook faild!");
            break;
        }

        if (read_count == 0)
        {
            read_ret = 0;
            break;
        }
        //��ȡ�ɹ�����ӵ�ͨѶ¼�б���
        add_phone_batch(phone_list);

        //�ж��Ƿ��Ƴ��������˳�����
        if (_b_exit == HPR_TRUE)
        {
            break;
        }

        //���¶�ȡ����
        read_offset += read_count;
        phone_list.clear();
    }

    return read_ret;
}

HPR_VOID addr_book_mgr_t::write_addressbook_to_db()
{
    read_write_lock_t::read_lock_t r_lock(&_addrbook_rwlock);
    addr_book_map::iterator addr_book_map_itor = _addressbook_map.begin();
    while (addr_book_map_itor != _addressbook_map.end())
    {
		db_manager_t::instance()->add_single_phone_sync(*(addr_book_map_itor->second));
        ++addr_book_map_itor;
    }
}

HPR_INT32 addr_book_mgr_t::add_phone_batch(VT_PHONE_LIST& phone_list)
{
    VT_PHONE_ITERATOR itor_phone_list = phone_list.begin();
    addr_book_map::iterator itor_addr_book;
    HPR_INT32 add_ret = 0;

    HPR_TIME_T t_now = HPR_TimeNow()/HPR_US_PER_SEC;

    read_write_lock_t::write_lock_t addr_book_wlock(&_addrbook_rwlock);
    while (itor_phone_list != phone_list.end())
    {
        itor_addr_book = _addressbook_map.find(itor_phone_list->_phone_no);

        if (itor_addr_book == _addressbook_map.end())     //����ͨѶ¼
        {
            //����豸��ͨѶ¼
            device_t* device = device_factory_t::create_device(*itor_phone_list);
            if (device == NULL)
            {
                add_ret = -1;
                SIP_SVR_ERROR("Add phone no %s faild!Error is %d", itor_phone_list->_phone_no.c_str(), HPR_GetSystemLastError());

                //����ͨѶ¼ʧ�ܣ�ֹͣͬ��ͨѶ¼
                break;
            }
            else
            {
                //���ӵ�ͨѶ¼�б���
                _addressbook_map.insert(book_map_value_type(device->get_phone_number(), device));

                //����Ƿ���ע����Ч���ڣ������ע����Ч���ڣ�ֱ������Ϊ��ע��״̬,������ע���ַ
                if (t_now - itor_phone_list->_last_reg_time < config_t::instance()->get_valid_register_time() )
                {
                    device->on_register(itor_phone_list->_register_ip.c_str(), static_cast<HPR_UINT16>(itor_phone_list->_register_port), itor_phone_list->_last_reg_time);
                }

                //���Ϊ���������Ҫ��ӵ�������б���
                if (device->is_manager_center() == HPR_TRUE)
                {
                    add_to_manager_center(device);
                }
            }
        }
        else if(itor_addr_book->second->need_update(*itor_phone_list) == HPR_TRUE)     //����ͨѶ¼
        {
            itor_addr_book->second->update(*itor_phone_list);
            //���ԭ��Ϊ����������ڲ��ǹ��������Ҫ�ӹ�����б���ɾ��
            if ((itor_addr_book->second->is_manager_center() == HPR_TRUE) &&\
                (device_t::check_manager_center(itor_phone_list->_dev_main_type, itor_phone_list->_dev_sub_type) == HPR_FALSE))
            {
                del_from_manager_center(itor_addr_book->second);
            }
            else if((itor_addr_book->second->is_manager_center() == HPR_FALSE) &&\
                (device_t::check_manager_center(itor_phone_list->_dev_main_type, itor_phone_list->_dev_sub_type) == HPR_TRUE))
            {
                //���ԭ�����ǹ�����������ǹ��������Ҫ��ӵ�������б���
                add_to_manager_center(itor_addr_book->second);
            }
            else if((itor_addr_book->second->is_manager_center() == HPR_TRUE) &&\
                (device_t::check_manager_center(itor_phone_list->_dev_main_type, itor_phone_list->_dev_sub_type) == HPR_TRUE))
            {
                del_from_manager_center(itor_addr_book->second);
                add_to_manager_center(itor_addr_book->second);
            }
        }

        ++itor_phone_list;
    }

    return add_ret;
}

HPR_INT32 addr_book_mgr_t::update_phone_batch(VT_PHONE_LIST& phone_list)
{
    return add_phone_batch(phone_list);
}

HPR_INT32 addr_book_mgr_t::delete_phone_batch(std::list<std::string>& phone_list)
{
    std::list<std::string>::iterator itor = phone_list.begin();

    addr_book_map::iterator itor_addr_book;
    read_write_lock_t::write_lock_t addr_book_wlock(&_addrbook_rwlock);
    while (itor != phone_list.end())
    {
        itor_addr_book = _addressbook_map.find(*itor);
        if (itor_addr_book != _addressbook_map.end())
        {
        	del_from_manager_center(itor_addr_book->second);
            delete itor_addr_book->second;
            _addressbook_map.erase(itor_addr_book);
        }

        ++itor;
    }

    return 0;
}

HPR_INT32 addr_book_mgr_t::clear_phone()
{
    addr_book_map::iterator itor_addr_book;
    read_write_lock_t::write_lock_t addr_book_wlock(&_addrbook_rwlock);
    itor_addr_book = _addressbook_map.begin();

    while (itor_addr_book != _addressbook_map.end())
    {
        delete itor_addr_book->second;

        ++itor_addr_book;
    }

    _addressbook_map.clear();

    return 0;
}

HPR_INT32 addr_book_mgr_t::add_phone(vt_phone_t& phone)
{
    //����豸��ͨѶ¼
    device_t* device = device_factory_t::create_device(phone);
    if (device == NULL)
    {
        SIP_SVR_ERROR("Add phone no %s faild!Error is %d", phone._phone_no.c_str(), HPR_GetSystemLastError());

        return -1;
    }
    else
    {
        //���ӵ�ͨѶ¼�б���
        _addressbook_map.insert(book_map_value_type(device->get_phone_number(), device));

        //���Ϊ���������Ҫ��ӵ�������б���
        if (device->is_manager_center() == HPR_TRUE)
        {
            add_to_manager_center(device);
        }
    }

    return 0;
}

HPR_INT32 addr_book_mgr_t::redirect_addr(device_t* device)
{
    HPR_INT32 rt = HPR_ERROR;
    device_t* outdoor_device = NULL;
    std::string outdoor_device_ip;

	do
	{
      	if (device == NULL)
      	{
            break;
		}

	    if (device->is_analog_indoor_device())
	    {
	        outdoor_device = get_outdoor_device(device->get_phone_number());
        	if (outdoor_device == NULL)
        	{
       			SIP_SVR_ERROR("outdoor device is not in addr book.");
	            break;
        	}
			SIP_SVR_INFO("indoor(%s) -----> outdoor(%s)", device->get_phone_number(), outdoor_device->get_phone_number());
	        if (HPR_FALSE == outdoor_device->is_online_status())
	        {
		        SIP_SVR_ERROR("outdoor device(%s) is not online.", outdoor_device->get_phone_number());
	            break;
	        }

	        outdoor_device->get_ip(outdoor_device_ip);
	        device->set_ip(outdoor_device_ip);
	        device->set_port(outdoor_device->get_port());
	    }

		rt = HPR_OK;
	} while(0);

	return rt;
}

#if 0
HPR_INT32 addr_book_mgr_t::refresh_addr_book(std::string& indexcode, HPR_UINT32 sync_sequence)
{
    addr_book_map::iterator itor_addr_book;

    read_write_lock_t::write_lock_t addr_book_wlock(&_addrbook_rwlock);
    itor_addr_book = _addressbook_map.begin();
    while (itor_addr_book != _addressbook_map.end())
    {
        if (itor_addr_book->second->check_indexcode(indexcode) == HPR_FALSE)
        {
            //�����ڸ�ͬ��Դ���豸����������
            ++itor_addr_book;
        }
        else
        {
            if (itor_addr_book->second->get_sync_sequence() != sync_sequence)
            {
                //֪ͨ���ݿ�ģ�飬ɾ��ͨ��¼

                //����ǹ��������Ҫ�ӹ�����б���ɾ��
                if (itor_addr_book->second->is_manager_center() == HPR_TRUE)
                {
                    del_from_manager_center(itor_addr_book->second);
                }

                //��ͨѶ¼��ɾ��.�˴�����ֱ��ɾ��ͨѶ¼������Ϊ��д�����ռ������û�������̻߳��д��
                //������Ҫ��֤�����ģ���ȡ��ʹ��ͨѶ¼ʱҪ�Ӷ�����һֱ����ȡ��ɡ�
                //�����Ƕ�д�������Զ����ܼ�����Ӱ��.
                delete itor_addr_book->second;
                itor_addr_book = _addressbook_map.erase(itor_addr_book);
            }
            else
            {
                //ͨѶ¼û�б�ɾ������������
                ++itor_addr_book;
            }
        }
    }

    return 0;
}
#endif

HPR_INT32 addr_book_mgr_t::add_to_manager_center(device_t* device)
{
    manage_center_list::iterator itor = _mgr_center_list.begin();

    while (itor != _mgr_center_list.end())
    {
        if ((*itor)->get_priority() <= device->get_priority())
        {
            itor = _mgr_center_list.insert(itor, device);
            break;
        }

        ++itor;
    }

    //����ǰ���ȼ���ͻ��б�Ϊ�գ������������
    if (itor == _mgr_center_list.end())
    {
        _mgr_center_list.push_back(device);
    }

    return 0;
}

HPR_INT32 addr_book_mgr_t::del_from_manager_center(device_t* device)
{
    manage_center_list::iterator itor = _mgr_center_list.begin();

    while (itor != _mgr_center_list.end())
    {
        if (*itor == device)
        {
            _mgr_center_list.erase(itor);
            break;
        }

        ++itor;
    }

    return 0;
}

//��ȡһ�����еĹ�������
//call_id					���κ���ID
//from_phone_num		���к���
device_t* addr_book_mgr_t::get_idle_manager_center(const char* call_id, const char* from_phone_num)
{
	std::string phone_num = config_t::instance()->get_mgr_phone_num();

	if (HPR_OK == call_manager_t::instance()->get_called_phone_number(call_id, phone_num))
	{
		manage_center_list::iterator it = _mgr_center_list.begin();
		while (it != _mgr_center_list.end())
		{
			if (strcmp((*it)->get_phone_number(), phone_num.c_str()) == 0)
			{
				SIP_SVR_WARN("Resend invite, call-id:%s", call_id);
				return (*it);
			}

			++it;
		}
	}

	manage_center_list::iterator itor = _mgr_center_list.begin();

	while (itor != _mgr_center_list.end())
	{
		if ( ((*itor)->is_idle_status() == HPR_TRUE) && (strncmp((*itor)->get_phone_number(), from_phone_num, 1) == 0) )
		{
			//���óɹ����ٷ��أ���Ϊ�п����ڻ�ȡ�����õļ���ڣ��豸״̬�����ı�
			if ((*itor)->set_busy_status() == HPR_TRUE)
			{
				return (*itor);
			}
		}

		++itor;
	}
	SIP_SVR_INFO("not find idle manage_center, call_id:%s", call_id);
	return NULL;
}