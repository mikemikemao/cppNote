#ifndef MULTI_THREAD_WORK_2PR0B0BM
#define MULTI_THREAD_WORK_2PR0B0BM

#include<list>
#include<hpr/HPR_Hpr.h>
#include<hpr/HPR_Semaphore.h>
#include<hpr/HPR_Thread.h>
#include <hpr/HPR_Mutex.h>

#define MAX_THREAD_NUM  128

typedef HPR_VOIDPTR (CALLBACK *WORKROUTINE)(HPR_VOIDPTR);
typedef HPR_VOIDPTR (CALLBACK *CLEARROUTINE)(HPR_VOIDPTR);
typedef struct work_data
{
    work_data()
    {
        work_info = NULL;
        work_route = NULL;
        clear_route = NULL;
    }
    HPR_VOIDPTR work_info;
    WORKROUTINE work_route;
    CLEARROUTINE clear_route;
}work_data_t;

class multi_thread_work_t
{
public:
    multi_thread_work_t (/* arguments*/);
    virtual ~multi_thread_work_t ();

    virtual HPR_INT32 init(HPR_INT32 thread_num);
    virtual HPR_INT32 fini();

    HPR_VOID push_work_list(work_data_t& data);
    HPR_BOOL pop_work_list(work_data_t& data);
    HPR_VOID clear_work_list();

    HPR_VOID work_work();

private:
    HPR_VOID add_work(work_data_t& data);
    HPR_BOOL get_work(work_data_t& data);

    //是否退出
    HPR_BOOL _quit;

    //待处理队列
    std::list<work_data_t> _work_list;
    //list之锁
    HPR_Mutex _list_lock;
    //list信号量
    HPR_SEM_T _sem;

    //线程信息数组指针
    HPR_HANDLE _threads[MAX_THREAD_NUM];
    //线程数
    HPR_INT32 _thread_num;
} /* multi_thread_work_t */;

#endif /* end of include guard: MULTI_THREAD_WORK_2PR0B0BM */
