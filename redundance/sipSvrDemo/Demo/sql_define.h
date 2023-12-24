/** @file:   sql_define.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  sql语句定义
 *
 *  @author: wangfeida@hikvision.com.cn
 *  @data:   2014/07/18
 *  
 *  @note:   
 *
 *  @Modification History:
 *  <version>       <time>          <author>        <desc>
 *    V1.0        2014/07/18        wangfeida       Create
 *
 *  @warning: <No waining>        
*/

#ifndef _SQL_DEFINE_H_
#define _SQL_DEFINE_H_

//通讯录表
#define SQL_CREATE_ADDRESS_BOOK_TABLE "Create Table if not exists address_book(phone_no char(16) PRIMARY KEY ASC,\
                                                                     phone_name char(32),\
                                                                     dev_main_type interger not null,\
                                                                     dev_sub_type interger not null,\
                                                                     call_priority interger not null,\
                                                                     net_zone_id interger not null,\
                                                                     ip char(16),\
                                                                     port interger not null,\
                                                                     last_reg_time interger not null);"

//日志表
#define SQL_CREATE_CALL_LOG_TABLE "Create Table if not exists call_log(call_id char(128) PRIMARY KEY ASC,\
                                                                       calling_no char(16),\
                                                                       called_no char(16),\
                                                                       start_time char(20),\
                                                                       stop_time char(20),\
                                                                       event_type interger not null);"

//通讯录同步中心表
#define SQL_CREATE_SYNC_SRC_TABLE "Create Table if not exists sync_src(indexcode char(64) PRIMARY KEY ASC,\
                                                                       ip char(16),\
                                                                       port interger not null,\
                                                                       sync_protocol char(32));"

//本地配置表
#define SQL_CREATE_LOCAL_CONFIG_TABLE "Create Table if not exists local_config(indexcode char(64) PRIMARY KEY ASC,\
                                                                          manager_center_num char(16),\
                                                                          user_name char(512) ,\
                                                                          pass_word char(512),\
                                                                          user_agent char(64),\
                                                                          sip_port interger not null,\
                                                                          webservice_port interger not null,\
                                                                          http_port interger not null,\
                                                                          net_manager_device_port interger not null,\
                                                                          net_manager_server_port interger not null,\
                                                                          max_talking_time interger not null,\
                                                                          max_no_rsp_time interger not null,\
                                                                          hearbeat_timeout interger not null,\
                                                                          valid_register_time interger not null);"

//远程配置表
#define SQL_CREATE_REMOTE_CONFIG_TABLE "Create Table if not exists remote_config(remote_id char(128) PRIMARY KEY ASC,\
                                                                          indexcode char(128),\
                                                                          remote_uri char(128));"

//网域信息表
#define SQL_CREATE_NETZONE_TABLE "Create Table if not exists net_zone(id interger PRIMARY KEY,\
                                                                      ip char(16),\
                                                                      port interger not null);"


//读取所有通讯录
#define SQL_SELECT_ALL_ADDRESS_BOOK "select * from address_book;"

//读取所有同步信息
#define SQL_SELECT_ALL_SYNC_SRC "select * from sync_src;"

//读取所有本地配置
#define SQL_SELECT_ALL_LOCAL_CONFIG "select * from local_config;"

//读取远程配置信息
#define SQL_SELECT_REMOTE_CONFIG "select * from remote_config;"

class sql_def_t
{
public:
    sql_def_t(){}
    virtual ~sql_def_t(){}


public:
    /*--------------------------------------------------------------------------
    -------------------数据库字段索引定义-------------------------------------
    ----------------------------------------------------------------------------*/

    //数据库类型
    enum db_type
    {
        DB_SQLITE = 0
    };

    //通讯录表
    enum address_book_table_index
    {
        addrbook_invalid_idx = 0,
        addrbook_phone_no_idx,
        addrbook_phone_name_idx,
        addrbook_dev_main_type_idx,
        addrbook_dev_sub_type_idx,
        addrbook_call_prioriry_idx,
        addrbook_net_zone_id_idx,
        addrbook_ip_idx,
        addrbook_port_idx,
        addrbook_last_reg_time_idx,
        addrbook_idx_num

    };

    //通话记录表
    enum call_log_table_index
    {
        call_log_invalid_idx = 0,
        call_log_call_id_idx,
        call_log_calling_no_idx,
        call_log_called_no_idx,
        call_log_start_time_idx,
        call_log_stop_time_idx,
        call_log_event_type_idx,
        call_log_idex_num

    };

    //本地配置表
    enum local_config_table_index
    {
        local_cfg_invalid_idx = 0,
        local_cfg_indexcode_idx,
        local_cfg_mgr_center_no_idx,
        local_cfg_user_name_idx,
        local_cfg_psw_idx,
        local_cfg_user_agent_idx,
        local_cfg_sip_port_idx,
        local_cfg_webservice_port_idx,
        local_cfg_http_port_idx,
        local_cfg_net_manager_device_port_idx,
        local_cfg_net_manager_server_port_idx,
        local_cfg_max_talk_time_idx,
        local_cfg_max_rsp_time_idx,
        local_cfg_hb_timeout_idx,
        local_cfg_valid_reg_time_idx,
        local_cfg_idx_num

    };

    //远程配置表
    enum remote_config_table_index
    {
        remote_cfg_invalid_idx = 0,
        remote_cfg_remote_id_idx,
        remote_cfg_indexcode_idx,
        remote_cfg_remote_url_idx,
        remote_cfg_idx_num

    };

    //网域配置表
    enum net_zone_table_index
    {
        net_zone_invalid_idx = 0,
        net_zone_id_idx,
        net_zone_ip_idx,
        net_zone_port_idx,
        net_zone_idx_num

    };


};

#endif
