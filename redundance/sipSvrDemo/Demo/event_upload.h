/** @file:   event_upload.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  事件上传
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

    //初始化
    HPR_INT32 init();
    //反初始化
    HPR_INT32 finit();

    //上传通话日志
    HPR_INT32 upload_record(call_record_t& call_record);

protected:

    //工作函数
    HPR_VOID work(HPR_VOID);

private:

    //从内存中读取记录
    HPR_INT32 read_log_from_mem(std::list<call_record_t>& call_records, HPR_UINT32 read_count);
    //写内存中的记录到数据库
    HPR_INT32 write_log_to_db();
    //上传数据库中的记录
    HPR_INT32 upload_log_from_db();

private:

    static event_upload_t* _instance;
    //需要上传的日志列表
    std::list<call_record_t> _call_record_ls;
    //日志锁
    HPR_Mutex _call_record_lock;

};

#endif
