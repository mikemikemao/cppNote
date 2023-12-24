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
    //停止任务线程
    stop();

    //内存中的数据写入数据库(再执行一次任务)
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
    //每次写1000个
    HPR_INT32 write_count = 0;
    std::list<register_status_t> reg_list;
    persistence_iter_t itor;

    HPR_BOOL b_exit = HPR_FALSE;

    while(!b_exit)
    {
        {
            HPR_Guard register_map_lock(&_reg_map_mutex);
            //所有信息已全部写入数据库
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
                    //在此时判断，若已经全部写完，应该先退出本次任务
                    //防止在写数据库时，sip服务又接受到注册，导致本次任务无法立即结束
                    //锁占用时间太长，应该注册的并发
                    b_exit = HPR_TRUE;
                    break;
                }

                //每次最多写1000个
                if (write_count > 1000)
                {
                    break;
                }
            }
        }

        //写入数据库
        if (db_manager_t::instance()->write_reg_status(reg_list) != 0)
        {
            //一次写入失败，直接退出
            break;
        }
    }

    return;
}
