#ifndef SERVICE_SIP_XYODNEOG
#define SERVICE_SIP_XYODNEOG

#include<hpr/HPR_Socket.h>
#include<hpr/HPR_Mutex.h>
#include<hpr/HPR_Semaphore.h>
#include<hlog/logdef.h>
#include<osipparser2/osip_parser.h>
#include<list>

#include"memory_pool.h"
#include"definer.h"
#include"service_base.h"
#include"multi_thread_work.h"

class service_sip_t : public service_base_t
{
protected:
    typedef struct _sip_buffer_
    {
        char* _buf;
        HPR_INT32 _buf_len;
        HPR_ADDR_T _client_addr;
        void* _user_data;

    }sip_buffer;

public:

    service_sip_t (/* arguments*/);
    virtual ~service_sip_t ();

    virtual HPR_INT32 init();
    virtual HPR_VOID fini();

    HPR_VOID recv_msg();

    HPR_INT32 send_data(sip_buffer* buf);

    HPR_VOID free_memory_from_out(HPR_VOIDPTR buf);

private:
    /* --------------------------------------------------------------------------*/
    /**
     * @brief handle_incoming_message 处理接收到的Sip信令字符串内容
     *
     * @param buf 字符串内容
     * @param buf_len 字符串长度
     * @param buf_after_handle 处理后的字符串
     * @param buf_len_after_handle 处理后的字符串长度
     * @param client_addr 客户端地址
     *
     * @return 0成功，其他失败
     */
    /* ----------------------------------------------------------------------------*/
    HPR_INT32 handle_incoming_message(char* buf, HPR_INT32 buf_len, char** buf_after_handle, HPR_INT32* buf_len_after_handle, HPR_ADDR_T& client_addr);
    /* --------------------------------------------------------------------------*/
    /**
     * @brief proccess_message 处理已经形成的SIP消息结构体
     *
     * @param message 消息结构体
     * @param answer 应答结构体
     * @param dest_addr 目的地址
     *
     * @return 0成功，其他失败
     */
    /* ----------------------------------------------------------------------------*/
    HPR_INT32 proccess_message(osip_message_t* message, osip_message_t** answer, HPR_ADDR_T& dest_addr);
    /* --------------------------------------------------------------------------*/
    /**
     * @brief handle_register 处理注册数据
     *
     * @param message
     * @param answer
     * @param client_addr
     *
     * @return 
     */
    /* ----------------------------------------------------------------------------*/
    HPR_INT32 handle_register(osip_message_t* message, osip_message_t** answer, HPR_ADDR_T& client_addr);
    /* --------------------------------------------------------------------------*/
    /**
     * @brief handle_invite 处理INVITE请求
     *
     * @param message
     * @param answer
     * @param dest_addr
     *
     * @return 
     */
    /* ----------------------------------------------------------------------------*/
    HPR_INT32 handle_invite(osip_message_t* message, osip_message_t** answer, HPR_ADDR_T& dest_addr);
    /* --------------------------------------------------------------------------*/
    /**
     * @brief handle_ack 处理ack信息
     *
     * @param message
     * @param dest_addr
     *
     * @return 
     */
    /* ----------------------------------------------------------------------------*/
    HPR_INT32 handle_ack(osip_message_t* message, HPR_ADDR_T& dest_addr);
    /* --------------------------------------------------------------------------*/
    /**
     * @brief handle_response 处理返回信息
     *
     * @param message
     * @param dest_addr
     *
     * @return 
     */
    /* ----------------------------------------------------------------------------*/
    HPR_INT32 handle_response(osip_message_t* message, HPR_ADDR_T& dest_addr);
    /* --------------------------------------------------------------------------*/
    /**
     * @brief handle_message 处理Sip的MESSAGE消息
     *
     * @param message 消息内容
     * @param answer 应答内容
     * @param dest_addr 发送的目的地址
     *
     * @return 成功0，其他失败
     */
    /* ----------------------------------------------------------------------------*/
    HPR_INT32 handle_message(osip_message_t* message, osip_message_t** answer, HPR_ADDR_T& dest_addr);
    /* --------------------------------------------------------------------------*/
    /**
     * @brief handle_bye 处理BYE信令
     *
     * @param message
     * @param answer
     * @param dest_addr
     *
     * @return 
     */
    /* ----------------------------------------------------------------------------*/
    HPR_INT32 handle_bye(osip_message_t* message, osip_message_t** answer, HPR_ADDR_T& dest_addr);

    HPR_INT32 build_response(osip_message_t** rsp, HPR_INT32 status, osip_message_t* req);

    HPR_INT32 validate_credential(osip_proxy_authorization_t* auth);
    HPR_INT32 add_credential(osip_message_t* message, const char* realm, osip_message_t* rsp, HPR_INT32 netzone_id);

    //发送线程入口函数
    static HPR_VOIDPTR CALLBACK send_worker(HPR_VOIDPTR thread_param);
    //工作任务线程
    static HPR_VOIDPTR CALLBACK work_routine(HPR_VOIDPTR thread_param);
    static HPR_VOIDPTR CALLBACK clear_routine(HPR_VOIDPTR thread_param);
    //发送数据的函数
    HPR_VOID send_msg();

private:

    HPR_BOOL _run_status;

    HPR_SOCK_T _sock;
    HPR_HANDLE _recv_thread;

    HPR_HANDLE _send_thread;

    std::list<sip_buffer*> _send_task_list;
    HPR_Mutex _send_task_mutex;
    HPR_SEM_T _sem;


    //内存池
    memory_pool_t _memory_pool;

    //线程池
    multi_thread_work_t _thread_pool;
} /* service_sip_t */;

#endif /* end of include guard: SERVICE_SIP_XYODNEOG */
