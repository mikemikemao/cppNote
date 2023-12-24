/** @file:   call_manager.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  ���й�����Ҫ���������־���ɼ���ʱ����
             �豸״̬����������ĸ��¶��ڴ�����ɣ�����ȡ���й�����⣨����޶ȱ�֤���гɹ�������������¶��豸���и���
              ����װ�������
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

    //��ʼ��
    HPR_INT32 init();
    //����ʼ��
    HPR_INT32 finit();
    //����һ�κ���
    HPR_INT32 init_call(const char* call_id, device_t* calling_phone, device_t* called_phone,\
		HPR_INT32& error_code);
    
    //���ú���״̬
    inline HPR_INT32 set_call_faild_status(const char* call_id)
    {
        return set_call_status(call_id, call_status_faild);
    }
    //���ú��г�ʱ״̬(cancel)
    inline HPR_INT32 set_call_timeout_status(const char* call_id)
    {
        return set_call_status(call_id, call_status_no_one_answer);
    }
    //����æ��״̬
    inline HPR_INT32 set_busy_status(const char* call_id)
    {
        return set_call_status(call_id, call_status_be_talking);
    }
    //���ùһ�״̬
    inline HPR_INT32 set_hang_up_status(const char* call_id)
    {
        return set_call_status(call_id, call_status_hang_up);
    }
    //���ò�������ֱ�ӹһ�
    inline HPR_INT32 set_no_pick_up_status(const char* call_id)
    {
        return set_call_status(call_id, call_status_decline);
    }
    //����ȡ��
    inline HPR_INT32 set_cancel_status(const char* call_id)
    {
        return set_call_status(call_id, call_status_cancel);
    }

    HPR_INT32 set_rcd_start_time_now(const char* call_id);
    HPR_INT32 set_rcd_stop_time_now(const char* call_id);
    HPR_INT32 set_trying_time_now(const char* call_id);

    HPR_INT32 get_called_phone_number(const char* call_id, std::string& phone_num);

protected:

    //�����̺߳���,�ɻ��ฺ�����
    HPR_VOID work(HPR_VOID);

private:

    HPR_INT32 set_call_status(const char* call_id, dev_call_status call_status);
    HPR_INT32 set_call_status(call_record_t& call_rcd, dev_call_status call_status);

    //����豸�Ƿ��ں���״̬
    HPR_INT32 check_phone_in_call(const char* phone_num, const char* call_id);

private:

    typedef std::map<std::string, call_record_t> call_map;
    //����ʵ��
    static call_manager_t* _instance;
    //�����豸�б�
    call_map _call_map;
    //�����б���
    HPR_Mutex _call_map_lock;

};

#endif
