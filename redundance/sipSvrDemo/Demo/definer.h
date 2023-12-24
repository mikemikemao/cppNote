/**
 * @file definer.h
 * @brief 结构体定义
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
#define UAS_STATUS_200 200  //成功响应
#define UAS_STATUS_300 300  //转发
#define UAS_STATUS_400 400  //失败响应
#define UAS_STATUS_401 401  //需要鉴权
#define UAS_STATUS_403 403  //注册拒绝，平台地址编码不正确
#define UAS_STATUS_404 404  //地址找不到
#define UAS_STATUS_406 406  //拒绝接听
#define UAS_STATUS_603 603  //拒绝接听
#define UAS_STATUS_407 407  //上级平台当前负荷已满，稍后再注册(时间在20~40秒)
#define UAS_STATUS_486 486  //服务器忙或目的设备满负荷
#define UAS_STATUS_480 480  //媒体协商失败。在应答中增加“Warning”头，描述失败的原因
#define UAS_STATUS_488 488  //媒体协商失败。在应答中增加“Warning”头，描述失败的原因
#define UAS_STATUS_500 500  //因为内部逻辑错误，不能处理请求
#define UAS_STATUS_503 503  //服务器资源不足


enum dev_call_status
{
    call_status_service_stop = 0,       //程序结束，导致异常通话，结束时间为程序结束时间
    call_status_recv_call,              //接收到呼叫请求
    call_status_cancel,                 //取消呼叫
    call_status_be_talking,             //正在通话中
    call_status_decline,                //拒绝接听
    call_status_no_one_answer,          //无人接听超时
    call_status_hang_up,                //正常通话结束挂机
    call_status_max_talking,            //没有挂机，超过最大时间
    call_status_faild                   //呼叫失败
};

#define SIP_MESSAGE_BUF_LEN 8191    //sip信令最大接收长度
#define NETMANAGE_MESSAGE_BUF_LEN 8191    //sip信令最大接收长度

#define MSG_HEADER  "header"
#define MSG_METHOD  "method"

#define MSG_HEART_BEAT  1
#define MSG_ALARM       6

#define NUM_TO_REPLACE  6 //根据室内机寻找门口机时需要替换“00000”的字符位置
#define FLOOR_FIGURE    5 //层号房间号的位数
#define OUTDOOR_FLAG    "00000" //门口机标志

//通讯录信息
typedef struct _vt_phone_info_
{
    std::string _phone_no;  //长号
    std::string _phone_name;
    //char _sip_indexcode[config_t::MAX_SIP_INDEX_CODE]; //sip服务器indexcode
    HPR_INT32 _dev_main_type;                  //主类型
    HPR_INT32 _dev_sub_type;                   //次类型
    HPR_INT32 _call_priority;                  //呼叫优先级
    HPR_INT32 _net_zone_id;                    //网域id

    //用于注册信息
    std::string _register_ip;                  //注册时的ip地址
    HPR_UINT32  _register_port;
    HPR_UINT64  _last_reg_time;               //最新一次的注册时间

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

//通讯录同步服务器信息
typedef struct _addrbook_sync_
{
    char _src_indexcode[config_t::MAX_SIP_INDEX_CODE];
    char _src_ip[config_t::MAX_IP_ADDR_LEN];
    HPR_UINT16 _src_port;
    HPR_INT32 _protocol;

}ADDRBOOK_SRC;
typedef std::list<ADDRBOOK_SRC> ADDRBOOK_SRC_LIST;


//呼叫信息
typedef struct _call_record_
{
    std::string _call_id;           //呼叫id
    std::string _calling_phone;     //主叫方
    std::string _called_phone;      //被叫方
    dev_call_status _call_status;      //定义为HPR_ATOMIC_T，方便操作
    time_t   _start_time;           //通话开始时间
    time_t   _stop_time;            //通话结束时间
    time_t   _invite_time;          //请求时间
    time_t   _trying_time;          //返回100trying的时间

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

//网域信息
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

//远程配置信息
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

//设备注册信息
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
