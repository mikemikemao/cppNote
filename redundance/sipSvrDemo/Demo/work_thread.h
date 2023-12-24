/** @file:   work_thread.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  工作线程类
 *
 *  @author: wangfeida@hikvision.com.cn
 *  @data:   2014/07/09
 *  
 *  @note:   定义工作线程，实际的线程函数由派生类负责实现
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

    //线程句柄
    HPR_HANDLE _work_thread;
    //退出事件句柄
    HPR_HANDLE _task_event;

    //派生类名，主要用于调试
    std::string _class_name;

    //任务执行间隔时间
    HPR_UINT32 _work_wait_time;

    //是否停止
    HPR_BOOL _exit;

public:
    //启动工作线程
    HPR_INT32 start();
    //停止工作线程，并释放资源
    HPR_VOID stop();

    //唤醒工作线程，立即开始执行任务
    inline HPR_BOOL execute_immediately()
    {
        return HPR_SetEvent(_task_event);
    }

    //挂起工作线程（超时后执行一次任务）
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
    //设置任务执行的间隔时间
    HPR_VOID set_work_wait_time(const HPR_UINT32 wait_time);

    //实际的线程函数，由派生类负责实现
    virtual HPR_VOID work(HPR_VOID) = 0;

};
#endif
