/**
 * @file device.h
 * @brief �豸����
 * @author zhanghaifengyf1@hikvision.com.cn
 * @version 0.0.1
 * @date 2014-07-08 16:27:22
 */

#ifndef DEVICE_C5GNRAZO
#define DEVICE_C5GNRAZO

#include<string>

#include<hpr/HPR_Atomic.h>
#include<hpr/HPR_Time.h>
#include<hpr/HPR_Utils.h>

#include"atomic_64.h"
#include"definer.h"
#include"service_base.h"
#include"atom_mutex.h"
#include"net_manager.h"

#define STORE_IP_AS_UINT

enum enum_maindevice_index
{
	MAIN_TYPE_INDOOR = 1,
	MAIN_TYPE_OUTDOOR = 2,	
	MAIN_TYPE_MANAGER = 3,	
};

enum enum_subdevice_index
{
	SUB_TYPE_INDOOR = 1,
	SUB_TYPE_ANALOG_INDOOR = 2,
	SUB_TYPE_OUTDOOR = 3,
	SUB_TYPE_WALL = 4,
	SUB_TYPE_VILLA = 5,
	SUB_TYPE_MANAGER = 6,
	SUB_TYPE_PC_MANAGER = 7,
};


class device_t
{
public:
    device_t (/* arguments*/);
    device_t(const vt_phone_t& vt_phone);
    virtual ~device_t ();

    inline const char* get_phone_number()
    {
        return _phone_num.c_str();
    }
    
	inline std::string get_phone_number_str()
    {
        return _phone_num;
    }

    inline HPR_VOID set_phone_number(const std::string& phone_number)
    {
        _phone_num = phone_number;
        return;
    }

	inline const char* get_call_number()
	{
	    return _call_num.c_str();
	}

	inline HPR_VOID set_call_number(const std::string& call_number)
	{
	    _call_num = call_number;
		return;
	}

    inline HPR_VOID get_ip(std::string& device_ip)
    {
#ifdef STORE_IP_AS_UINT
        ip_int_2_string(static_cast<HPR_UINT32>(HPR_AtomicCas((&_ip), 0, 0)), device_ip);
#else
        if(_update_mutex.try_enter() == HPR_TRUE)
        {
            device_ip = _ip;
        }
        else
        {
            device_ip = "";
        }
        _update_mutex.leave();
#endif
        return;
    }

    inline HPR_VOID set_ip(std::string& device_ip)
    {
        HPR_UINT32 uint_ip = ip_string_2_int(device_ip.c_str());
        HPR_AtomicSet(&_ip, static_cast<HPR_ATOMIC_T>(uint_ip));
    }

    inline HPR_UINT16 get_port()
    {
        return static_cast<HPR_UINT16>(HPR_AtomicCas(&_port, 0, 0));
    }

    inline HPR_VOID set_port(HPR_UINT16 device_port)
    {
        HPR_AtomicSet(&_port, static_cast<HPR_ATOMIC_T>(device_port));
    }

    //����
    inline HPR_BOOL on_heartbeat()
    {
        if (is_online_status() == HPR_FALSE)
        {
            return HPR_FALSE;
        }
        else
        {
            set_update_time(HPR_TimeNow()/HPR_US_PER_SEC);
            return HPR_TRUE;
        }
    }

    //�Ƿ�Ϊ����״̬
    inline HPR_BOOL is_idle_status()
    {
        return (HPR_AtomicCas(&_status, dev_status_online, dev_status_online) == dev_status_online);
    }

    //����Ϊ����״̬(ֻ��æ��״̬��������Ϊ����״̬)
    inline HPR_BOOL set_idle_status()
    {
		SIP_SVR_INFO("set device(%s, cur status:%u) to idle status", get_phone_number(), _status);
        return (HPR_AtomicCas(&_status, dev_status_online, dev_status_busy) == dev_status_busy);
    }

