#include"read_write_lock.h"
#include <hpr/HPR_Utils.h>

read_write_lock_t::read_write_lock_t(void)
: _rw_num(0)
, _waiting_read_num(0)
, _waiting_write_num(0)
, _writing_thread(0)
{
#if (defined(_WIN32) || defined(_WIN64))
    _read_semaphore = ::CreateSemaphore(NULL, 0, 1024, NULL);
    _write_event = ::CreateEvent(NULL, HPR_FALSE, HPR_FALSE, NULL);
    ::InitializeCriticalSectionAndSpinCount(&_sec_counter, 4000);
#else
    //初始化信号
    sem_init(&_write_event, 0, 0);
    //创建读信号量
    _read_semaphore = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    union semun sem_union;
    sem_union.val = 0;
    semctl(_read_semaphore, 0, SETVAL, sem_union);
    HPR_MutexCreate(&_sec_counter);
#endif
}

read_write_lock_t::~read_write_lock_t(void)
{
#if (defined(_WIN32) || defined(_WIN64))
    ::DeleteCriticalSection(&_sec_counter);
    ::CloseHandle(_write_event);
    ::CloseHandle(_read_semaphore);
#else
    sem_destroy(&_write_event);
    union semun sem_union;
    sem_union.val = 0;
    semctl(_read_semaphore, 0, IPC_RMID, sem_union);
    HPR_MutexDestroy(&_sec_counter);
#endif
}

void read_write_lock_t::enter_read()
{
#if (defined(_WIN32) || defined(_WIN64))
    ::EnterCriticalSection(&_sec_counter);
#else
    HPR_MutexLock(&_sec_counter);
#endif
    //是否有读或写请求正在执行
    bool is_writing = ((_rw_num < 0) || (_waiting_write_num > 0));

    if (is_writing == true)
    {
        //增加等待读的数量
        ++_waiting_read_num;
    }
    else
    {
        //没有正在写的，则更新读状态
        ++_rw_num;
    }

#if (defined(_WIN32) || defined(_WIN64))
    ::LeaveCriticalSection(&_sec_counter);
#else
    HPR_MutexUnlock(&_sec_counter);
#endif

    if (is_writing == true)
    {
        //如果正在写，则等待
#if (defined(_WIN32) || defined(_WIN64))
        ::WaitForSingleObject(_read_semaphore, INFINITE);
#else
        struct sembuf sem_b;
        sem_b.sem_num = 0;
        sem_b.sem_op = -1;
        sem_b.sem_flg = SEM_UNDO;

        semop(_read_semaphore, &sem_b, 1);
#endif
    }
}

void read_write_lock_t::leave_read()
{
#if (defined(_WIN32) || defined(_WIN64))
    ::EnterCriticalSection(&_sec_counter);
#else
    HPR_MutexLock(&_sec_counter);
#endif

    --_rw_num;

    if(_rw_num == 0)
    {
        if (_waiting_write_num > 0)
        {
            //如果有等待写的请求，唤醒
#if (defined(_WIN32) || defined(_WIN64))
            ::SetEvent(_write_event);
#else
            sem_post(&_write_event);
#endif

            //设置写状态
            --_rw_num;

            //等待写计数-1
            --_waiting_write_num;
        }
    }

#if (defined(_WIN32) || defined(_WIN64))
    ::LeaveCriticalSection(&_sec_counter);
#else
    HPR_MutexUnlock(&_sec_counter);
#endif
}

void read_write_lock_t::enter_write()
{
    //是否正在读写
    bool is_readwrite;
#if (defined(_WIN32) || defined(_WIN64))
    ::EnterCriticalSection(&_sec_counter);
#else
    HPR_MutexLock(&_sec_counter);
#endif

#if (defined(_WIN32) || defined(_WIN64))
    if ((_rw_num < 0 && (GetCurrentThreadId() == _writing_thread)) || (_rw_num == 0))
#else
    if ((_rw_num < 0 && (pthread_self() == _writing_thread)) || (_rw_num == 0))
#endif
    {
        is_readwrite = false;
    }
    else
    {
        is_readwrite = true;
    }

    if (is_readwrite == true)
    {
        //如果正在读写,增加等待写的数量
        ++_waiting_write_num;
    }
    else
    {
        if (_rw_num == 0)
        {
            //线程第一次写，记录线程id
#if (defined(_WIN32) || defined(_WIN64))
            _writing_thread = GetCurrentThreadId();
#else
        _writing_thread = pthread_self();
#endif

        }

        //没有读写或当前线程在写，则设置写状态
        --_rw_num;
    }

#if (defined(_WIN32) || defined(_WIN64))
    ::LeaveCriticalSection(&_sec_counter);
#else
    HPR_MutexUnlock(&_sec_counter);
#endif

    if (is_readwrite)
    {
#if (defined(_WIN32) || defined(_WIN64))
        ::WaitForSingleObject(_write_event, INFINITE);
        //线程第一次进入写，设置线程信息
        _writing_thread = GetCurrentThreadId();
#else
        sem_wait(&_write_event);
        _writing_thread = pthread_self();
#endif
    }
}

void read_write_lock_t::leave_write()
{
#if (defined(_WIN32) || defined(_WIN64))
    ::EnterCriticalSection(&_sec_counter);
#else
    HPR_MutexLock(&_sec_counter);
#endif

    ++_rw_num;

    if(_rw_num == 0)
    {
        //先把线程ID设置为无效
        _writing_thread = 0;

        if (_waiting_write_num > 0)
        {
            //有等待写的请求
#if (defined(_WIN32) || defined(_WIN64))
            ::SetEvent(_write_event);
#else
            sem_post(&_write_event);
#endif
            --_waiting_write_num;
            //标志写状态
            --_rw_num;
        }
        else if(_waiting_read_num > 0)
        {
            _rw_num = _waiting_read_num;
            _waiting_read_num = 0;
#if (defined(_WIN32) || defined(_WIN64))
            ::ReleaseSemaphore(_read_semaphore, _rw_num, NULL);
#else
            struct sembuf sem_b;
            sem_b.sem_num = 0;
            //由于线程数量有限，因此，不可能超过short所能表示的大小
            sem_b.sem_op = (short)_rw_num;
            sem_b.sem_flg = SEM_UNDO;

            semop(_read_semaphore, &sem_b, 1);      
#endif
        }
    }

#if (defined(_WIN32) || defined(_WIN64))
    ::LeaveCriticalSection(&_sec_counter);
#else
    HPR_MutexUnlock(&_sec_counter);
#endif
}
