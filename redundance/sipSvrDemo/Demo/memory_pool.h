/****************************************************************
  @filename: memory_pool_t.h
  @description: �ڴ��
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
     @brief: ��ʼ�������������ڴ�
     @param: 
             block_size ÿһ���ڴ��Ĵ�С
             block_num  �ڴ�������
     @notes:
     **/
    HPR_INT32 init(HPR_UINT32 block_size, HPR_UINT32 block_num);

    /**
     @function: HPR_VOID finit();
     @brief: ����ʼ��
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_VOID finit();

    /**
     @function: HPR_VOIDPTR get_memory()
     @brief: ��ȡһ���ڴ�
     @notes:
     **/
    HPR_VOIDPTR get_memory();

    /**
     @function: HPR_VOID free_memory(HPR_VOIDPTR mem_addr)
     @brief: �ͷ��ڴ��
     @notes:
     **/
    HPR_VOID free_memory(HPR_VOIDPTR mem_addr);

    /**
     @function: HPR_VOID wait_free_all()
     @brief: �ȴ����е��ڴ涼���ͷ�
     @notes:
     **/
    HPR_VOID wait_free_all();

protected:

    /**
     @function:  HPR_BOOL check_all_free()
     @brief: ����Ƿ������ڴ�鶼���ͷ�
     @notes:
     **/
    HPR_BOOL check_all_free();

private:

    //�ڴ�ָ��
    HPR_UINT8* _buffer;
    //�ڴ���С
    HPR_UINT32 _buffer_size;
    //�����ڴ���б�
    link_t* _free_list;
    //����ʹ�õ�����
    HPR_UINT32 _used_num;
    //�ڴ����
    HPR_Mutex _memory_lock;
    //�Ƿ��ͷ�
    HPR_BOOL _is_release;

};

#endif