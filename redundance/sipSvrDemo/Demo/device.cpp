#include"device.h"
#include<hpr/HPR_String.h>
#include"register_persistence.h"

device_t::device_t()
: _phone_num("")
#ifdef STORE_IP_AS_UINT
, _ip(0)
#else
, _ip("")
#endif
, _port(0)
, _status(0)
//, _be_changed(0)
, _main_type(0)
, _sub_type(0)
, _priority(0)
, _update_time(0)
, _net_zone_id(-1)
{
}

device_t::device_t(const vt_phone_t& vt_phone)
: _phone_num(vt_phone._phone_no)
#ifdef STORE_IP_AS_UINT
, _ip(0)
#else
, _ip("")
#endif
, _port(0)
, _status(0)
//, _be_changed(0)
, _main_type(vt_phone._dev_main_type)
, _sub_type(vt_phone._dev_sub_type)
, _priority(vt_phone._call_priority)
, _update_time(0)
, _net_zone_id(vt_phone._net_zone_id)
{
    //如果是模拟室内机，初始默认在线
   	if (is_analog_indoor_device())
    {
        _status = dev_status_online;
    }
}

device_t::~device_t()
{
}

HPR_INT32 device_t::update(vt_phone_t& vt_phone)
{
    //根据设备类型，确认是否需要更新设备协议解析对象

    _main_type = vt_phone._dev_main_type;
    _sub_type = vt_phone._dev_sub_type;
    _priority = vt_phone._call_priority;
    _net_zone_id = vt_phone._net_zone_id;

    return 0;
}

HPR_BOOL device_t::on_register(const char* ip, HPR_UINT16 port, HPR_TIME_T reg_time)
{
    SIP_SVR_INFO("------register----- %s, %s, %u.", get_phone_number(), ip, port);
#ifdef STORE_IP_AS_UINT
    HPR_UINT32 uint_ip = ip_string_2_int(ip);
    HPR_AtomicSet(&_ip, static_cast<HPR_ATOMIC_T>(uint_ip));
#else
    HPR_BOOL b_ip_change = HPR_FALSE;
    //若设备ip地址发送改变，则先更新只用来写的ip地址
    if (strcmp(_ip_for_write.c_str(), ip) != 0)
    {
        _ip_for_write = ip;
        b_ip_change = HPR_TRUE;
    }

    //若ip地址有更新，则更新ip地址，保证_ip与_ip_for_write同步
    //有可能存在ip地址更新后，端口还未更新，但此时若获取设备地址信息，将获取错误的信息，
    //但暂时不处理这种情况
    if(b_ip_change == HPR_TRUE)
    {
        _update_mutex.enter(100);
        _ip = ip;
        _update_mutex.leave();
    }
#endif

    HPR_AtomicSet(&_port, static_cast<HPR_ATOMIC_T>(port));
    //更新心跳时间
    set_update_time(reg_time);
    
    set_online_status();

    //存储注册信息
    register_status_t rst(get_phone_number(), ip, port, reg_time);
    register_persistence_t::instance()->persistence_reg_status(rst);

    return HPR_TRUE;
}

HPR_VOID device_t::on_unregister()
{
    SIP_SVR_INFO("-----unregister----- %s.", get_phone_number());
    set_offline_status();
	//存储注册信息
    register_status_t rst(get_phone_number(), "", 0, 0);
    register_persistence_t::instance()->persistence_reg_status(rst);
	return;
}

HPR_UINT32 device_t::ip_string_2_int(const char* ip_addr)
{
    HPR_INT32 i = 0;
    HPR_UINT32 i_ret = 0;
    HPR_INT32 i_loop = 3;

    char ip_tmp[4];

    while(i_loop >= 0)
    {
        while (*(ip_addr + i) != '.' && *(ip_addr + i) != 0)
        {
            ++i;
        }

        if (i > 3)
        {
            return 0;
        }

        memcpy(ip_tmp, ip_addr, i);
        ip_tmp[i] = 0;
        i_ret += (HPR_Atoi32(ip_tmp) << (8*i_loop));
        ip_addr += i+1;
        i = 0;

        -- i_loop;
    }

    return i_ret;
}

HPR_VOID device_t::get_dev_type(char *phone_number, HPR_INT32 &main_type, HPR_INT32 &sub_type)
{
	int zone_no, build_no, unit_no, floor_no, dev_no, ret;
	ret = sscanf(phone_number, "%01d%03d%02d%03d%02d", &zone_no, &build_no, &unit_no, &floor_no, &dev_no);
	if(ret != 5)
	{
		SIP_SVR_ERROR("phone_number:%s invaild\n", phone_number);
		return;
	}
	
	if (unit_no > 0)
	{
		if (floor_no > 0)
		{
			main_type = MAIN_TYPE_INDOOR;
			sub_type = SUB_TYPE_INDOOR;
		}
		else
		{
			main_type = MAIN_TYPE_OUTDOOR;
			sub_type = SUB_TYPE_OUTDOOR;
		}
	}
	else if(build_no == 0)
	{
		if (floor_no == 1)
		{
			main_type = MAIN_TYPE_MANAGER;
			if (dev_no >= 51)
			{
				sub_type = SUB_TYPE_MANAGER;
			}
			else
			{
				sub_type = SUB_TYPE_PC_MANAGER;
			}
		}
		else
		{
			main_type = MAIN_TYPE_OUTDOOR;
			sub_type = SUB_TYPE_WALL;
		}
	}		
}
