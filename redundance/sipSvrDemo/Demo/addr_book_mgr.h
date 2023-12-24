/**
 * @file addr_book_mgr.h
 * @brief ͨѶ¼�����࣬����
 * @author zhanghaifengyf1@hikvision.com.cn
 * @version 0.0.1
 * @date 2014-07-08 16:23:10
 */

#ifndef ADDR_BOOK_MGR_QL4YB8WJ
#define ADDR_BOOK_MGR_QL4YB8WJ

#include"definer.h"
#include"hash_map_def.h"
#include"device.h"
#include"read_write_lock.h"
#include"work_thread.h"
#include"net_manage_parser.h"

class CUpgradeSrv;
class addr_book_mgr_t
    :public work_thread_t
{
    friend class CUpgradeSrv;
    //������������
#if (defined(_WIN32) || defined(_WIN64))
    typedef stdext::hash_map<std::string, device_t*> addr_book_map;
#elif (defined(__linux__))
    typedef __gnu_cxx::hash_map<std::string, device_t*> addr_book_map;
#else
    #error unsupport!
#endif

    typedef addr_book_map::iterator addr_book_iterator;
    typedef addr_book_map::value_type book_map_value_type;


public:
    typedef std::list<device_t*> manage_center_list;

private:
    addr_book_mgr_t ();
    virtual ~addr_book_mgr_t ();

public:

    //��ȡ����ʵ��
    inline static addr_book_mgr_t* instance()
    {
        return _instance;
    }

    inline HPR_VOID lock_address_book_r()
    {
        _addrbook_rwlock.enter_read();
        return;
    }

    inline HPR_VOID unlock_address_book_r()
    {
        _addrbook_rwlock.leave_read();
        return;
    }

    inline NET_MANAGE_DEVICE_INFO_T* get_devices_status(HPR_INT32* num)
    {
        lock_address_book_r();
        *num = _addressbook_map.size();
        NET_MANAGE_DEVICE_INFO_T* info = NULL;
        if (*num > 0)
        {
            info = new NET_MANAGE_DEVICE_INFO_T[*num];
            addr_book_map::iterator itor = _addressbook_map.begin();
            HPR_INT32 i = 0;
            for (; itor!=_addressbook_map.end(); itor++)
            {
                HPR_Snprintf(info[i].device_index, 64, "%s", itor->first.c_str() );
                info[i].device_status = (itor->second->is_online_status()==HPR_TRUE)?0:1;
                //itor->second->set_unchanged();
                i++;
            }
        }

        unlock_address_book_r();
        return info;
    }

    //inline NET_MANAGE_DEVICE_INFO_T* get_changed_devices_status(HPR_INT32* num)
    //{
    //    lock_address_book_r();
    //    std::list<NET_MANAGE_DEVICE_INFO_T> changed_devices;
    //    addr_book_map::iterator itor = _addressbook_map.begin();
    //    NET_MANAGE_DEVICE_INFO_T dev_status;
    //    for (; itor!=_addressbook_map.end(); itor++)
    //    {
    //        if (HPR_TRUE == itor->second->is_changed() )
    //        {
    //            HPR_Snprintf(dev_status.device_index, 64, "%s", itor->first.c_str() );
    //            dev_status.device_status = (itor->second->is_online_status()==HPR_TRUE)?0:1;
    //            changed_devices.push_back(dev_status);
    //            itor->second->set_unchanged();
    //        }
    //    }
    //    unlock_address_book_r();

    //    *num = changed_devices.size();
    //    NET_MANAGE_DEVICE_INFO_T* info = NULL;
    //    if (*num > 0)
    //    {
    //        info = new NET_MANAGE_DEVICE_INFO_T[*num];
    //        std::list<NET_MANAGE_DEVICE_INFO_T>::iterator it = changed_devices.begin();
    //        int i = 0;
    //        for (; it!=changed_devices.end(); it++)
    //        {
    //            info[i] = *it;
    //            i++;
    //        }
    //    }

    //    return info;
    //}

    inline HPR_VOID free_devices_status(NET_MANAGE_DEVICE_INFO_T* status)
    {
        if (status != NULL)
        {
            delete[] status;
        }
    }

    //��ȡͨѶ¼����
    //phone_number		���к���
    //status_code		״̬��
    //call_id				��ʶ���κ��е�ID
    //from_phone_num	���к���
    inline device_t* get_address_book_for_call(const char* phone_number, HPR_INT32& status_code, const char* call_id, const char* from_phone_num)
    {
        device_t* dev = NULL;

        if (0 == strcmp(phone_number, config_t::instance()->get_mgr_phone_num()))
        {
            //��Ϊ��������򷵻�һ�����еĹ����
            dev = get_idle_manager_center(call_id, from_phone_num);
            if (NULL == dev)
            {
                status_code = UAS_STATUS_486;
            }
        }
        else
        {
            //�ǹ������������״̬�жϣ�ֱ�ӷ����豸ָ��
            HPR_INT32 dev_main_type = 0;
			HPR_INT32 dev_sub_type = 0;
			char call_num[16] = {0};
			
			memcpy(call_num, phone_number, 16);
            dev = get_address_book_not_mgr(phone_number);
            if (NULL == dev)
            {
                device_t::get_dev_type(call_num, dev_main_type, dev_sub_type);
			    if ((MAIN_TYPE_INDOOR == dev_main_type) && (SUB_TYPE_INDOOR == dev_sub_type))
			    {
			        dev = get_outdoor_device(phone_number);
					if (NULL == dev)
					{
					    status_code = UAS_STATUS_404;    
					}
			    }
				else
				{
				    status_code = UAS_STATUS_404;
				}                        
			}
			else
			{
			     std::string call_number(phone_number);
				 dev->set_call_number(call_number);
			}
        }
		
        return dev;
    }

	inline device_t *get_address_book_for_register(const char* phone_number)
	{
        std::string num(phone_number);
        addr_book_map::iterator itor = _addressbook_map.find(num);
        return (itor == _addressbook_map.end() ? NULL:itor->second);
    }

    inline device_t* get_address_book(const char* phone_number)
    {
        std::string num(phone_number);
		device_t * dev = NULL;
        addr_book_map::iterator itor = _addressbook_map.find(num);
		if( itor == _addressbook_map.end() )
		{
		    HPR_INT32 dev_main_type = 0;
			HPR_INT32 dev_sub_type = 0;
			char call_num[16] = {0};
			memcpy(call_num,phone_number,16);
		    device_t::get_dev_type(call_num, dev_main_type, dev_sub_type);
			if( MAIN_TYPE_INDOOR == dev_main_type && SUB_TYPE_INDOOR == dev_sub_type )
			{
		        dev = get_outdoor_device(phone_number);
				if(dev)
				{
				    dev->set_call_number(num);
				}
				return dev;
			}
			else
			{
			    return NULL;
			}
		}
		else
		{
		    itor->second->set_call_number(num);
		    return itor->second;
		}
    }

    inline device_t* get_outdoor_device(const char* indoor_phone_no)
    {
        std::string num(indoor_phone_no);
        num.replace(NUM_TO_REPLACE, FLOOR_FIGURE, OUTDOOR_FLAG);
        addr_book_map::iterator itor = _addressbook_map.find(num);
        return (itor == _addressbook_map.end() ? NULL:itor->second);
    }

    //��������ʵ��
    static HPR_INT32 create_instance();
    //ɾ������ʵ��
    static HPR_VOID destroy_instance();
    //��ʼ��
    HPR_INT32 init();
    //����ʼ��
    HPR_VOID finit();

    //��ʾͨѶ¼��Ϣ
    HPR_VOID  show();
    HPR_VOID  show(char *phone_num);
    //����ͨѶ¼
    HPR_INT32 load_addressbook();
    //��ͨѶ¼д�����ݿ�
	HPR_VOID write_addressbook_to_db();
    //����ͨѶ¼,����ͨѶ¼ͬ��
    HPR_INT32 add_phone_batch(VT_PHONE_LIST& phone_list);
    //����ͨѶ¼
    HPR_INT32 update_phone_batch(VT_PHONE_LIST& phone_list);
    //ɾ��ͨѶ¼
    HPR_INT32 delete_phone_batch(std::list<std::string>& phone_list);
    //ɾ������ͨѶ¼
    HPR_INT32 clear_phone();
    //����ͨѶ¼�����ڴ����ݿ����
    HPR_INT32 add_phone(vt_phone_t& phone);
    //��indexcodeˢ��ͨѶ¼������ͬ�����кţ�ɾ����Ҫɾ����ͨ��¼
    //ͨѶ¼��ͬ���߼����ⲿ���
    //HPR_INT32 refresh_addr_book(std::string& indexcode, HPR_UINT32 sync_sequence);
    inline manage_center_list get_center_list()
    {
        return _mgr_center_list;
    }
	HPR_INT32 redirect_addr(device_t* device);

protected:

    HPR_VOID work(HPR_VOID);

private:

    //���ӵ�������б���
    HPR_INT32 add_to_manager_center(device_t* device);
    //�ӹ�����б���ɾ��
    HPR_INT32 del_from_manager_center(device_t* device);
    //��ȡһ�����еĹ�������
    device_t* get_idle_manager_center(const char* call_id, const char* from_phone_num);
    //��ȡ�ǹ����
    inline device_t* get_address_book_not_mgr(const char* phone_number)
    {
        addr_book_map::iterator itor = _addressbook_map.find(phone_number);
        return (itor == _addressbook_map.end() ? NULL:itor->second);
    } 

private:

    static addr_book_mgr_t* _instance;
    //ͨѶ¼
    addr_book_map _addressbook_map;
    //���������б�, �����ȼ��ߵͽ�������
    manage_center_list _mgr_center_list;
    //ͨѶ¼��д��
    read_write_lock_t _addrbook_rwlock;

    //�����Ƿ��˳�
    HPR_BOOL _b_exit;

} /* addr_book_mgr_t */;

#endif /* end of include guard: ADDR_BOOK_MGR_QL4YB8WJ */
