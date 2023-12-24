/** @file:   call_manager.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  呼叫管理（主要负责呼叫日志生成及超时管理）
             设备状态（管理机）的更新都在此类完成（除获取空闲管理机外（最大限度保证呼叫成功），其他情况下对设备进行更新
              都封装在这里）。
 *
 *  @author: wangfeida@hikvision.com.cn
 *  @data:   2014/07/22
 *  
 *  @note:   
 *
 *  @Modification History:
 *  <version>       <time>          <author>        <desc>
 *    V1.0        2014/07/22        wangfeida       Create
 *
 *  @warning: <No waining>        
*/

#ifndef _CALL_MANAGER_H_
#define _CALL_MANAGER_H_

#include<map>

#include<hpr/HPR_Mutex.h>
#include"definer.h"
#include"work_thread.h"
#include"device.h"

class call_manager_t
    : public work_thread_t
{
private:

    call_manager_t();
    virtual ~call_manager_t();

public:

    static HPR_INT32 create_instance();
    static HPR_VOID destroy_instance();

    inline static call_manager_t* instance()
    {
        return _instance;
    }

    //初始化
    HPR_INT32 init();
    //反初始化
    HPR_INT32 finit();
    //增加一次呼叫
    HPR_INT32 init_call(const char* call_id, device_t* calling_phone, device_t* called_phone,\
		HPR_INT32& error_code);
    
    //设置呼叫状态
    inline HPR_INT32 set_call_faild_status(const char* call_id)
    {
        return set_call_status(call_id, call_status_faild);
    }
    //设置呼叫超时状态(cancel)
    inline HPR_INT32 set_call_timeout_status(const char* call_id)
    {
        return set_call_status(call_id, call_status_no_one_answer);
    }
    //设置忙线状态
    inline HPR_INT32 set_busy_status(const char* call_id)
    {
        return set_call_status(call_id, call_status_be_talking);
    }
    //设置挂机状态
    inline HPR_INT32 set_hang_up_status(const char* call_id)
    {
        return set_call_status(call_id, call_status_hang_up);
    }
    //设置不接听，直接挂机
    inline HPR_INT32 set_no_pick_up_status(const char* call_id)
    {
        return set_call_status(call_id, call_status_decline);
    }
    //设置取消
    inline HPR_INT32 set_cancel_status(const char* call_id)
    {
        return set_call_status(call_id, call_status_cancel);
    }

    HPR_INT32 set_rcd_start_time_now(const char* call_id);
    HPR_INT32 set_rcd_stop_time_now(const char* call_id);
    HPR_INT32 set_trying_time_now(const char* call_id);

    HPR_INT32 get_called_phone_number(const char* call_id, std::string& phone_num);

protected:

    //工作线程函数,由基类负责调度
    HPR_VOID work(HPR_VOID);

private:

    HPR_INT32 set_call_status(const char* call_id, dev_call_status call_status);
    HPR_INT32 set_call_status(call_record_t& call_rcd, dev_call_status call_status);

    //检测设备是否处于呼叫状态
    HPR_INT32 check_phone_in_call(const char* phone_num, const char* call_id);

private:

    typedef std::map<std::string, call_record_t> call_map;
    //对象实例
    static call_manager_t* _instance;
    //呼叫设备列表
    call_map _call_map;
    //呼叫列表锁
    HPR_Mutex _call_map_lock;

};

#endif
