/**
 * @file event_upload.cpp
 * @brief 通话事件上传类
 * @author zhanghaifengyf1@hikvision.com.cn
 * @version 0.0.1
 * @date 2014-11-19 15:33:46
 */

#include"event_upload.h"
#include"config.h"
#include"db_manager.h"
#include<hpr/HPR_Guard.h>
#include <hpr/HPR_Utils.h>
#include<hlog/logdef.h>
#include"plat_sdk.h"

#define MAX_CALL_RCD_IN_MEM 10000

event_upload_t* event_upload_t::_instance = NULL;

event_upload_t::event_upload_t()
: work_thread_t("event_upload_t", config_t::LOG_RE_UPLOAD_TIME)
{
}

event_upload_t::~event_upload_t()
{

}

HPR_INT32 event_upload_t::create_instance()
{
    if (_instance == NULL)
    {
        _instance = new(std::nothrow)event_upload_t();
    }

    return (_instance == NULL ? -1 : 0);
}

HPR_VOID event_upload_t::delete_instance()
{
    if (_instance != NULL)
    {
        delete _instance;
        _instance = NULL;
    }

    return;
}

HPR_INT32 event_upload_t::init()
{
    return start();
}

HPR_INT32 event_upload_t::finit()
{
    SIP_SVR_INFO("start finit event upload ...");
    //先停止上传线程
    stop();

    //把内存中的数据写入数据库
    write_log_to_db();

    SIP_SVR_INFO("event upload stopped");
    return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief upload_record 上传记录，将数据放入列表
 *
 * @param call_record 将上传的记录结构体
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
HPR_INT32 event_upload_t::upload_record(call_record_t& call_record)
{
    HPR_BOOL execute_task = HPR_FALSE;

    {
        HPR_Guard lock(&_call_record_lock);
        if (_call_record_ls.size() > MAX_CALL_RCD_IN_MEM)
        {
            //记录数太多，删除最前边一条
            _call_record_ls.erase(_call_record_ls.begin());
            HPR_Debug("delete one record!");
        }

        _call_record_ls.push_back(call_record);
        if (_call_record_ls.size() >= config_t::MAX_UPLOAD_RECORD_COUNT_ONCE)
        {
            execute_task = HPR_TRUE;
        }
    }

    if (HPR_TRUE == execute_task)
    {
        //立即执行一次上传
        execute_immediately();
    }

    return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief read_log_from_mem 将内存中的记录读出来
 *
 * @param call_records 待写入列表
 * @param read_count 读取数量
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
HPR_INT32 event_upload_t::read_log_from_mem(std::list<call_record_t>& call_records, HPR_UINT32 read_count)
{
    std::list<call_record_t>::iterator itor;
    HPR_UINT32 upload_count = 0;

    HPR_Guard lock(&_call_record_lock);
    if (_call_record_ls.size() > 0)
    {
        itor = _call_record_ls.begin();

        while (itor != _call_record_ls.end())
        {
            call_records.push_back(*itor);
            itor = _call_record_ls.erase(itor);

            ++upload_count;
            if (upload_count >= read_count)
            {
                break;
            }
        }
    }

    return static_cast<HPR_INT32>(upload_count);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief work 工作线程函数
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
HPR_VOID event_upload_t::work(HPR_VOID)
{
    //先上传新增加的记录
    std::list<call_record_t> call_records;
    HPR_BOOL upload_ret = HPR_TRUE;

    while (HPR_TRUE == upload_ret)
    {
        call_records.clear();

        //读取内存中的记录
        if (read_log_from_mem(call_records, config_t::MAX_CALL_RECORD_NUM_DB) == 0)
        {
            //内存中的记录读取完成，跳出循环
            break;
        }
		
        //上传记录
        if (plat_sdk_t::instance()->upload_record(call_records) != 0)
        {
            //上传失败，终止上传
            upload_ret = HPR_FALSE;
            break;
        }
    }

    if (HPR_TRUE == upload_ret)
    {
        //内存中的数据上传成功,上传数据库中的记录
        upload_log_from_db();
    }
    else
    {
        //把内存中的记录全部写入数据库
        if(db_manager_t::instance()->write_call_log_sync(call_records) != 0)
        {
            //写入失败，直接返回
            SIP_SVR_ERROR("write call log to db faild!");
            return;
        }

        //写剩余的数据
        write_log_to_db();
    }


    if (HPR_TRUE == upload_ret)
    {
        HPR_Guard lock(&_call_record_lock);
        if (_call_record_ls.size() < config_t::MAX_UPLOAD_RECORD_COUNT_ONCE)
        {
            //若内存中的数据小于每次上传的最大数量，则挂起任务线程
            execute_timeout();
        }
        else
        {
            //若达到每次上传的最大数量，再执行一次任务
            execute_immediately();
        }
    }

    return;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief write_log_to_db 将记录写入数据库
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
HPR_INT32 event_upload_t::write_log_to_db()
{
    std::list<call_record_t> call_records;

    while (HPR_TRUE)
    {
        call_records.clear();

        if (read_log_from_mem(call_records, config_t::MAX_CALL_RECORD_NUM_DB) == 0)
        {
            //内存中的数据已经全部读完
            break;
        }

        //写入数据库
        if (db_manager_t::instance()->write_call_log_sync(call_records) != 0)
        {
            //写入失败，直接返回,写失败的数据暂时不处理，直接丢掉
            return -1;
        }
    }

    return 0;
}

HPR_INT32 event_upload_t::upload_log_from_db()
{
    HPR_INT32 log_count = 0;
    std::list<call_record_t> call_records;

    while(HPR_TRUE)
    {
        log_count = db_manager_t::instance()->get_call_log_count();
        if (log_count < 0)
        {
            //读取日志数量失败，直接返回
            return -1;
        }
        else if(log_count == 0)
        {
            break;
        }

        call_records.clear();

        if(db_manager_t::instance()->read_call_log_sync(config_t::MAX_CALL_RECORD_NUM_DB, call_records) != 0)
        {
            return -1;
        }

        //上传读取成功的记录
        if(plat_sdk_t::instance()->upload_record(call_records) != 0)
        {
            //日志上传失败，直接返回
            SIP_SVR_ERROR("upload call record faild!");
            return -1;
        }

        //上传成功，删除数据库中的记录
        if (db_manager_t::instance()->del_call_log_sync(call_records.size()) != 0)
        {
            return -1;
        }
    }

    return 0;
}
