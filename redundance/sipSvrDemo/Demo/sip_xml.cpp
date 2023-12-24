#include"sip_xml.h"
#include<hpr/HPR_String.h>
#include<hlog/logdef.h>

#define get_xml_element_attr_int(element, attr, value_i, value_tmp)\
    value_tmp = element->Attribute(_attr_table[attr]);\
    if (value_tmp == NULL)\
    {\
        return -1;\
    }\
    value_i = HPR_Atoi32(value_tmp)

#define get_xml_element_attr_str(element, attr, value_str, value_tmp)\
    value_tmp = element->Attribute(_attr_table[attr]);\
    if (value_tmp == NULL)\
    {\
        return -1;\
    }\
    HPR_Snprintf(value_str, sizeof(value_str), value_tmp)

#define get_xml_element_attr_std_string(element, attr, value_str, value_tmp)\
    value_tmp = element->Attribute(_attr_table[attr]);\
    if (value_tmp == NULL)\
    {\
        return -1;\
    }\
    value_str = value_tmp

#define get_xml_element_label_std_string(element, value_str, value_tmp)\
    value_tmp = element->GetText();\
    if (value_tmp == NULL)\
    {\
        return -1;\
    }\
    value_str = value_tmp

#define get_xml_element_label_int(element, value_i, value_tmp)\
    value_tmp = element->GetText();\
    if (value_tmp == NULL)\
    {\
        return -1;\
    }\
    value_i = HPR_Atoi32(value_tmp)

#define get_xml_element_label_int_no_err(element, value_i, value_tmp, value_no_err)\
    value_tmp = element->GetText();\
    if (value_tmp != NULL)\
    {\
        value_i = HPR_Atoi32(value_tmp);\
    }\
    else\
    {\
        value_i = value_no_err;\
    }


const char sip_xml::_label_table[label_num][SIP_XML_MAX_NODE_NAME] = {
    "root",
    "request",
    "body",
    "callRecords",
    "record",
    "operator",
    "param",
    "sipServer",
    "content",
    "list",
    "devices",
    "device",
    "configuration",
    "webServiceUri",
    "indexCode",
    "port",
    "sipCommunicatePort",
    "netmanagerDevicePort",
    "netmanagerServerPort",
    "cmsVersion",
    "netAreaInfo",
    "netArea",
    "sdk",
    "plat",
    "dso",
    "function",
    "protocol",
    "result"

};

const char sip_xml::_attr_table[attr_num][SIP_XML_MAX_NODE_NAME] = {

    "IP",
    "longNum",
    "callHierarchy",
    "mainModel",
    "subModel",
    "netZoneId",
    "sysCode",
    "name",
    "file",
    "init",
    "finit",
    "upload_record",
    "error",
    "version",
    "free_temporary_data",
    "1.0",
    "UTF-8",
    "yes",
    "callId",
    "callingNum",
    "calledNum",
    "callType",
    "startTime",
    "stopTime",
    "message",
    "errorCode",
    "result"

};

sip_xml::sip_xml()
{
}

sip_xml::~sip_xml()
{

}

HPR_INT32 sip_xml::parse_addrbook_add(const char*xml, VT_PHONE_LIST& add_list, VT_PHONE_LIST& update_list, std::list<std::string>& del_list)
{
    TiXmlDocument addrbook_doc;

    addrbook_doc.Parse(xml);
    if (addrbook_doc.Error())
    {
        return -1;
    }

    TiXmlElement* root = addrbook_doc.RootElement();
    if (root == NULL)
    {
        return -1;
    }

    TiXmlElement* request = root->FirstChildElement(_label_table[label_request]);
    if (request == NULL)
    {
        return -1;
    }

    TiXmlElement* op = request->FirstChildElement(_label_table[label_operate]);
    if (op == NULL)
    {
        return -1;
    }

    const char* operator_type = op->GetText();
    if (operator_type == NULL)
    {
        return -1;
    }

    TiXmlElement* content = root->FirstChildElement(_label_table[label_content]);
    if (content == NULL)
    {
        return -1;
    }

    TiXmlElement* devices = content->FirstChildElement(_label_table[label_devices]);
    if (devices == NULL)
    {
        return -1;
    }

    if (strcmp(operator_type, "ADD_DEVICE") == 0)
    {
        return parse_add_update_dev(devices, add_list);
    }
    else if (strcmp(operator_type, "UPDATE_DEVICE") == 0)
    {

        return parse_add_update_dev(devices, update_list);
    }
    else if (strcmp(operator_type, "DEL_DEVICE") == 0)
    {
        return parse_del_dev(devices, del_list);
    }
    else
    {
        return -1;
    }
}

