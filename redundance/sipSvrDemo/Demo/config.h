/**
 * @file config.h
 * @brief ������Ϣ�����࣬����
 * @author zhanghaifengyf1@hikvision.com.cn
 * @version 0.0.1
 * @date 2014-07-08 16:25:56
 */

#ifndef CONFIG_WFG7TMOZ
#define CONFIG_WFG7TMOZ

#include<string>
#include<map>

#include<hpr/HPR_Hpr.h>

class config_t
{
private:
    config_t (/* arguments*/);
    virtual ~config_t ();

public:

    static HPR_INT32 create_instance();
    static HPR_VOID del_instance();
    HPR_INT32 init();
    HPR_VOID finit();
    
    HPR_VOID set_default_net_zone();
    const char* get_sipws_server_uri();
	HPR_VOID  show();

    inline static config_t* instance()
    {
        return _instance;
    }

    inline const char* get_indexcode()
    {
        return _indexcode.c_str();
    }

    inline const char* get_remote_ws_uri()
    {
        return _remote_ws_uri.c_str();
    }

    inline const char* get_mgr_phone_num()
    {
        return _manager_center_num.c_str();
    }

    inline std::string get_user_name()
    {
        return _user_name;
    }

    inline std::string get_password()
    {
        return _password;
    }

    inline HPR_UINT32 get_max_talking_time() const
    {
        return _max_talking_time;
    }

    inline HPR_UINT32 get_max_no_rsp_time() const
    {
        return _max_no_rsp_time;
    }

    inline HPR_UINT32 get_heart_beat_timeout() const
    {
        return _heart_beat_timeout;
    }

    inline const char* get_local_ip(HPR_INT32 net_zone_id)
    {
        std::map<HPR_INT32, std::string>::iterator itor = _net_zone_map.find(net_zone_id);
        return (itor == _net_zone_map.end() ? "" : itor->second.c_str());
    }

    inline HPR_UINT16 get_sip_port() const
    {
        return _sip_port;
    }

    inline HPR_UINT16 get_webservice_port() const
    {
        return _webservice_port;
    }

    inline HPR_UINT16 get_http_port() const
    {
        return _http_port;
    }

    inline HPR_UINT16 get_net_manager_device_port() const
    {
        return _net_manager_device_port;
    }

    inline HPR_UINT16 get_net_manager_server_port() const
    {
        return _net_manager_server_port;
    }

    inline const char* get_user_agent()
    {
        return _user_agent.c_str();
    }

    inline HPR_BOOL get_auth_status()
    {
        return _auth;
    }

    inline std::string get_realm()
    {
        return _realm;
    }

    inline HPR_UINT32 get_valid_register_time()
    {
        return _valid_register_time;
    }

private:
    static config_t* _instance;

    std::string _indexcode;
    std::string _remote_ws_uri;

    HPR_UINT16 _sip_port;
    HPR_UINT16 _webservice_port;
    HPR_UINT16 _http_port;
    HPR_UINT16 _net_manager_device_port;
    HPR_UINT16 _net_manager_server_port;

    std::string _user_agent;

    HPR_BOOL _auth;
    std::string _realm;
    std::string _user_name;
    std::string _password;

    //�������ĺ���
    std::string _manager_center_num;

    //�δ��Ӧʱ��
    HPR_UINT32 _max_no_rsp_time;
    //���ͨ��ʱ��
    HPR_UINT32 _max_talking_time;
    //�豸������ʱʱ��
    HPR_UINT32 _heart_beat_timeout;
    //SIP����������ip��Ϣ
    std::map<HPR_INT32, std::string> _net_zone_map;

    //ע����Ч��
    HPR_UINT32 _valid_register_time;

public:

    /** һЩ���ò��������� */
    //���к�����󳤶�
    static const HPR_UINT32 MAX_PHONE_NUM_LEN = 16;
    static const HPR_UINT32 MAX_PHONE_NAME_LEN = 32;
    //sip������indexcode��󳤶�
    static const HPR_UINT32 MAX_SIP_INDEX_CODE = 33;
    //ip��ַ��󳤶�
    static const HPR_UINT32 MAX_IP_ADDR_LEN = 16;
    //ʱ���ַ�����󳤶�
    static const HPR_UINT32 MAX_TIME_STR_LEN = 20;
    //��󱻽�δ��Ӧʱ�� 10��
    static const HPR_UINT32 MAX_NO_RESPONSE = 6;
    //���ͨ��ʱ�� 30m
    static const HPR_UINT32 MAX_TALKING_TIME = 1800;
    //�������
    static const HPR_UINT32 HB_UPDATE_TIME = 600;
    //��־�ش����
    static const HPR_UINT32 LOG_RE_UPLOAD_TIME = 600;
    //ÿ���ϴ��ĺ��м�¼���ֵ500
    static const HPR_UINT32 MAX_UPLOAD_RECORD_COUNT_ONCE = 100;
    //���ݿ��¼�������м�¼��
    static const HPR_UINT32 MAX_CALL_RECORD_NUM_DB = 30000;
    //֧�ֵ�����豸����
    static const HPR_UINT32 MAX_DEV_COUNT = 10000;
    //ע�����Ч��
    static const HPR_UINT32 VALID_REGISTER_TIME_DEFAULT = 3600;
	//Ĭ�ϵ��û���
	static const char* DEFAULT_USER_NAME;
	//Ĭ�ϵ�����
	static const char* DEFAULT_PASS_WORD;

} /* config_t */;

#endif /* end of include guard: CONFIG_WFG7TMOZ */
