#include<hlog/logdef.h>
#include<hpr/HPR_Utils.h>
#include <hpr/HPR_Guard.h>

#include"multi_thread_work.h"

HPR_VOIDPTR CALLBACK thread_callback(HPR_VOIDPTR param)
{
    do 
    {
        multi_thread_work_t* p = static_cast<multi_thread_work_t*>(param);
        if (p == NULL)
        {
            SIP_SVR_ERROR("thread_callback param err.");
            break;
        }

        p->work_work();
    } while (0);

    return NULL;
}

/*lint -save -e1401*/
multi_thread_work_t::multi_thread_work_t( /* arguments*/ )
: _quit(HPR_TRUE)
, _thread_num(0)
{
    for (int i=0; i<MAX_THREAD_NUM; i++)
    {
        _threads[i] = HPR_INVALID_THREAD;
    }
}
/*lint -restore*/

/*lint -save -e1540*/
multi_thread_work_t::~multi_thread_work_t()
{

}
/*lint -restore*/

HPR_INT32 multi_thread_work_t::init( HPR_INT32 thread_num )
{
    if (thread_num <= 0)
    {
        _thread_num = 2*HPR_GetCPUNumber() + 2;
    }
    else if (thread_num > MAX_THREAD_NUM)
    {
        _thread_num = MAX_THREAD_NUM;
    }
    else
    {
        _thread_num = thread_num;
    }

    HPR_INT32 rt = HPR_ERROR;

    do 
    {
        if (-1 == HPR_SemCreate(&_sem, 0) )
        {
            SIP_SVR_ERROR("HPR_SemCreate failed.");
            break;
        }

        _quit = HPR_FALSE;
        rt = HPR_OK;

        for (int i=0; i<_thread_num; i++)
        {
            _threads[i] = HPR_Thread_Create(thread_callback, this, 0);
            if (_threads[i] == HPR_INVALID_THREAD)
            {
                SIP_SVR_ERROR("HPR_Thread_Create failed.");
                rt = HPR_ERROR;
                break;
            }
        }
    } while (0);

    if (rt == HPR_ERROR)
    {
        fini();
    }

    return rt;
}

HPR_INT32 multi_thread_work_t::fini()
{
    _quit = HPR_TRUE;

    for (int i=0; i<_thread_num; i++)
    {
        HPR_SemPost(&_sem);
    }

    for (int i=0; i<_thread_num; i++)
    {
        if (_threads[i] != HPR_INVALID_THREAD )
        {
            HPR_Thread_Wait(_threads[i]);
            _threads[i] = HPR_INVALID_THREAD;
        }
    }

    clear_work_list();

    HPR_SemDestroy(&_sem);

    return HPR_OK;
}

HPR_VOID multi_thread_work_t::push_work_list( work_data_t& data )
{
    add_work(data);
    HPR_SemPost(&_sem);
}

HPR_BOOL multi_thread_work_t::pop_work_list( work_data_t& data )
{
    HPR_SemWait(&_sem);
    return get_work(data);
}

HPR_VOID multi_thread_work_t::clear_work_list()
{
    work_data_t data;
    HPR_Guard lock(&_list_lock);
    while (!_work_list.empty())
    {
        data = _work_list.front();
        _work_list.pop_front();
        data.clear_route(data.work_info);
    }
}

HPR_VOID multi_thread_work_t::work_work()
{
    work_data_t w_data;
    while (_quit == HPR_FALSE)
    {
        if (HPR_TRUE == pop_work_list(w_data) )
        {
            if (w_data.work_route != NULL)
            {
                w_data.work_route(w_data.work_info);
            }
        }
    }
}

HPR_VOID multi_thread_work_t::add_work( work_data_t& data )
{
    HPR_Guard lock(&_list_lock);
    _work_list.push_back(data);
}

HPR_BOOL multi_thread_work_t::get_work( work_data_t& data )
{
    HPR_Guard lock(&_list_lock);
    HPR_BOOL have_data = HPR_FALSE;
    if (!_work_list.empty())
    {
        data = _work_list.front();
        _work_list.pop_front();
        have_data =  HPR_TRUE;
    }
    return have_data;
}

