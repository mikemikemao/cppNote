#ifndef NET_MANAGER_7OYYREHG
#define NET_MANAGER_7OYYREHG

#include<hpr/HPR_Hpr.h>
#include<hpr/HPR_Atomic.h>
#include<hpr/HPR_Socket.h>
#include<hlog/logdef.h>
#include<snmp/NetManagerAgent.h>

#include"multi_thread_work.h"
#include"memory_pool.h"
#include"net_manage_parser.h"

typedef struct _task_buffer_
{
    _task_buffer_ ()
    {
        _buf = NULL;
        _buf_len = 0;
        _user_data = NULL;
        memset(&_client_addr, 0, sizeof(_client_addr));
    }
    char* _buf;
    HPR_INT32 _buf_len;
    void* _user_data;
    HPR_ADDR_T _client_addr;
}task_buffer;

class net_manager_t : public ISIP_Collector
{
private:
    net_manager_t (HPR_UINT16 net_manage_port);
    virtual ~net_manager_t ();

public:
    static net_manager_t* instance(HPR_UINT16 port)
    {
        if (_instance == NULL)
        {
            _instance = new(std::nothrow) net_manager_t(port);
            if (_instance == NULL)
            {
                LOG_ERROR("There is no memory.");
                return NULL;
            }
        }

        return _instance;
    }

    static net_manager_t* instance()
    {
        return _instance;
    }

    static HPR_VOID del_instance()
    {
        if (_instance != NULL)
        {
            delete _instance;
            _instance = NULL;
        }
    }

    HPR_INT32 init();
    HPR_VOID fini();

    HPR_VOID do_recv();

    HPR_VOID handle_message(std::string& msg, HPR_ADDR_T& addr);

    HPR_VOID on_register(HPR_ADDR_T& addr);
    HPR_VOID on_unregister();
    HPR_BOOL set_getall_status();
    HPR_VOID set_idle_status();
    HPR_BOOL is_register();
    HPR_BOOL is_idle();

    HPR_INT32 send_changed_device_status();

    HPR_VOID free_memory_from_out(HPR_VOIDPTR buf);

    HPR_VOID add_changed_device(std::string no, NET_MANAGE_DEVICE_INFO_T& info);
    HPR_VOID clear_changed_devices();

    int GetWorkState()
    {
        return 0;
    }
private:
    static net_manager_t* _instance;

    HPR_UINT16 _net_manage_port;        //网管发送数据端口
    HPR_SOCK_T _sock;                   //socket

    HPR_ADDR_T _net_manage_addr;          //网管服务器地址

    HPR_ATOMIC_T _status;               //网管状态
    HPR_Mutex _status_lock;             //网管状态锁

    HPR_BOOL _run_status;               //该网管模块运行状态

    //内存池
    memory_pool_t _memory_pool;

    //线程池
    multi_thread_work_t _thread_pool;

    //Timer
    HPR_INT32 _timer;

    //变更状态map
    std::map<std::string,NET_MANAGE_DEVICE_INFO_T> _changed_devices;
    //map锁
    HPR_Mutex _changed_devices_lock;
} /* net_manager_t */;

#endif /* end of include guard: NET_MANAGER_7OYYREHG */
