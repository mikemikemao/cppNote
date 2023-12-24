/****************************************************************
  @filename: memory_pool_t.h
  @description: 内存池
  @warning:         
  @notes: wangfeida 2013/05/10 created
 *****************************************************************/

#ifndef __MEMORY_POOL_H__
#define __MEMORY_POOL_H__

#include<hpr/HPR_Hpr.h>
#include<hpr/HPR_Mutex.h>

class memory_pool_t
{
        
    typedef union __link__
    {
        union __link__* free_list;
        HPR_UINT8 data[1];

    }link_t;

public:
    memory_pool_t();
    virtual ~memory_pool_t();

public:

    /**
     @function:HPR_INT32  init(HPR_UINT32 block_size, HPR_UINT32 block_num)
     @brief: 初始化，分配所有内存
     @param: 
             block_size 每一个内存块的大小
             block_num  内存块的数量
     @notes:
     **/
    HPR_INT32 init(HPR_UINT32 block_size, HPR_UINT32 block_num);

    /**
     @function: HPR_VOID finit();
     @brief: 反初始化
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_VOID finit();

    /**
     @function: HPR_VOIDPTR get_memory()
     @brief: 获取一块内存
     @notes:
     **/
    HPR_VOIDPTR get_memory();

    /**
     @function: HPR_VOID free_memory(HPR_VOIDPTR mem_addr)
     @brief: 释放内存块
     @notes:
     **/
    HPR_VOID free_memory(HPR_VOIDPTR mem_addr);

    /**
     @function: HPR_VOID wait_free_all()
     @brief: 等待所有的内存都已释放
     @notes:
     **/
    HPR_VOID wait_free_all();

protected:

    /**
     @function:  HPR_BOOL check_all_free()
     @brief: 检测是否所有内存块都已释放
     @notes:
     **/
    HPR_BOOL check_all_free();

private:

    //内存指针
    HPR_UINT8* _buffer;
    //内存块大小
    HPR_UINT32 _buffer_size;
    //空闲内存块列表
    link_t* _free_list;
    //正在使用的数量
    HPR_UINT32 _used_num;
    //内存块锁
    HPR_Mutex _memory_lock;
    //是否释放
    HPR_BOOL _is_release;

};

#endif