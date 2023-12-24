#include"memory_pool.h"
#include<hpr/HPR_Guard.h>
#include<hpr/HPR_Utils.h>

memory_pool_t::memory_pool_t()
: _buffer(NULL)
, _buffer_size(0)
, _free_list(NULL)
, _used_num(0)
, _is_release(HPR_TRUE)
{

}

memory_pool_t::~memory_pool_t()
{
    _buffer = NULL;
    _free_list = NULL;
}

HPR_INT32 memory_pool_t::init(HPR_UINT32 block_size, HPR_UINT32 block_num)
{
    //设置头指针
    HPR_Guard lock(&_memory_lock);
    if (_buffer != NULL)
    {
        return 0;
    }
#if (defined(_WIN32) || defined(_WIN64))
    _buffer = (HPR_UINT8*)::VirtualAlloc(NULL, block_size*block_num, MEM_COMMIT, PAGE_READWRITE);
#else
    //_buffer = (HPR_UINT8*)vmalloc(block_size*block_num);
    _buffer = (HPR_UINT8*)malloc(block_size*block_num);
#endif

    if (_buffer == NULL)
    {
        return -1;
    }

    _buffer_size = block_size*block_num;
    _free_list = (link_t*)_buffer;

    link_t* link_tmp = _free_list;

    //对前block_num -1个内存块进行设置
    for (HPR_UINT32 i = 0; i < block_num - 1; ++i)
    {
        link_tmp->free_list = (link_t*)(link_tmp->data + block_size);
        link_tmp = link_tmp->free_list;
    }

    //设置最后一个内存块，指向空指针
    if(link_tmp != NULL)
    {
        link_tmp->free_list = 0;
    }

    _is_release = HPR_FALSE;

    return 0;
}

HPR_VOID memory_pool_t::finit()
{
    wait_free_all();

    HPR_Guard lock(&_memory_lock);
    if (_buffer != NULL)
    {
#if (defined(_WIN32) || defined(_WIN64))
        ::VirtualFree(_buffer, _buffer_size, MEM_RELEASE);
#else
        //_buffer = (HPR_UINT8*)vfree(_buffer);
        free(_buffer);
#endif
        _buffer = NULL;
    }

    _buffer_size = 0;

    return;
}

HPR_VOID memory_pool_t::wait_free_all()
{
    _is_release = HPR_TRUE;

    while (1)
    {
        if (check_all_free() == HPR_TRUE)
        {
            break;
        }
        else
        {
            HPR_Sleep(300);
        }
    }

    return;
}

HPR_VOIDPTR memory_pool_t::get_memory()
{
    HPR_Guard lock(&_memory_lock);
    if(_is_release == HPR_TRUE)
    {
        return NULL;
    }

    if (_free_list == NULL)
    {
        return NULL;
    }
    else
    {
        //取链表的第一个元素
        HPR_UINT8* ret_memory = _free_list->data;
        _free_list = _free_list->free_list;
        ++_used_num;
        return ret_memory;
    }
}

HPR_VOID memory_pool_t::free_memory(HPR_VOIDPTR mem_addr)
{
    HPR_Guard lock(&_memory_lock);
    //放在链表头部
    ((link_t*)mem_addr)->free_list = _free_list;
    _free_list = (link_t*)mem_addr;
    --_used_num;

    return;
}

HPR_BOOL memory_pool_t::check_all_free()
{
    HPR_Guard lock(&_memory_lock);
    return (_used_num == 0 ? HPR_TRUE : HPR_FALSE);
}
