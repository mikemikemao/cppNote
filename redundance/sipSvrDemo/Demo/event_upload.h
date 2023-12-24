/** @file:   event_upload.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  �¼��ϴ�
 *
 *  @author: wangfeida@hikvision.com.cn
 *  @data:   2014/07/31
 *  
 *  @note:   
 *
 *  @Modification History:
 *  <version>       <time>          <author>        <desc>
 *    V1.0        2014/07/31        wangfeida       Create
 *
 *  @warning: <No waining>        
*/

#ifndef _EVENT_UPLOAD_H_
#define _EVENT_UPLOAD_H_

#include<list>

#include"work_thread.h"
#include"definer.h"
#include<hpr/HPR_Mutex.h>

class event_upload_t
    : public work_thread_t
{
private:
    event_upload_t();
    virtual ~event_upload_t();

public:

    static HPR_INT32 create_instance();
    static HPR_VOID delete_instance();

    inline static event_upload_t* instance()
    {
        return _instance;
    }

    //��ʼ��
    HPR_INT32 init();
    //����ʼ��
    HPR_INT32 finit();

    //�ϴ�ͨ����־
    HPR_INT32 upload_record(call_record_t& call_record);

protected:

    //��������
    HPR_VOID work(HPR_VOID);

private:

    //���ڴ��ж�ȡ��¼
    HPR_INT32 read_log_from_mem(std::list<call_record_t>& call_records, HPR_UINT32 read_count);
    //д�ڴ��еļ�¼�����ݿ�
    HPR_INT32 write_log_to_db();
    //�ϴ����ݿ��еļ�¼
    HPR_INT32 upload_log_from_db();

private:

    static event_upload_t* _instance;
    //��Ҫ�ϴ�����־�б�
    std::list<call_record_t> _call_record_ls;
    //��־��
    HPR_Mutex _call_record_lock;

};

#endif
