/**
 * @file event_upload.cpp
 * @brief ͨ���¼��ϴ���
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
    //��ֹͣ�ϴ��߳�
    stop();

    //���ڴ��е�����д�����ݿ�
    write_log_to_db();

    SIP_SVR_INFO("event upload stopped");
    return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief upload_record �ϴ���¼�������ݷ����б�
 *
 * @param call_record ���ϴ��ļ�¼�ṹ��
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
            //��¼��̫�࣬ɾ����ǰ��һ��
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
        //����ִ��һ���ϴ�
        execute_immediately();
    }

    return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief read_log_from_mem ���ڴ��еļ�¼������
 *
 * @param call_records ��д���б�
 * @param read_count ��ȡ����
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
 * @brief work �����̺߳���
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
HPR_VOID event_upload_t::work(HPR_VOID)
{
    //���ϴ������ӵļ�¼
    std::list<call_record_t> call_records;
    HPR_BOOL upload_ret = HPR_TRUE;

    while (HPR_TRUE == upload_ret)
    {
        call_records.clear();

        //��ȡ�ڴ��еļ�¼
        if (read_log_from_mem(call_records, config_t::MAX_CALL_RECORD_NUM_DB) == 0)
        {
            //�ڴ��еļ�¼��ȡ��ɣ�����ѭ��
            break;
        }
		
        //�ϴ���¼
        if (plat_sdk_t::instance()->upload_record(call_records) != 0)
        {
            //�ϴ�ʧ�ܣ���ֹ�ϴ�
            upload_ret = HPR_FALSE;
            break;
        }
    }

    if (HPR_TRUE == upload_ret)
    {
        //�ڴ��е������ϴ��ɹ�,�ϴ����ݿ��еļ�¼
        upload_log_from_db();
    }
    else
    {
        //���ڴ��еļ�¼ȫ��д�����ݿ�
        if(db_manager_t::instance()->write_call_log_sync(call_records) != 0)
        {
            //д��ʧ�ܣ�ֱ�ӷ���
            SIP_SVR_ERROR("write call log to db faild!");
            return;
        }

        //дʣ�������
        write_log_to_db();
    }


    if (HPR_TRUE == upload_ret)
    {
        HPR_Guard lock(&_call_record_lock);
        if (_call_record_ls.size() < config_t::MAX_UPLOAD_RECORD_COUNT_ONCE)
        {
            //���ڴ��е�����С��ÿ���ϴ����������������������߳�
            execute_timeout();
        }
        else
        {
            //���ﵽÿ���ϴ��������������ִ��һ������
            execute_immediately();
        }
    }

    return;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief write_log_to_db ����¼д�����ݿ�
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
            //�ڴ��е������Ѿ�ȫ������
            break;
        }

        //д�����ݿ�
        if (db_manager_t::instance()->write_call_log_sync(call_records) != 0)
        {
            //д��ʧ�ܣ�ֱ�ӷ���,дʧ�ܵ�������ʱ������ֱ�Ӷ���
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
            //��ȡ��־����ʧ�ܣ�ֱ�ӷ���
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

        //�ϴ���ȡ�ɹ��ļ�¼
        if(plat_sdk_t::instance()->upload_record(call_records) != 0)
        {
            //��־�ϴ�ʧ�ܣ�ֱ�ӷ���
            SIP_SVR_ERROR("upload call record faild!");
            return -1;
        }

        //�ϴ��ɹ���ɾ�����ݿ��еļ�¼
        if (db_manager_t::instance()->del_call_log_sync(call_records.size()) != 0)
        {
            return -1;
        }
    }

    return 0;
}
