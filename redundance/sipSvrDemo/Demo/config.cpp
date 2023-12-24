#include<hlog/logdef.h>
#include <hpr/HPR_Utils.h>
#include"config.h"
#include"db_manager.h"
#include"base64.h"

config_t* config_t::_instance = NULL;
const char* config_t::DEFAULT_USER_NAME = "admin";
const char* config_t::DEFAULT_PASS_WORD = "12345";

config_t::config_t( /* arguments*/ )
{

}

config_t::~config_t()
{

}

HPR_INT32 config_t::create_instance()
{
    if (_instance == NULL)
    {
        _instance = new(std::nothrow)config_t();
    }

    return (_instance == NULL ? -1 : 0);
}

HPR_VOID config_t::del_instance()
{
    if (NULL != _instance)
    {
        delete _instance;
        _instance = NULL;
    }

    return;
}

HPR_INT32 config_t::init()
{
    //从数据库读取本地配置信息
    db_manager_t::local_config_t db_config;
	 SIP_SVR_INFO("config_t::init()...");
    if (db_manager_t::instance()->get_local_config(db_config) != 0)
    {
        //获取系统配置信息失败,写入系统配置信息
        db_config._index_code = "0";
        db_config._manager_center_num = "ManageCenter";
		//用户名、密码存储加密后的数据
		char* base64_data = NULL;
		if(base64_encode(DEFAULT_USER_NAME, strlen(DEFAULT_USER_NAME), &base64_data) != -1)
		{
			db_config._user_name = base64_data;
            free(base64_data);
            base64_data = NULL;
		}
		else
		{
			db_config._user_name = "YWRtaW4=";  //admin
		}

		if (base64_encode(DEFAULT_PASS_WORD, strlen(DEFAULT_PASS_WORD), &base64_data) != -1)
		{
			db_config._pass_word = base64_data;
            free(base64_data);
            base64_data = NULL;
		}
		else
		{
			db_config._pass_word = "MTIzNDU="; //12345
		}

        db_config._user_agent = "sipServer";
        db_config._sip_port = 5065;
        db_config._webservice_port = 8000;
        db_config._http_port = 80;
        db_config._net_manager_device_port = 8010;
        db_config._net_manager_server_port = 8011;
        db_config._max_talking_time = MAX_TALKING_TIME;
        db_config._max_no_rsp_time = MAX_NO_RESPONSE;
        db_config._hb_timeout = HB_UPDATE_TIME;     //连续5次心跳失败
        db_config._valid_register_time = VALID_REGISTER_TIME_DEFAULT;

        //写入数据库
        db_manager_t::instance()->set_local_config(db_config);
    }

    //设置系统配置信息
    _manager_center_num = db_config._manager_center_num;
    _user_name = db_config._user_name;
    _password = db_config._pass_word;
    _user_agent = db_config._user_agent;
    _sip_port = db_config._sip_port;
    _webservice_port = db_config._webservice_port;
    _http_port = db_config._http_port;
    _net_manager_device_port = db_config._net_manager_device_port;
    _net_manager_server_port = db_config._net_manager_server_port;
    _max_talking_time = db_config._max_talking_time;
    _max_no_rsp_time = db_config._max_no_rsp_time;
    _heart_beat_timeout = db_config._hb_timeout;

    _valid_register_time = db_config._valid_register_time;

    _auth = HPR_FALSE;
    _realm = "";

    //读取远程配置信息
    remote_config_t remote_config;
    if(db_manager_t::instance()->get_remote_config(remote_config) != 0)
    {
        //读取远程配置信息失败，表明服务器尚未被配置！不能直接返回
        SIP_SVR_WARN("read remote config faild!");
    }

    _indexcode = remote_config._indexcode;
    _remote_ws_uri = remote_config._remote_addr;
    //设置网域信息
    std::list<net_zone_t>::iterator itor = remote_config._net_zone_info.begin();
    while (itor != remote_config._net_zone_info.end())
    {
        _net_zone_map.insert(std::map<HPR_INT32, std::string>::value_type(itor->_id, itor->_ip));

        ++itor;
    }
	set_default_net_zone();

    return HPR_OK;
}

HPR_VOID config_t::finit()
{
    return;
}

HPR_VOID config_t::set_default_net_zone()
{
	char ip[32] = {0};
    std::map<HPR_INT32, std::string>::iterator itor = _net_zone_map.find(-1);
    if (itor != _net_zone_map.end())
    {
 		SIP_SVR_INFO("update default_net_zone!");   
        itor->second = db_manager_t::instance()->get_local_ip_str(ip);
    }
    else
    {
		SIP_SVR_INFO("set default_net_zone!"); 
 		_net_zone_map.insert(std::map<HPR_INT32, std::string>::value_type(-1, db_manager_t::instance()->get_local_ip_str(ip)));
    }

    return;
}

const char* config_t::get_sipws_server_uri()
{
    return "/sipservice";
}

/**
*     config_t.show
*\brief   可以显示net_zone_map（网域表）信息
 
*\param[out] void
*\return HPR_VOID  config_t::
*\for use : config_t::instance()->show();
			sipCmd info		//显示show内容
*/
HPR_VOID  config_t::show()
{
	SIP_SVR_DEBUG("net_zone_map:");
    std::map<HPR_INT32, std::string>::iterator itor = _net_zone_map.begin();
	while(itor != _net_zone_map.end())
	{	
		SIP_SVR_DEBUG("%d:%s\n", itor->first, itor->second.c_str());
		itor++;
	}
    return;
}
