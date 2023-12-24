#include<hpr/HPR_Guard.h>
#include<hpr/HPR_Utils.h>
#include<hlog/logdef.h>
#include"register_persistence.h"
#include"db_manager.h"

register_persistence_t* register_persistence_t::_instance = NULL;

register_persistence_t::register_persistence_t()
: work_thread_t("reg_status_t", config_t::instance()->get_valid_register_time()/2)
{

}

register_persistence_t::~register_persistence_t()
{

}

HPR_INT32 register_persistence_t::creat_instance()
{
    if (_instance == NULL)
    {
        _instance = new(std::nothrow)register_persistence_t();
    }

    return (_instance == NULL ? -1 : 0);
}

HPR_VOID register_persistence_t::del_instance()
{
    if (_instance != NULL)
    {
        delete _instance;
        _instance = NULL;
    }

    return;
}

HPR_INT32 register_persistence_t::init()
{
    return start();
}

HPR_VOID register_persistence_t::finit()
{
    SIP_SVR_INFO("begin fini.");
    //ֹͣ�����߳�
    stop();

    //�ڴ��е�����д�����ݿ�(��ִ��һ������)
    work();
    SIP_SVR_INFO("end fini.");

    return;
}

HPR_INT32 register_persistence_t::persistence_reg_status(register_status_t& reg_info)
{
    persistence_iter_t itor;

    HPR_Guard register_map_lock(&_reg_map_mutex);
    itor = _reg_persistence_map.find(reg_info._phone_no);
    if (itor != _reg_persistence_map.end())
    {
        itor->second = reg_info;
    }
    else
    {
        _reg_persistence_map.insert(persistence_map_t::value_type(reg_info._phone_no, reg_info));
    }

    return 0;
}

HPR_INT32 register_persistence_t::persistence_offline_status(const char* phone_num)
{
    register_status_t reg_info(phone_num, "", 0, 0);

    persistence_iter_t itor;

    HPR_Guard register_map_lock(&_reg_map_mutex);
    itor = _reg_persistence_map.find(reg_info._phone_no);
    if (itor != _reg_persistence_map.end())
    {
        itor->second = reg_info;
    }
    else
    {
        _reg_persistence_map.insert(persistence_map_t::value_type(reg_info._phone_no, reg_info));
    }

    return 0;
}

HPR_VOID register_persistence_t::work(HPR_VOID)
{
    //ÿ��д1000��
    HPR_INT32 write_count = 0;
    std::list<register_status_t> reg_list;
    persistence_iter_t itor;

    HPR_BOOL b_exit = HPR_FALSE;

    while(!b_exit)
    {
        {
            HPR_Guard register_map_lock(&_reg_map_mutex);
            //������Ϣ��ȫ��д�����ݿ�
            if (_reg_persistence_map.size() == 0)
            {
                return;
            }

            itor = _reg_persistence_map.begin();
            reg_list.clear();
            write_count = 1;

            while (1)
            {
                reg_list.push_back(itor->second);

                ++write_count;

                //itor = _reg_persistence_map.erase(itor);
                _reg_persistence_map.erase(itor++);

                if (itor == _reg_persistence_map.end())
                {
                    //�ڴ�ʱ�жϣ����Ѿ�ȫ��д�꣬Ӧ�����˳���������
                    //��ֹ��д���ݿ�ʱ��sip�����ֽ��ܵ�ע�ᣬ���±��������޷���������
                    //��ռ��ʱ��̫����Ӧ��ע��Ĳ���
                    b_exit = HPR_TRUE;
                    break;
                }

                //ÿ�����д1000��
                if (write_count > 1000)
                {
                    break;
                }
            }
        }

        //д�����ݿ�
        if (db_manager_t::instance()->write_reg_status(reg_list) != 0)
        {
            //һ��д��ʧ�ܣ�ֱ���˳�
            break;
        }
    }

    return;
}
