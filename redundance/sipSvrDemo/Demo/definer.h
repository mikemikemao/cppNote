/**
 * @file definer.h
 * @brief �ṹ�嶨��
 * @author zhanghaifengyf1@hikvision.com.cn
 * @version 0.0.1
 * @date 2014-07-08 16:26:41
 */

#ifndef DEFINER_KB13XD3G
#define DEFINER_KB13XD3G

#include<list>
#include<string>

#include"config.h"

enum service_type
{
    DEFALT_SERVICE,
    SIP_SERVICE
};

#define UAS_STATUS_100 100  //trying
#define UAS_STATUS_200 200  //�ɹ���Ӧ
#define UAS_STATUS_300 300  //ת��
#define UAS_STATUS_400 400  //ʧ����Ӧ
#define UAS_STATUS_401 401  //��Ҫ��Ȩ
#define UAS_STATUS_403 403  //ע��ܾ���ƽ̨��ַ���벻��ȷ
#define UAS_STATUS_404 404  //��ַ�Ҳ���
#define UAS_STATUS_406 406  //�ܾ�����
#define UAS_STATUS_603 603  //�ܾ�����
#define UAS_STATUS_407 407  //�ϼ�ƽ̨��ǰ�����������Ժ���ע��(ʱ����20~40��)
#define UAS_STATUS_486 486  //������æ��Ŀ���豸������
#define UAS_STATUS_480 480  //ý��Э��ʧ�ܡ���Ӧ�������ӡ�Warning��ͷ������ʧ�ܵ�ԭ��
#define UAS_STATUS_488 488  //ý��Э��ʧ�ܡ���Ӧ�������ӡ�Warning��ͷ������ʧ�ܵ�ԭ��
#define UAS_STATUS_500 500  //��Ϊ�ڲ��߼����󣬲��ܴ�������
#define UAS_STATUS_503 503  //��������Դ����


enum dev_call_status
{
    call_status_service_stop = 0,       //��������������쳣ͨ��������ʱ��Ϊ�������ʱ��
    call_status_recv_call,              //���յ���������
    call_status_cancel,                 //ȡ������
    call_status_be_talking,             //����ͨ����
    call_status_decline,                //�ܾ�����
    call_status_no_one_answer,          //���˽�����ʱ
    call_status_hang_up,                //����ͨ�������һ�
    call_status_max_talking,            //û�йһ����������ʱ��
    call_status_faild                   //����ʧ��
};

#define SIP_MESSAGE_BUF_LEN 8191    //sip���������ճ���
#define NETMANAGE_MESSAGE_BUF_LEN 8191    //sip���������ճ���

#define MSG_HEADER  "header"
#define MSG_METHOD  "method"

#define MSG_HEART_BEAT  1
#define MSG_ALARM       6

#define NUM_TO_REPLACE  6 //�������ڻ�Ѱ���ſڻ�ʱ��Ҫ�滻��00000�����ַ�λ��
#define FLOOR_FIGURE    5 //��ŷ���ŵ�λ��
#define OUTDOOR_FLAG    "00000" //�ſڻ���־

