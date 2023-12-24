/** @file:   atom_mutex.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  使用原子锁实现的互斥对象
 *
 *  @author: wangfeida@hikvision.com.cn
 *  @data:   2014/08/15
 *  
 *  @note:   
 *
 *  @Modification History:
 *  <version>       <time>          <author>        <desc>
 *    V1.0        2014/08/15        wangfeida       Create
 *
 *  @warning: <No waining>        
*/

#ifndef _ATOM_MUTEX_H_
#define _ATOM_MUTEX_H_

#include<hpr/HPR_Atomic.h>
#include<hpr/HPR_Thread.h>
#include<hpr/HPR_Utils.h>

class atom_mutex_t
{
public:

    atom_mutex_t()
        : _atom(0)
        , _thread_id(HPR_INVALID_HANDLE)
        , _enter_count(0)
    {

    }

    virtual ~atom_mutex_t(){_thread_id = HPR_INVALID_HANDLE;}

public:

    HPR_BOOL try_enter()
    {
        HPR_HANDLE thread_id = HPR_Thread_GetSelfId();
        if(HPR_AtomicCas(&_atom, 1, 0) == 0)
        {
            //获得锁，设置线程id
            _thread_id = thread_id;
            ++_enter_count;
            return HPR_TRUE;
        }
        else if(_thread_id == thread_id)
        {
            //没有获得锁，判断获得锁的线程与当前线程是否为同一线程
            ++_enter_count;
            return HPR_TRUE;
        }
        else
        {
            return HPR_FALSE;
        }
    }

    HPR_VOID enter(HPR_UINT32 spin_count)
    {
        while (--spin_count > 0)
        {
            if (try_enter() == HPR_TRUE)
            {
                return;
            }
        }

        //在spin_count内未获得锁
        /*lint -save -e506*/
        do 
        {
            HPR_Sleep(2);

            if (try_enter() == HPR_TRUE)
            {
                return;
            }

        } while (1);
        /*lint -restore*/
    }

    HPR_VOID leave()
    {
        if (--_enter_count == 0)
        {
            _thread_id = HPR_INVALID_HANDLE;
            HPR_AtomicDec(&_atom);
        }
       
        return;
    }

private:

    //原子变量，为0代表没有在使用，为1代表被被使用
    HPR_ATOMIC_T _atom;
    //获得锁的线程id
    HPR_HANDLE _thread_id;
    //当前线程获得锁的次数
    HPR_UINT32 _enter_count;

};

#endif