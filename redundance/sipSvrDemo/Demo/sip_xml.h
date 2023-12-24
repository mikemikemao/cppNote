/** @file:   sip_xml.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  sip服务器通信xml解析
 *
 *  @author: wangfeida@hikvision.com.cn
 *  @data:   2014/08/01
 *  
 *  @note:   
 *
 *  @Modification History:
 *  <version>       <time>          <author>        <desc>
 *    V1.0        2014/08/01        wangfeida       Create
 *
 *  @warning: <No waining>        
*/

#ifndef _SIP_XML_H_
#define _SIP_XML_H_

#include<hpr/HPR_Hpr.h>
#include<hpr/HPR_Time.h>
#include<hpr/HPR_Utils.h>
#include<tinyxml/tinyxml.h>
#include"definer.h"

class sip_xml
{
private:
    sip_xml();
    virtual ~sip_xml();
public:
    static const HPR_UINT32 SIP_XML_MAX_NODE_NAME = 64;

    //标签
    typedef enum _label_node
    {
        label_root = 0,
        label_request,
        label_body,
        label_call_records,
        label_record,
        label_operate,
        label_param,
        label_sipServer,
        label_content,
        label_list,
        label_devices,
        label_device,
        label_configuration,
        label_webServiceUri,
        label_indexCode,
        label_webservice_port,
        label_sip_port,
        label_netmanager_device_port,
        label_netmanager_server_port,
        label_cms_version,
        label_netAreaInfo,
        label_netArea,
        //sipSDK
        label_sdk,
        label_plat,
        label_dso,
        label_function,
        label_protocol,

        //返回报文
        label_result,

        label_num

    }label_node;

    typedef enum _attr_node
    {
        attr_IP = 0,
        attr_phone_no,
        attr_callHierarchy,
        attr_mainModel,
        attr_subModel,
        attr_netZoneId,
        attr_sysCode,
        //sip 平台sdk
        attr_plat_name,
        attr_dso_file,
        attr_func_init,
        attr_func_finit,
        attr_func_upload_record,
        attr_func_error,
        attr_func_version,
        attr_func_free_temporary_data,

        //xml头
        attr_xml_version,
        attr_xml_encoding,
        attr_xml_standlone,

        //呼叫日志
        attr_callId,
        attr_callingNum,
        attr_calledNum,
        attr_callType,
        attr_startTime,
        attr_stopTime,

        //返回报文
        attr_message,
        attr_error_code,
        attr_result,

        attr_num

    }attr_node;

    static const char _label_table[label_num][SIP_XML_MAX_NODE_NAME];
    static const char _attr_table[attr_num][SIP_XML_MAX_NODE_NAME];

    static HPR_INT32 parse_addrbook_add(const char* xml, VT_PHONE_LIST& add_list, VT_PHONE_LIST& update_list, std::list<std::string>& del_list);

    static HPR_INT32 parse_remote_config(const char* xml, remote_config_t& remote_config);

    static HPR_INT32 parse_sdk_config(const char* config_file, std::string& plat_name, std::string& dso_file, std::string& init_name,\
		std::string& finit_name, std::string& record_name, std::string& error_name, std::string& version_name, std::string& free_data_name);

    static HPR_INT32 build_call_record(std::string& out_xml, std::list<call_record_t>& call_records);

    //解析返回报文
    static HPR_INT32 parse_response_xml(const char* xml, std::string& result_message, HPR_INT32& error_code, std::string& ret_str);

private:

    //解析通讯录信息
    static HPR_INT32 parse_add_update_dev(TiXmlElement* devices, VT_PHONE_LIST& phone_list);

    //解析通讯录删除信息
    static HPR_INT32 parse_del_dev(TiXmlElement* devices, std::list<std::string>& del_list);

    //时间格式转换
    static HPR_INT32 time_2_string(time_t t, char* str_time, HPR_INT32 str_len)
    {
        HPR_TIME_EXP_T exp_time;
        if(HPR_ExpTimeFromTimeLocal(t, &exp_time) == 0)
        {
            HPR_Snprintf(str_time, str_len - 1, "%04d-%02d-%02d %02d:%02d:%02d", exp_time.tm_year+1900,\
				exp_time.tm_mon+1, exp_time.tm_mday,  exp_time.tm_hour, exp_time.tm_min, exp_time.tm_sec);

            return 0;
        }
        else
        {
            *str_time = 0;
        }

        return -1;
    }
    //static const HPR_UINT32 SIP_XML_MAX_NODE_NAME = 64;

};

#endif
