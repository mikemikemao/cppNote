#include"plat_sdk.h"
#include"sip_xml.h"
#include"config.h"
#include<hpr/HPR_DllSo.h>
#include<hpr/HPR_String.h>
#include<hlog/logdef.h>
#include <ws/V2.1.1/SipServiceSoap12Binding.nsmap>
#include<ws/V2.1.1/sipServerSipServiceSoap12BindingProxy.h>

const std::string plat_sdk_t::null_interface = "null";
plat_sdk_t* plat_sdk_t::_instance = NULL;

plat_sdk_t::plat_sdk_t()
: _sdk_handle(HPR_INVALID_HANDLE)
, _init_interface(NULL)
, _finit_interface(NULL)
, _upload_record_interface(NULL)
, _get_last_error_interface(NULL)
, _get_version_interface(NULL)
, _free_temporary_data(NULL)
{
}

plat_sdk_t::~plat_sdk_t()
{
    _sdk_handle = HPR_INVALID_HANDLE;
}

HPR_INT32 plat_sdk_t::create_instance()
{
    if (_instance == NULL)
    {
        _instance = new(std::nothrow)plat_sdk_t();
    }

    return (_instance == NULL ? -1 : 0);
}

HPR_VOID plat_sdk_t::del_instance()
{
    if (_instance != NULL)
    {
        delete _instance;
        _instance = NULL;
    }
}

HPR_INT32 plat_sdk_t::init()
{
    std::string plat_name, dso_name;
    std::string init_str, finit_str, upload_str, error_str, version_str, free_data_str;

    if (sip_xml::parse_sdk_config("plat.sdk", plat_name, dso_name, init_str, finit_str, upload_str, error_str, version_str, free_data_str) != 0)
    {
        SIP_SVR_ERROR("Parse sdk config faild!");
        return -1;
    }

    _sdk_handle = HPR_LoadDSo(dso_name.c_str());
    if (_sdk_handle == HPR_INVALID_HANDLE)
    {
        SIP_SVR_ERROR("load dso %s faild!", dso_name.c_str());
        return -1;
    }

    //加载接口
    if (init_str != null_interface)
    {
        _init_interface = reinterpret_cast<sip_sdk_init>(HPR_GetDsoSym(_sdk_handle, init_str.c_str()));
    }

    if (finit_str != null_interface)
    {
        _finit_interface = reinterpret_cast<sip_sdk_finit>(HPR_GetDsoSym(_sdk_handle, finit_str.c_str()));
    }

    _upload_record_interface = reinterpret_cast<sip_sdk_upload_record>(HPR_GetDsoSym(_sdk_handle, upload_str.c_str()));
    if (_upload_record_interface == NULL)
    {
        SIP_SVR_ERROR("upload_interface is null!");
        return -1;
    }

    if (error_str != null_interface)
    {
        _get_last_error_interface = reinterpret_cast<sip_sdk_get_last_error>(HPR_GetDsoSym(_sdk_handle, error_str.c_str()));
    }

    if (version_str != null_interface)
    {
        _get_version_interface = reinterpret_cast<sip_sdk_version>(HPR_GetDsoSym(_sdk_handle, version_str.c_str()));
    }

	if (free_data_str != null_interface)
	{
		_free_temporary_data = reinterpret_cast<sip_sdk_free_temporary_data>(HPR_GetDsoSym(_sdk_handle, free_data_str.c_str()));
	}

    if (_init_interface != NULL)
    {
		if (_init_interface(config_t::instance()->get_remote_ws_uri()) != 0)
        {
            return -1;
        }
    }

    return 0;
}

HPR_VOID plat_sdk_t::finit()
{
    SIP_SVR_INFO("start finit plat sdk ...");
    //卸载动态库
    if (_sdk_handle != HPR_INVALID_HANDLE)
    {
        if (_finit_interface != NULL)
        {
            _finit_interface();
        }

        _init_interface = NULL;
        _finit_interface = NULL;
        _upload_record_interface = NULL;
        _get_last_error_interface = NULL;
        _get_version_interface = NULL;
		_free_temporary_data = NULL;

        HPR_UnloadDSo(_sdk_handle);
        _sdk_handle = HPR_INVALID_HANDLE;
    }

    SIP_SVR_INFO("plat sdk unloaded!");

    return;
}

HPR_INT32 plat_sdk_t::upload_record(std::list<call_record_t>& call_info)
{
    int ret = -1;
	HPR_INT32 errorCode = 0;
	std::string str_msg;
    std::string str_ret;
	std::string str_call_info;
	const char* ret_xml = NULL;
	SipServiceSoap12BindingProxy up_rec;
    _sipService1__writeCallRecord req;
    _sipService1__writeCallRecordResponse rsp;

    if (_upload_record_interface != NULL)
    {
        //构建上传的xml
        if (sip_xml::build_call_record(str_call_info, call_info) != 0)
        {
            SIP_SVR_ERROR("build upload record xml faild!");
            return -1;
        }
		
        //上传记录
        if (0 == HPR_Strcmp(config_t::instance()->get_user_agent(), "sipServer"))
        {
            ret_xml = _upload_record_interface(config_t::instance()->get_indexcode(), str_call_info.c_str(),\
                config_t::instance()->get_remote_ws_uri());
        }
        else
        {
            req.indexCode = (char*)config_t::instance()->get_indexcode();
            req.xml = (char*)str_call_info.c_str();
            if (SOAP_OK == up_rec.writeCallRecord(config_t::instance()->get_remote_ws_uri(), NULL, &req, &rsp))
            {
                ret_xml = rsp.return_;
            }
        }

        if (NULL == ret_xml)
        {
            HPR_OutputDebug("wangfeida_debug: upload call record ret null!");
            ret = -1;
        }
        else
        {
            if (sip_xml::parse_response_xml(ret_xml, str_msg, errorCode, str_ret) != 0)
            {
                SIP_SVR_ERROR("parse upload xml rsp faild!xml is:%s", ret_xml);
                HPR_OutputDebug("wangfeida_debug: upload call record parse xml faild!");
                ret = -1;
            }
            else
            {
                if (0 == errorCode)
                {
                    ret = 0;
                }
                else
                {
                    ret = -1;
                    SIP_SVR_ERROR("upload record faild! msg is %s.", str_msg.c_str());
                }
            }

        }

        SIP_SVR_DEBUG("upload xml is %s", str_call_info.c_str());

        if (_free_temporary_data != NULL)
        {
            _free_temporary_data();
        }
    }
	
    return ret;
}