//ͨѶ¼��Ϣ
typedef struct _vt_phone_info_
{
    std::string _phone_no;  //����
    std::string _phone_name;
    //char _sip_indexcode[config_t::MAX_SIP_INDEX_CODE]; //sip������indexcode
    HPR_INT32 _dev_main_type;                  //������
    HPR_INT32 _dev_sub_type;                   //������
    HPR_INT32 _call_priority;                  //�������ȼ�
    HPR_INT32 _net_zone_id;                    //����id

    //����ע����Ϣ
    std::string _register_ip;                  //ע��ʱ��ip��ַ
    HPR_UINT32  _register_port;
    HPR_UINT64  _last_reg_time;               //����һ�ε�ע��ʱ��

    _vt_phone_info_()
        : _dev_main_type(0)
        , _dev_sub_type(0)
        , _call_priority(0)
        , _net_zone_id(-1)
        , _register_port(0)
        , _last_reg_time(0)
    {
    }

    _vt_phone_info_& operator = (const _vt_phone_info_& phone)
    {
        if (this != &phone)
        {
            _phone_no = phone._phone_no;
            _phone_name = phone._phone_name;
            _dev_main_type = phone._dev_main_type;
            _dev_sub_type = phone._dev_sub_type;
            _call_priority = phone._call_priority;
            _net_zone_id = phone._net_zone_id;
            _register_ip = phone._register_ip;
            _register_port = phone._register_port;
            _last_reg_time = phone._last_reg_time;
        }

        return *this;
    }

}vt_phone_t;
typedef std::list<vt_phone_t> VT_PHONE_LIST;
typedef VT_PHONE_LIST::iterator VT_PHONE_ITERATOR;

//ͨѶ¼ͬ����������Ϣ
typedef struct _addrbook_sync_
{
    char _src_indexcode[config_t::MAX_SIP_INDEX_CODE];
    char _src_ip[config_t::MAX_IP_ADDR_LEN];
    HPR_UINT16 _src_port;
    HPR_INT32 _protocol;

}ADDRBOOK_SRC;
typedef std::list<ADDRBOOK_SRC> ADDRBOOK_SRC_LIST;


//������Ϣ
typedef struct _call_record_
{
    std::string _call_id;           //����id
    std::string _calling_phone;     //���з�
    std::string _called_phone;      //���з�
    dev_call_status _call_status;      //����ΪHPR_ATOMIC_T���������
    time_t   _start_time;           //ͨ����ʼʱ��
    time_t   _stop_time;            //ͨ������ʱ��
    time_t   _invite_time;          //����ʱ��
    time_t   _trying_time;          //����100trying��ʱ��

    _call_record_& operator=(const _call_record_& call_record) 
    {
        if (this != &call_record)
        {
            _call_id = call_record._call_id;
            _calling_phone = call_record._calling_phone;
            _called_phone = call_record._called_phone;
            _call_status = call_record._call_status;
            _start_time = call_record._start_time;
            _stop_time = call_record._stop_time;
            _invite_time = call_record._invite_time;
            _trying_time = call_record._trying_time;
        }
        return *this;
    }

}call_record_t;

//������Ϣ
typedef struct _net_zone_
{
    HPR_INT32 _id;
    std::string _ip;
    HPR_UINT16 _port;

    _net_zone_& operator = (const _net_zone_& net_zone)
    {
        if (this != &net_zone)
        {
            _id = net_zone._id;
            _ip = net_zone._ip;
            _port = net_zone._port;
        }

        return *this;
    }

}net_zone_t;

//Զ��������Ϣ
typedef struct _remote_config_
{
    std::string _indexcode;
    std::string _remote_addr;
    std::string _user_agent;
    HPR_UINT16 _sip_port;
    HPR_UINT16 _webservice_port;
    HPR_UINT16 _net_manager_device_port;
    HPR_UINT16 _net_manager_server_port;
    std::list<net_zone_t> _net_zone_info;
}remote_config_t;

//�豸ע����Ϣ
typedef struct _regist_status_
{
    std::string _phone_no;
    std::string _ip;
    HPR_UINT16 _port;
    HPR_UINT64 _time;

    _regist_status_(const char* phone_no, const char* ip, HPR_UINT16 port, HPR_INT64 t)
        : _phone_no(phone_no)
        , _ip(ip)
        , _port(port)
        , _time(t)
    {
    }

    _regist_status_& operator = (const _regist_status_& register_info)
    {
        if (this != &register_info)
        {
            _phone_no = register_info._phone_no;
            _ip = register_info._ip;
            _port = register_info._port;
            _time = register_info._time;
        }

        return *this;
    }

}register_status_t;


#endif /* end of include guard: DEFINER_KB13XD3G */
