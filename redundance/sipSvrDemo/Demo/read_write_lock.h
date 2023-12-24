/****************************************************************
  @filename: read_write_lock_t.h
  @description: 读写锁
  @warning:         
  @notes: wangfeida 2013/05/21 created
 *****************************************************************/

#ifndef __READ_WRITE_LOCK_H__
#define __READ_WRITE_LOCK_H__

#include<hpr/HPR_Hpr.h>

#if (defined(__linux__))
    #include<sys/sem.h>
    #include<semaphore.h>
    #include<pthread.h>
    #include<hpr/HPR_Mutex.h>

    union semun
    {
        int val;
        struct semid_ds *buf;
        ushort* seminfo;
        void* __pad;
    };
#endif


class read_write_lock_t
{
public:
    read_write_lock_t(void);
    virtual ~read_write_lock_t(void);

public:

    void enter_read();

    void leave_read();

    void enter_write();

    void leave_write();


public:

    ///>>读锁
    class read_lock_t
    {
    public:

        read_lock_t(read_write_lock_t* rw_lock)
        {
            m_lock = rw_lock;
            m_lock->enter_read();
        }

        ~read_lock_t()
        {
            try
            {
                m_lock->leave_read();
            }
            catch(...)
            {

            }
        }

    private:

        read_write_lock_t* m_lock;
    };

    ///>>写锁
    class write_lock_t
    {
    public:

        write_lock_t(read_write_lock_t* rw_lock)
        {
            m_lock = rw_lock;
            m_lock->enter_write();
        }

        ~write_lock_t()
        {
            try
            {
                m_lock->leave_write();
            }
            catch(...)
            {

            }
        }

        read_write_lock_t* m_lock;
    };

private:

#if (defined(_WIN32) || defined(_WIN64))
    //读信标
    HANDLE _read_semaphore;
    //写事件
    HANDLE _write_event;
    //引用计数临界区
    CRITICAL_SECTION _sec_counter;
#else
    //读信标，hpr中的信号只能一次释放一个信号
    int _read_semaphore;
    //写事件, HPR中无自动重置事件，使用信号替代
    sem_t _write_event;
    //引用计数锁
    HPR_MUTEX_T _sec_counter;

#endif

    //当前的读写数量
    int _rw_num;    //负值代表写，0代表无读写，正数代表读

    //正在写时，等待读的数量
    unsigned int _waiting_read_num;

    //正在读时，等待写的数量
    unsigned int _waiting_write_num;

    //当前写线程的id，0代表没有线程在写
    unsigned long _writing_thread;

};

#endif
