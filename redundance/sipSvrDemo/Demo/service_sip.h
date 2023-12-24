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
     * @brief handle_incoming_message ������յ���Sip�����ַ�������
     *
     * @param buf �ַ�������
     * @param buf_len �ַ�������
     * @param buf_after_handle �������ַ���
     * @param buf_len_after_handle �������ַ�������
     * @param client_addr �ͻ��˵�ַ
     *
     * @return 0�ɹ�������ʧ��
     */
    /* ----------------------------------------------------------------------------*/
    HPR_INT32 handle_incoming_message(char* buf, HPR_INT32 buf_len, char** buf_after_handle, HPR_INT32* buf_len_after_handle, HPR_ADDR_T& client_addr);
    /* --------------------------------------------------------------------------*/
    /**
     * @brief proccess_message �����Ѿ��γɵ�SIP��Ϣ�ṹ��
     *
     * @param message ��Ϣ�ṹ��
     * @param answer Ӧ��ṹ��
     * @param dest_addr Ŀ�ĵ�ַ
     *
     * @return 0�ɹ�������ʧ��
     */
    /* ----------------------------------------------------------------------------*/
    HPR_INT32 proccess_message(osip_message_t* message, osip_message_t** answer, HPR_ADDR_T& dest_addr);
    /* --------------------------------------------------------------------------*/
    /**
     * @brief handle_register ����ע������
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
     * @brief handle_invite ����INVITE����
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
     * @brief handle_ack ����ack��Ϣ
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
     * @brief handle_response ��������Ϣ
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
     * @brief handle_message ����Sip��MESSAGE��Ϣ
     *
     * @param message ��Ϣ����
     * @param answer Ӧ������
     * @param dest_addr ���͵�Ŀ�ĵ�ַ
     *
     * @return �ɹ�0������ʧ��
     */
    /* ----------------------------------------------------------------------------*/
    HPR_INT32 handle_message(osip_message_t* message, osip_message_t** answer, HPR_ADDR_T& dest_addr);
    /* --------------------------------------------------------------------------*/
    /**
     * @brief handle_bye ����BYE����
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

    //�����߳���ں���
    static HPR_VOIDPTR CALLBACK send_worker(HPR_VOIDPTR thread_param);
    //���������߳�
    static HPR_VOIDPTR CALLBACK work_routine(HPR_VOIDPTR thread_param);
    static HPR_VOIDPTR CALLBACK clear_routine(HPR_VOIDPTR thread_param);
    //�������ݵĺ���
    HPR_VOID send_msg();

private:

    HPR_BOOL _run_status;

    HPR_SOCK_T _sock;
    HPR_HANDLE _recv_thread;

    HPR_HANDLE _send_thread;

    std::list<sip_buffer*> _send_task_list;
    HPR_Mutex _send_task_mutex;
    HPR_SEM_T _sem;


    //�ڴ��
    memory_pool_t _memory_pool;

    //�̳߳�
    multi_thread_work_t _thread_pool;
} /* service_sip_t */;

#endif /* end of include guard: SERVICE_SIP_XYODNEOG */
