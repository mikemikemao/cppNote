/** @file:   work_thread.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  �����߳���
 *
 *  @author: wangfeida@hikvision.com.cn
 *  @data:   2014/07/09
 *  
 *  @note:   ���幤���̣߳�ʵ�ʵ��̺߳����������ฺ��ʵ��
 *
 *  @Modification History:
 *  <version>       <time>          <author>        <desc>
 *    V1.0        2014/07/09        wangfeida       Create
 *
 *  @warning: <No waining>        
*/

#ifndef _WORK_THREAD_H_
#define _WORK_THREAD_H_

#include<string>

#include<hpr/HPR_Hpr.h>
#include<hpr/HPR_Event.h>

class work_thread_t
{
public:
    work_thread_t(const char* class_name, HPR_UINT32 wait_time);
    virtual ~work_thread_t();

protected:

    //�߳̾��
    HPR_HANDLE _work_thread;
    //�˳��¼����
    HPR_HANDLE _task_event;

    //������������Ҫ���ڵ���
    std::string _class_name;

    //����ִ�м��ʱ��
    HPR_UINT32 _work_wait_time;

    //�Ƿ�ֹͣ
    HPR_BOOL _exit;

public:
    //���������߳�
    HPR_INT32 start();
    //ֹͣ�����̣߳����ͷ���Դ
    HPR_VOID stop();

    //���ѹ����̣߳�������ʼִ������
    inline HPR_BOOL execute_immediately()
    {
        return HPR_SetEvent(_task_event);
    }

    //�������̣߳���ʱ��ִ��һ������
    inline HPR_BOOL execute_timeout()
    {
        return HPR_ResetEvent(_task_event);
    }

protected:

    static HPR_VOIDPTR CALLBACK work_thread(HPR_VOIDPTR work_param)
    {
        work_thread_t* _this = static_cast<work_thread_t*>(work_param);
        if (_this != NULL)
        {
            _this->work_routine();
        }

        return NULL;
    }

    HPR_VOID work_routine(HPR_VOID);
    //��������ִ�еļ��ʱ��
    HPR_VOID set_work_wait_time(const HPR_UINT32 wait_time);

    //ʵ�ʵ��̺߳������������ฺ��ʵ��
    virtual HPR_VOID work(HPR_VOID) = 0;

};
#endif
