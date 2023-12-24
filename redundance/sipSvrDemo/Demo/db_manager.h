/** @file:   db_manager.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  数据库管理模块
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
        std::string _index_code;            //indexcode，暂时不使用
        std::string _manager_center_num;    //管理中心号码
        std::string _user_name;             //服务器的用户名
        std::string _pass_word;             //服务器的密码
        std::string _user_agent;            //SIP代理名称
        HPR_UINT16 _sip_port;               //SIP协议端口
        HPR_UINT16 _webservice_port;        //webservice端口
        HPR_UINT16 _http_port;              //http端口
        HPR_UINT16 _net_manager_device_port;//网管端口，上报设备状态
        HPR_UINT16 _net_manager_server_port;//网管端口，上报服务器状态
        HPR_UINT32 _max_talking_time;       //最大通话时间
        HPR_UINT32 _max_no_rsp_time;        //最大未响应时间
        HPR_UINT32 _hb_timeout;             //心跳超时时间

        HPR_UINT32 _valid_register_time;    //注册的有效期

    }local_config_t;

public:

    static HPR_INT32 create_instance();
    static HPR_VOID destroy_instance();

    inline static db_manager_t* instance()
    {
        return _instance;
    }

    //创建数据库
    HPR_INT32 init(sql_def_t::db_type database);
    //关闭数据库
    HPR_INT32 finit();

    //读取通讯录
#if 0
    HPR_INT32 load_addressbook();
#endif
    //分批次读取通讯录
    HPR_INT32 load_addressbook(VT_PHONE_LIST& phone_list, HPR_INT32 count, HPR_INT32 offset, HPR_INT32& read_count);
    //读取系统配置
    HPR_INT32 get_local_config(db_manager_t::local_config_t& config);
    //写入系统配置信息
    HPR_INT32 set_local_config(db_manager_t::local_config_t& config);
    //读取远程配置信息
    HPR_INT32 get_remote_config(remote_config_t& config);
    //设置远程配置信息
    HPR_INT32 set_remote_config(remote_config_t& config);

#ifdef SYNC_ADDRESS_BOOK
    //删除一条通讯录
    HPR_INT32 del_phone(const char* phone_no);
    //增加一条通讯录
    HPR_INT32 insert_phone(vt_phone_t& phone);
    //更新一条通讯录
    HPR_INT32 update_phone(vt_phone_t& phone);
#endif
    //增加通讯录
    HPR_INT32 add_phone_sync(VT_PHONE_LIST& add_list);
	HPR_INT32 add_single_phone_sync(vt_phone_t& phone);
	HPR_INT32 add_single_phone_sync(device_t& device);
    //更新通讯录
    HPR_INT32 update_phone_sync(VT_PHONE_LIST& update_list);
    //删除通讯录
    HPR_INT32 del_phone_sync(std::list<std::string>& del_list);

#ifdef SYNC_ADDRESS_BOOK
    //读取所有同步源列表
    HPR_INT32 load_sync_src(ADDRBOOK_SRC_LIST& addrbook_src_list);
    //插入一条同步信息
    HPR_INT32 insert_sync_src(const char* indexcode, const char* ip, HPR_UINT16 port, HPR_INT32 protocol);
    //删除一条同步信息
    HPR_INT32 delete_sync_src(const char* indexcode);
#endif

    //写通话记录
    HPR_INT32 write_call_log_sync(std::list<call_record_t>& call_list);
    //读取通话记录
    HPR_INT32 read_call_log_sync(HPR_INT32 read_count, std::list<call_record_t>& call_list);
    //同步删除通话记录
    HPR_INT32 del_call_log_sync(HPR_INT32 del_count);
    //读取日志数量
    HPR_INT32 get_call_log_count();

    //清除表
    HPR_INT32 trancate_addrbook();

    //修改密码
    HPR_INT32 modify_psw(const char* psw);
    //写入注册信息
    HPR_INT32 write_reg_status(std::list<register_status_t>& reg_info_list);

    char *get_local_ip_str(char *ip);
protected:

#ifdef SYNC_ADDRESS_BOOK
    //数据库刷新
    HPR_VOID work(HPR_VOID);
#endif

private:

    //任务放在任务队列中
#ifdef SYNC_ADDRESS_BOOK
    HPR_INT32 push_task_to_list(const char* task);
#endif

    //读取网域信息
    HPR_INT32 read_net_zones(std::list<net_zone_t>& net_zone_list);
    //设置网域信息
    HPR_INT32 set_net_zones(std::list<net_zone_t>& net_zone_list);

private:

    //对象实例
    static db_manager_t* _instance;
    //数据库操作接口
    db_interface_t* _db_interface;

#ifdef SYNC_ADDRESS_BOOK
    //数据库任务列表
    std::list<std::string> _sql_list;
    //数据库任务表锁
    HPR_Mutex _sql_list_lock;
    //数据库刷新时间间隔(5分钟）
    static const HPR_UINT32 DB_REFRESH_TIMEOUT = 300000;
    //任务队列最大任务数
    static const HPR_UINT32 MAX_TASK_COUNT_IN_LIST = 10000;
    //唤醒任务线程的最小任务数
    static const HPR_UINT32 EXE_IMMEDIATELY_TASK_NUM = 500;
#endif

};

#endif