    //����Ϊռ��״̬(ֻ�п���״̬��������Ϊæ��״̬)
    inline HPR_BOOL set_busy_status()
    {
		SIP_SVR_INFO("set device(%s, cur status:%u) to busy status", get_phone_number(), _status);
        return (HPR_AtomicCas(&_status, dev_status_busy, dev_status_online) == dev_status_online);
    }

    //�κ�ʱ�򶼿�������Ϊ����״̬
    inline HPR_VOID set_offline_status()
    {
        //�����ģ�����ڻ���һֱ���ߣ�
        if (is_analog_indoor_device())
        {
            return;
        }
		SIP_SVR_INFO("set device(%s, cur status:%u) to offline status", get_phone_number(), _status);
        if (dev_status_offline != HPR_AtomicSet(&_status, dev_status_offline) )
        {
            NET_MANAGE_DEVICE_INFO_T info;
            HPR_Snprintf(info.device_index, 64, "%s", _phone_num.c_str());
            info.device_status = 1;
            net_manager_t::instance()->add_changed_device(_phone_num, info);
        }
        //HPR_AtomicSet(&_be_changed, 1);

        return;
    }

    //�Ƿ�����
    inline HPR_BOOL is_online_status()
    {
        return (HPR_AtomicCas(&_status, dev_status_offline, dev_status_offline) < dev_status_online ? HPR_FALSE : HPR_TRUE);
    }

    //ֻ��ԭ�������ߵ�״̬������Ϊ����״̬����������²���������
    inline HPR_VOID set_online_status()
    {
		SIP_SVR_INFO("set device(%s, cur status:%u) to online status", get_phone_number(), _status);
        if (dev_status_offline == HPR_AtomicCas(&_status, dev_status_online, dev_status_offline) )
        {
            NET_MANAGE_DEVICE_INFO_T info;
            HPR_Snprintf(info.device_index, 64, "%s", _phone_num.c_str());
            info.device_status = 0;
            net_manager_t::instance()->add_changed_device(_phone_num, info);
        }
        //HPR_AtomicSet(&_be_changed, 1);

        return;
    }

    //inline HPR_VOID set_unchanged()
    //{
    //    HPR_AtomicSet(&_be_changed, 0);
    //}

    //inline HPR_BOOL is_changed() 
    //{
    //    return HPR_AtomicCas(&_be_changed, 1, 1)==1?HPR_TRUE:HPR_FALSE;
    //}

    //�豸�Ƿ��ǹ�������
    inline HPR_BOOL is_manager_center() const
    {
        return (_main_type == 3);
    }

    //�����豸���ͣ�����Ƿ�Ϊ��������
    inline static HPR_BOOL check_manager_center(HPR_INT32 main_type, HPR_INT32 sub_type)
    {
        return (main_type == 3);
    }

    //��ȡ�������ȼ�
    inline HPR_INT32 get_priority() const
    {
        return _priority;
    }

    //��ȡ����ʱ��
    inline HPR_TIME_T get_update_time()
    {
        return atomic64_t::atomic64_get(&_update_time);
    }
    //��������ʱ��
    inline HPR_VOID set_update_time(const HPR_TIME_T update_time)
    {
        atomic64_t::atomic64_set(&_update_time, update_time);
        return;
    }

    inline HPR_INT32 get_net_zone_id() const
    {
        return _net_zone_id;
    }

    inline HPR_INT32 get_main_type() const
    {
        return _main_type;
    }

    inline HPR_INT32 get_sub_type() const
    {
        return _sub_type;
    }

    //����Ƿ���Ҫ����ͨѶ¼��Ϣ
    inline HPR_BOOL need_update(vt_phone_t& vt_phone) const
    {
        return ((vt_phone._dev_main_type != _main_type) ||\
                (vt_phone._dev_sub_type != _sub_type) ||\
                (vt_phone._call_priority != _priority) ||\
                (vt_phone._net_zone_id != _net_zone_id));
    }

    //����Ƿ�������ʱ
    inline HPR_VOID check_hb_timeout(HPR_TIME_T t)
    {
        if (t - get_update_time() > config_t::instance()->get_heart_beat_timeout())
        {
            //��������ʱ��ִ�з�ע������
            HPR_Debug("wangfeida_debug: phone %s is expired!", get_phone_number());
            on_unregister();
        }
        return;
    }

