/** @file:   db_manager.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  ���ݿ����ģ��
 *
 *  @author: wangfeida@hikvision.com.cn
 *  @data:   2014/07/17
 *  
 *  @note:   
 *
 *  @Modification History:
 *  <version>       <time>          <author>        <desc>
 *    V1.0        2014/07/17        wangfeida       Create
 *
 *  @warning: <No waining>        
*/

#ifndef _DB_MANAGER_H_
#define _DB_MANAGER_H_

#include<hpr/HPR_Mutex.h>

#include"definer.h"
#include"db_interface.h"
#include"work_thread.h"
#include"sql_define.h"
#include"device.h"

//#define SYNC_ADDRESS_BOOK

class db_manager_t
#ifdef SYNC_ADDRESS_BOOK
    : public work_thread_t
#endif
{
private:
    db_manager_t();
    virtual ~db_manager_t();

public:

    typedef struct _system_config_
    {
        std::string _index_code;            //indexcode����ʱ��ʹ��
        std::string _manager_center_num;    //�������ĺ���
        std::string _user_name;             //���������û���
        std::string _pass_word;             //������������
        std::string _user_agent;            //SIP��������
        HPR_UINT16 _sip_port;               //SIPЭ��˿�
        HPR_UINT16 _webservice_port;        //webservice�˿�
        HPR_UINT16 _http_port;              //http�˿�
        HPR_UINT16 _net_manager_device_port;//���ܶ˿ڣ��ϱ��豸״̬
        HPR_UINT16 _net_manager_server_port;//���ܶ˿ڣ��ϱ�������״̬
        HPR_UINT32 _max_talking_time;       //���ͨ��ʱ��
        HPR_UINT32 _max_no_rsp_time;        //���δ��Ӧʱ��
        HPR_UINT32 _hb_timeout;             //������ʱʱ��

        HPR_UINT32 _valid_register_time;    //ע�����Ч��

    }local_config_t;

public:

    static HPR_INT32 create_instance();
    static HPR_VOID destroy_instance();

    inline static db_manager_t* instance()
    {
        return _instance;
    }

    //�������ݿ�
    HPR_INT32 init(sql_def_t::db_type database);
    //�ر����ݿ�
    HPR_INT32 finit();

    //��ȡͨѶ¼
#if 0
    HPR_INT32 load_addressbook();
#endif
    //�����ζ�ȡͨѶ¼
    HPR_INT32 load_addressbook(VT_PHONE_LIST& phone_list, HPR_INT32 count, HPR_INT32 offset, HPR_INT32& read_count);
    //��ȡϵͳ����
    HPR_INT32 get_local_config(db_manager_t::local_config_t& config);
    //д��ϵͳ������Ϣ
    HPR_INT32 set_local_config(db_manager_t::local_config_t& config);
    //��ȡԶ��������Ϣ
    HPR_INT32 get_remote_config(remote_config_t& config);
    //����Զ��������Ϣ
    HPR_INT32 set_remote_config(remote_config_t& config);

#ifdef SYNC_ADDRESS_BOOK
    //ɾ��һ��ͨѶ¼
    HPR_INT32 del_phone(const char* phone_no);
    //����һ��ͨѶ¼
    HPR_INT32 insert_phone(vt_phone_t& phone);
    //����һ��ͨѶ¼
    HPR_INT32 update_phone(vt_phone_t& phone);
#endif
    //����ͨѶ¼
    HPR_INT32 add_phone_sync(VT_PHONE_LIST& add_list);
	HPR_INT32 add_single_phone_sync(vt_phone_t& phone);
	HPR_INT32 add_single_phone_sync(device_t& device);
    //����ͨѶ¼
    HPR_INT32 update_phone_sync(VT_PHONE_LIST& update_list);
    //ɾ��ͨѶ¼
    HPR_INT32 del_phone_sync(std::list<std::string>& del_list);

#ifdef SYNC_ADDRESS_BOOK
    //��ȡ����ͬ��Դ�б�
    HPR_INT32 load_sync_src(ADDRBOOK_SRC_LIST& addrbook_src_list);
    //����һ��ͬ����Ϣ
    HPR_INT32 insert_sync_src(const char* indexcode, const char* ip, HPR_UINT16 port, HPR_INT32 protocol);
    //ɾ��һ��ͬ����Ϣ
    HPR_INT32 delete_sync_src(const char* indexcode);
#endif

    //дͨ����¼
    HPR_INT32 write_call_log_sync(std::list<call_record_t>& call_list);
    //��ȡͨ����¼
    HPR_INT32 read_call_log_sync(HPR_INT32 read_count, std::list<call_record_t>& call_list);
    //ͬ��ɾ��ͨ����¼
    HPR_INT32 del_call_log_sync(HPR_INT32 del_count);
    //��ȡ��־����
    HPR_INT32 get_call_log_count();

    //�����
    HPR_INT32 trancate_addrbook();

    //�޸�����
    HPR_INT32 modify_psw(const char* psw);
    //д��ע����Ϣ
    HPR_INT32 write_reg_status(std::list<register_status_t>& reg_info_list);

    char *get_local_ip_str(char *ip);
protected:

#ifdef SYNC_ADDRESS_BOOK
    //���ݿ�ˢ��
    HPR_VOID work(HPR_VOID);
#endif

private:

    //����������������
#ifdef SYNC_ADDRESS_BOOK
    HPR_INT32 push_task_to_list(const char* task);
#endif

    //��ȡ������Ϣ
    HPR_INT32 read_net_zones(std::list<net_zone_t>& net_zone_list);
    //����������Ϣ
    HPR_INT32 set_net_zones(std::list<net_zone_t>& net_zone_list);

private:

    //����ʵ��
    static db_manager_t* _instance;
    //���ݿ�����ӿ�
    db_interface_t* _db_interface;

#ifdef SYNC_ADDRESS_BOOK
    //���ݿ������б�
    std::list<std::string> _sql_list;
    //���ݿ��������
    HPR_Mutex _sql_list_lock;
    //���ݿ�ˢ��ʱ����(5���ӣ�
    static const HPR_UINT32 DB_REFRESH_TIMEOUT = 300000;
    //����������������
    static const HPR_UINT32 MAX_TASK_COUNT_IN_LIST = 10000;
    //���������̵߳���С������
    static const HPR_UINT32 EXE_IMMEDIATELY_TASK_NUM = 500;
#endif

};

#endif
