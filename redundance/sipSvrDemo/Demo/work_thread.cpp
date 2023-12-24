#include<hpr/HPR_Thread.h>
#include<hpr/HPR_Error.h>
#include <hpr/HPR_Utils.h>
#include<hlog/logdef.h>

#include"work_thread.h"

work_thread_t::work_thread_t(const char* class_name, HPR_UINT32 wait_time)
: _work_thread(HPR_INVALID_THREAD)
, _task_event(HPR_INVALID_HANDLE)
, _class_name(class_name)
, _work_wait_time(wait_time*1000)
, _exit(HPR_TRUE)
{
}

work_thread_t::~work_thread_t()
{
    try
    {
        stop();
    }
    catch(...)
    {
    }
}

HPR_INT32 work_thread_t::start()
{
    HPR_INT32 ret = -1;

    do 
    {
        if (_work_thread != HPR_INVALID_THREAD)
        {
            ret = 0;
            SIP_SVR_WARN("[%s]Work thread alreay start!", _class_name.c_str());
            break;
        }

        _exit = HPR_FALSE;

        _task_event = HPR_CreateEvent(HPR_FALSE);
        if (_task_event == HPR_INVALID_HANDLE)
        {
            SIP_SVR_ERROR("[%s]Create exit event faild!Error is %d.", _class_name.c_str(), HPR_GetSystemLastError());
            break;
        }

        _work_thread = HPR_Thread_Create(work_thread, this, 0);
        if (_work_thread == HPR_INVALID_THREAD)
        {
            SIP_SVR_ERROR("[%s]Create work thread faild!Error is %d.", _class_name.c_str(), HPR_GetSystemLastError());
            break;
        }

        ret = 0;

        SIP_SVR_INFO("[%s] start sucess!", _class_name.c_str());

    } while (0);

    if (ret != 0)
    {
        stop();
    }

    return ret;
}

HPR_VOID work_thread_t::stop()
{
    _exit = HPR_TRUE;

    if (_task_event != HPR_INVALID_HANDLE)
    {
        HPR_SetEvent(_task_event);
    }

    if (_work_thread != HPR_INVALID_THREAD)
    {
        HPR_Thread_Wait(_work_thread);
        _work_thread = HPR_INVALID_THREAD;
    }

    if (_task_event != HPR_INVALID_HANDLE)
    {
        HPR_CloseEvent(_task_event);
        _task_event = HPR_INVALID_HANDLE;
    }

    return;
}

HPR_VOID work_thread_t::work_routine(HPR_VOID)
{
    SIP_SVR_INFO("Start %s routine!", _class_name.c_str());

    //线程启动后，先执行一次任务
    HPR_UINT32 wait_ret = HPR_WAIT_OBJECT_0;

    while( wait_ret == HPR_WAIT_TIMEOUT || wait_ret == HPR_WAIT_OBJECT_0 )
    {
        //当等待成功或超时时，执行一次
        wait_ret = HPR_WaitForSingleObject(_task_event, _work_wait_time);

        if (_exit == HPR_TRUE)
        {
            break;
        }

        //由于sip服务器启动后，2-5分钟内，所有设备会重新注册，因此，所有任务放在一次超时后再执行
        work();
    }


    SIP_SVR_INFO("%s routine exit!", _class_name.c_str());

    return;
}

HPR_VOID work_thread_t::set_work_wait_time(const HPR_UINT32 wait_time)
{
    _work_wait_time = wait_time*1000;
    return;
}