    //��int ip��ַתΪstring
    inline static HPR_VOID ip_int_2_string(HPR_UINT32 ip_i, std::string& ip_str)
    {
        char ip_s[16];
        HPR_Snprintf(ip_s, 16, "%d.%d.%d.%d", (ip_i >> 24), ((ip_i >> 16) & 0xff), ((ip_i >> 8) & 0xff), (ip_i & 0xff));
        ip_str = ip_s;
        return;
    }

    inline HPR_BOOL is_analog_indoor_device()
    {
        return ((_main_type == MAIN_TYPE_INDOOR) && (_sub_type == SUB_TYPE_ANALOG_INDOOR));
    }

    //����ͨѶ¼��Ϣ
    HPR_INT32 update(vt_phone_t& vt_phone);
    //ע��
    HPR_BOOL on_register(const char* ip, HPR_UINT16 port, HPR_TIME_T reg_time);
    //��ע��
    HPR_VOID on_unregister();
    //��string����ip��ַת��Ϊint
    static HPR_UINT32 ip_string_2_int(const char* ip_addr);
	//���ݳ��Ż�ȡ�豸����
    static HPR_VOID get_dev_type(char *phone_number, HPR_INT32 &main_type, HPR_INT32 &sub_type);

    HPR_UINT32 get_dev_status()
    {
        return HPR_AtomicCas(&_status, 0, 0);
    }


private:

    //std::string _indexcode;    //�豸������ƽ̨��SIP�������ı��

    std::string _phone_num;
	std::string _call_num;
#ifdef STORE_IP_AS_UINT
    HPR_ATOMIC_T _ip;
#else
    std::string _ip;              //�豸ip
    atom_mutex_t _update_mutex;   //������ip��port����Ϣ�������
    std::string _ip_for_write;    //�豸ip������ע��ʱ���бȽϣ������ÿ���豸�ĵ�ַ���м���
#endif
    HPR_ATOMIC_T _port;

    /*
        �豸״̬  0--�����ߣ�, 16--����/���ߣ�17--ռ��
        �豸״̬ת����1.����ֻ��ת��Ϊ����״̬�����ڸ���״̬
                      2.ֻ������״̬�����ڸ���״̬��ת��Ϊ����״̬������״̬����ת��Ϊ����״̬
                      3.����״̬�����״̬����һ��״̬��--16
                      4.ֻ������״̬��������Ϊæ��״̬
                      5.ֻ��æ��״̬��������Ϊ����״̬�����ڿ��������߹���һ��״̬�룬��������ʱ�������������ÿ���
                        ����Ϊ���߼�����״̬����������ʱ���������ÿ���״̬�Ľӿڻ�ʧ�ܡ�
                      6.æ��ֻ��תΪ���У�����תΪ����״̬
                      7.���ܻ��ں��еĹ����У�����æ�߱�Ϊ���ߣ�����ʱ����Ϊ����״̬��ʧ�ܣ�ֻ�ܵȵ����н�����ʱ��
                        ����æ��תΪ���У����ɳ�ʱ�̸߳�������Ϊ����״̬����ˣ������ж��ݵ�ʱ�䣬�豸״̬����ȷ
                        
    */
    HPR_ATOMIC_T _status; 

    //HPR_ATOMIC_T _be_changed;


    HPR_INT32 _main_type;
    HPR_INT32 _sub_type;

    HPR_INT32 _priority;
    HPR_TIME_T _update_time;
    HPR_INT32 _net_zone_id;      //�豸����id

    static const HPR_UINT32 dev_status_offline = 0;
    static const HPR_UINT32 dev_status_online = 16;
    //static const HPR_UINT32 dev_status_update = 2;
    static const HPR_UINT32 dev_status_busy = 17;

} /* device_t */;

#endif /* end of include guard: DEVICE_C5GNRAZO */