HPR_INT32 sip_xml::parse_add_update_dev(TiXmlElement* devices, VT_PHONE_LIST& phone_list)
{
    TiXmlElement* device = devices->FirstChildElement(_label_table[label_device]);
    const char* attr_value = NULL;

    vt_phone_t phone;

    while (device != NULL)
    {
        //长号
        get_xml_element_attr_std_string(device, attr_phone_no, phone._phone_no, attr_value);
        
        //呼叫优先级
        get_xml_element_attr_int(device, attr_callHierarchy, phone._call_priority, attr_value);

        //主设备类型
        get_xml_element_attr_int(device, attr_mainModel, phone._dev_main_type, attr_value);

        //次设备类型
        get_xml_element_attr_int(device, attr_subModel, phone._dev_sub_type, attr_value);

        //网域id
        get_xml_element_attr_int(device, attr_netZoneId, phone._net_zone_id, attr_value);

        phone_list.push_back(phone);

        device = device->NextSiblingElement(_label_table[label_device]);
    }

    return 0;
}

HPR_INT32 sip_xml::parse_del_dev(TiXmlElement* devices, std::list<std::string>& del_list)
{
    TiXmlElement* device = devices->FirstChildElement(_label_table[label_device]);
    const char* attr_value = NULL;

    std::string phone;
    while (device != NULL)
    {
        //长号
        get_xml_element_attr_std_string(device, attr_phone_no, phone, attr_value);

        del_list.push_back(phone);

        device = device->NextSiblingElement(_label_table[label_device]);
    }

    return 0;
}

HPR_INT32 sip_xml::parse_remote_config(const char* xml, remote_config_t& remote_config)
{
    TiXmlDocument addrbook_doc;

    addrbook_doc.Parse(xml);
    if (addrbook_doc.Error())
    {
        return -1;
    }

    TiXmlElement* configuration = addrbook_doc.RootElement();
    if (configuration== NULL)
    {
        return -1;
    }

    const char* value_str = NULL;

    //远端地址
    TiXmlElement* webservice_uri = configuration->FirstChildElement(_label_table[label_webServiceUri]);
    if (webservice_uri == NULL)
    {
        return -1;
    }

    get_xml_element_label_std_string(webservice_uri, remote_config._remote_addr, value_str);

    //indexcode
    TiXmlElement* indexcode = configuration->FirstChildElement(_label_table[label_indexCode]);
    if (indexcode == NULL)
    {
        return -1;
    }

    get_xml_element_label_std_string(indexcode, remote_config._indexcode, value_str);

    //webservice port
    remote_config._webservice_port = config_t::instance()->get_webservice_port();
    TiXmlElement* webservice_port = configuration->FirstChildElement(_label_table[label_webservice_port]);
    if (webservice_port != NULL)
    {
        get_xml_element_label_int(webservice_port, remote_config._webservice_port, value_str);
    }

    //sip port
    remote_config._sip_port = config_t::instance()->get_sip_port();
    TiXmlElement* sip_port = configuration->FirstChildElement(_label_table[label_sip_port]);
    if (sip_port != NULL)
    {
        get_xml_element_label_int(sip_port, remote_config._sip_port, value_str);
    }

    //netmanager device port
    remote_config._net_manager_device_port = config_t::instance()->get_net_manager_device_port();
    TiXmlElement* netmanager_device_port = configuration->FirstChildElement(_label_table[label_netmanager_device_port]);
    if (netmanager_device_port != NULL)
    {
        get_xml_element_label_int(netmanager_device_port, remote_config._net_manager_device_port, value_str);
    }

    //netmanager service port
    remote_config._net_manager_server_port = config_t::instance()->get_net_manager_server_port();
    TiXmlElement* netmanager_service_port = configuration->FirstChildElement(_label_table[label_netmanager_server_port]);
    if (netmanager_service_port != NULL)
    {
        get_xml_element_label_int(netmanager_service_port, remote_config._net_manager_server_port, value_str);
    }

    //user agent
    remote_config._user_agent = "sipServer";
    TiXmlElement* cms_version = configuration->FirstChildElement(_label_table[label_cms_version]);
    if (cms_version != NULL)
    {
        std::string str_cms_version;
        get_xml_element_label_std_string(cms_version, str_cms_version, value_str);
        remote_config._user_agent += str_cms_version;
    }

    //网域信息
    net_zone_t net_zone;
    TiXmlElement* net_area_info = configuration->FirstChildElement(_label_table[label_netAreaInfo]);
    if (net_area_info == NULL)
    {
        return -1;
    }

    TiXmlElement* net_area = net_area_info->FirstChildElement(_label_table[label_netArea]);

    while (net_area != NULL)
    {
        //网域ip
        get_xml_element_attr_std_string(net_area, attr_IP, net_zone._ip, value_str);
        //网域id
        get_xml_element_label_int(net_area, net_zone._id, value_str);

        remote_config._net_zone_info.push_back(net_zone);

        net_area = net_area->NextSiblingElement(_label_table[label_netArea]);
    }

    return 0;
}

