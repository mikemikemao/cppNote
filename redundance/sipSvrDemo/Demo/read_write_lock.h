/****************************************************************
  @filename: read_write_lock_t.h
  @description: ��д��
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

    ///>>����
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

    ///>>д��
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
    //���ű�
    HANDLE _read_semaphore;
    //д�¼�
    HANDLE _write_event;
    //���ü����ٽ���
    CRITICAL_SECTION _sec_counter;
#else
    //���ű꣬hpr�е��ź�ֻ��һ���ͷ�һ���ź�
    int _read_semaphore;
    //д�¼�, HPR�����Զ������¼���ʹ���ź����
    sem_t _write_event;
    //���ü�����
    HPR_MUTEX_T _sec_counter;

#endif

    //��ǰ�Ķ�д����
    int _rw_num;    //��ֵ����д��0�����޶�д�����������

    //����дʱ���ȴ���������
    unsigned int _waiting_read_num;

    //���ڶ�ʱ���ȴ�д������
    unsigned int _waiting_write_num;

    //��ǰд�̵߳�id��0����û���߳���д
    unsigned long _writing_thread;

};

#endif
