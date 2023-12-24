/** @file:   atomic_64.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  64λԭ��������
 *
 *  @author: wangfeida@hikvision.com.cn
 *  @data:   2014/07/11
 *  
 *  @note:   ֻ֧��gcc 4.1.0�Ժ�İ汾��1��2��4��8�ֽڵ����ݽṹ
 *
 *  @Modification History:
 *  <version>       <time>          <author>        <desc>
 *    V1.0        2014/07/11        wangfeida       Create
 *
 *  @warning: <No waining>        
*/

#ifndef _ATOMIC_64_H_
#define _ATOMIC_64_H_

#include<hpr/HPR_Hpr.h>

class atomic64_t
{
public:
    atomic64_t(){}
    virtual ~atomic64_t(){}
public:

    inline static HPR_INT64 atomic64_add(HPR_INT64* src, HPR_INT64 add_value)    
    {
#if (defined(_WIN32) || defined(_WIN64))
        return ::InterlockedExchangeAdd64(src, add_value);
#else
        return __sync_fetch_and_add(src, add_value); 
#endif
    }

    inline static HPR_INT64 atomic64_sub(HPR_INT64* src, HPR_INT64 sub_value)
    {
#if (defined(_WIN32) || defined(_WIN64))
        return ::InterlockedExchangeAdd64(src, -sub_value);
#else
        return __sync_fetch_and_add(src, -sub_value); 
#endif
    }

    inline static HPR_INT64 atomic64_inc(HPR_INT64* src)
    {
#if (defined(_WIN32) || defined(_WIN64))
        return ::InterlockedExchangeAdd64(src, 1);
#else
        return __sync_fetch_and_add(src, 1); 
#endif
    }

    inline static HPR_INT64 atomic_dec(HPR_INT64* src)
    {
#if (defined(_WIN32) || defined(_WIN64))
        return ::InterlockedExchangeAdd64(src, -1);
#else
        return __sync_fetch_and_add(src, -1); 
#endif
    }

    inline static HPR_INT64 atomic64_set(HPR_INT64* src, HPR_INT64 set_value)
    {

#if (defined(_WIN32) || defined(_WIN64))
        return ::InterlockedExchange64(src, set_value);
#else
        //return __sync_val_compare_and_swap(src, *src, set_value);  //����������??
        return __sync_lock_test_and_set(src, set_value);
#endif
    }

    inline static HPR_INT64 atomic64_get(HPR_INT64* src)
    {
#if (defined(_WIN32) || defined(_WIN64))
        return ::InterlockedExchangeAdd64(src, 0);
#else
        return __sync_fetch_and_add(src, 0);
#endif
    }

};

#endif