HPR_INT32 sip_xml::parse_sdk_config(const char* config_file, std::string& plat_name, std::string& dso_file, std::string& init_name,\
                                    std::string& finit_name, std::string& record_name, std::string& error_name, std::string& version_name,\
									std::string& free_data_name)
{
    TiXmlDocument sdk_doc;
    if (!sdk_doc.LoadFile(config_file))
    {
        SIP_SVR_ERROR("load sdk config file faild!");
        return -1;
    }

    //解析xml
    TiXmlElement* root = sdk_doc.RootElement();
    if (root == NULL)
    {
        SIP_SVR_ERROR("parse root <sdk> faild!");
        return -1;
    }

    const char* value_str = NULL;

    //平台名称
    TiXmlElement* element = root->FirstChildElement(_label_table[label_plat]);
    if (element == NULL)
    {
        SIP_SVR_ERROR("parse %s faild!", _label_table[label_plat]);
        return -1;
    }
    get_xml_element_attr_std_string(element, attr_plat_name, plat_name, value_str);

    //sdk名字
    element = root->FirstChildElement(_label_table[label_dso]);
    if (element == NULL)
    {
        SIP_SVR_ERROR("parse %s faild!", _label_table[label_dso]);
        return -1;
    }
    get_xml_element_attr_std_string(element, attr_dso_file, dso_file, value_str);

    //接口
    element = root->FirstChildElement(_label_table[label_function]);
    if (element == NULL)
    {
        SIP_SVR_ERROR("parse %s faild!", _label_table[label_function]);
        return -1;
    }

    get_xml_element_attr_std_string(element, attr_func_init, init_name, value_str);
    get_xml_element_attr_std_string(element, attr_func_finit, finit_name, value_str);
    get_xml_element_attr_std_string(element, attr_func_upload_record, record_name, value_str);
    get_xml_element_attr_std_string(element, attr_func_error, error_name, value_str);
    get_xml_element_attr_std_string(element, attr_func_version, version_name, value_str);
	get_xml_element_attr_std_string(element, attr_func_free_temporary_data, free_data_name, value_str);

    return 0;
}

//构建需要上传的通话记录的xml报文
//out_xml		输出上传通话记录的xml报文
//call_records	待上传通话记录列表
HPR_INT32 sip_xml::build_call_record(std::string& out_xml, std::list<call_record_t>& call_records)
{
    TiXmlDocument doc;

    TiXmlDeclaration declaration(_attr_table[attr_xml_version], _attr_table[attr_xml_encoding], _attr_table[attr_xml_standlone]);
    doc.InsertEndChild(declaration);

    //root
    TiXmlElement root(_label_table[label_root]);
    //request
    TiXmlElement request(_label_table[label_request]);

    //operate
    TiXmlElement operater(_label_table[label_operate]);

    TiXmlText* operator_text = new(std::nothrow)TiXmlText("UPLOAD_CALL_RECORD");
    if (operator_text == NULL)
    {
        return -1;
    }
    operater.LinkEndChild(operator_text);

    //callRecords
    TiXmlElement parameter(_label_table[label_param]);

    TiXmlElement record(_label_table[label_record]);

    char str_time[32];
    //record
    std::list<call_record_t>::iterator itor = call_records.begin();
    while (itor != call_records.end())
    {
        record.SetValue(_label_table[label_record]);

        record.SetAttribute(_attr_table[attr_callId], itor->_call_id);
        record.SetAttribute(_attr_table[attr_callingNum], itor->_calling_phone);
        record.SetAttribute(_attr_table[attr_calledNum], itor->_called_phone);
        record.SetAttribute(_attr_table[attr_callType], static_cast<HPR_INT32>(itor->_call_status));

        time_2_string(itor->_start_time, str_time, 32);
        record.SetAttribute(_attr_table[attr_startTime], str_time);

        time_2_string(itor->_stop_time, str_time, 32);
        record.SetAttribute(_attr_table[attr_stopTime], str_time);

        parameter.InsertEndChild(record);

        record.Clear();
        ++itor;
    }

    request.InsertEndChild(operater);
    request.InsertEndChild(parameter);
    root.InsertEndChild(request);
    doc.InsertEndChild(root);

    //std::stringstream oss;
    //oss << doc;
    //out_xml = oss.str();

    out_xml << doc;
    return 0;
}
/*lint -restore*/

HPR_INT32 sip_xml::parse_response_xml(const char* xml, std::string& result_message, HPR_INT32& error_code, std::string& ret_str)
{
    TiXmlDocument addrbook_doc;

    addrbook_doc.Parse(xml);
    if (addrbook_doc.Error())
    {
        return -1;
    }

    TiXmlElement* root = addrbook_doc.RootElement();
    if (root == NULL)
    {
        return -1;
    }

    const char* value_str = NULL;

    TiXmlElement* result_ele = root->FirstChildElement(_label_table[label_result]);
    if (result_ele == NULL)
    {
        return -1;
    }

    /////////解析各字段//////////
    //message
    get_xml_element_attr_std_string(result_ele, attr_message, result_message, value_str);
    //errorcode
    get_xml_element_attr_int(result_ele, attr_error_code, error_code, value_str);
    //result
    get_xml_element_attr_std_string(result_ele, attr_result, ret_str, value_str);

    return 0;
}
