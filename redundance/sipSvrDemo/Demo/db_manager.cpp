#include<hpr/HPR_Utils.h>
#include<hpr/HPR_Guard.h>
#include<hpr/HPR_Error.h>
#include<hlog/logdef.h>

#include"db_manager.h"
#include"db_sqlite.h"
//#include"addr_book_mgr.h"
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") 

db_manager_t* db_manager_t::_instance = NULL;

db_manager_t::db_manager_t()
#ifdef SYNC_ADDRESS_BOOK
: work_thread_t("db_manager_t", DB_REFRESH_TIMEOUT)
, _db_interface(NULL)
#else
: _db_interface(NULL)
#endif
{
}

db_manager_t::~db_manager_t()
{
    _db_interface = NULL;
}

HPR_INT32 db_manager_t::create_instance()
{
    if (_instance == NULL)
    {
        _instance = new(std::nothrow)db_manager_t();
    }

    return (_instance == NULL ? -1:0);
}

HPR_VOID db_manager_t::destroy_instance()
{
    if (_instance != NULL)
    {
        delete _instance;
        _instance = NULL;
    }

    return;
}

HPR_INT32 db_manager_t::init(sql_def_t::db_type database)
{
    HPR_INT32 ret = -1;
    HPR_BOOL need_rollback = HPR_FALSE;

    //创建数据库对象
    switch (database)
    {
    case sql_def_t::DB_SQLITE:
        _db_interface = new(std::nothrow)db_sqlite_t();
        break;
    default:
        SIP_SVR_ERROR("unkown db type %d!", database);
        break;
    }

    do
    {
        if (_db_interface == NULL)
        {
            SIP_SVR_ERROR("Create db interface faild!Error is %d.", HPR_GetSystemLastError());
            break;
        }

        //打开数据库
        if (_db_interface->connect_db(NULL, 0, "addressbook.db", NULL, NULL) != 0)
        {
            break;
        }

        //创建事物
        if(_db_interface->begin_transaction() != 0)
        {
            break;
        }

        need_rollback = HPR_TRUE;
        //创建数据库表
#ifdef SYNC_ADDRESS_BOOK
        if (_db_interface->create_table(SQL_CREATE_SYNC_SRC_TABLE) != 0)
        {
            break;
        }
#endif

        if (_db_interface->create_table(SQL_CREATE_ADDRESS_BOOK_TABLE) != 0)
        {
            SIP_SVR_ERROR("create table addr_book faild!");
            break;
        }
        if (_db_interface->create_table(SQL_CREATE_CALL_LOG_TABLE) != 0)
        {
            SIP_SVR_ERROR("create table call log faild!");
            break;
        }
        if (_db_interface->create_table(SQL_CREATE_LOCAL_CONFIG_TABLE) != 0)
        {
            SIP_SVR_ERROR("create table local config faild!");
            break;
        }
        if (_db_interface->create_table(SQL_CREATE_REMOTE_CONFIG_TABLE) != 0)
        {
            SIP_SVR_ERROR("create table remote config faild!");
            break;
        }
        if (_db_interface->create_table(SQL_CREATE_NETZONE_TABLE) != 0)
        {
            SIP_SVR_ERROR("create table net zone faild!");
            break;
        }
        //提交事物
        if (_db_interface->commit_transaction() != 0)
        {
            //_db_interface->rollback_transaction();
            break;
        }
#ifdef SYNC_ADDRESS_BOOK
        //启动任务线程
        if (start() != 0)
        {
            break;
        }
#endif

        ret = 0;
        need_rollback = HPR_FALSE;

    }while(0);

    if (ret != 0)
    {
        if (_db_interface != NULL)
        {
            if (need_rollback == HPR_TRUE)
            {
                _db_interface->rollback_transaction();
            }

            _db_interface->disconnet_db();
            delete _db_interface;
            _db_interface = NULL;
        }
    }

    //测试代码
    /*
    *
    *std::list<vt_phone_t> phone_list;
    *vt_phone_t phone;
    *char tmp[8];
    *for (int i=0; i<10000; i++)
    *{
    *    memset(tmp, 0, 8);
    *    _snprintf_s(tmp, 8, _TRUNCATE, "%06d", i);
    *    phone._call_priority = 0;
    *    phone._dev_main_type = 1;
    *    phone._dev_sub_type = 1;
    *    phone._net_zone_id = 3;
    *    phone._phone_no = tmp;
    *    phone_list.push_back(phone);
    *}
    *add_phone_sync(phone_list);
    */


    return ret;
}

HPR_INT32 db_manager_t::finit()
{
    SIP_SVR_INFO("start finit data base ...");
#ifdef SYNC_ADDRESS_BOOK
    //关闭任务线程
    stop();
#endif

    if (_db_interface != NULL)
    {
        _db_interface->disconnet_db();
        delete _db_interface;
        _db_interface = NULL;
    }

    SIP_SVR_INFO("data base closed!");

    return 0;
}

#if 0
HPR_INT32 db_manager_t::load_addressbook()
{
    if (_db_interface == NULL)
    {
        SIP_SVR_ERROR("db interface is null!");
        return -1;
    }

    cursor_interface_t* cursor = _db_interface->create_cursor(SQL_SELECT_ALL_ADDRESS_BOOK);
    if (cursor == NULL)
    {
        return -1;
    }

    vt_phone_t phone;
    cursor->begin();

    while (cursor->is_eof() == HPR_FALSE)
    {
        /////////获取各列数据
        //电话号码
        HPR_Snprintf(phone._phone_num, config_t::MAX_PHONE_NUM_LEN, cursor->get_column_text(1));
        //sip indexcode
        //HPR_Snprintf(phone._sip_indexcode, config_t::MAX_SIP_INDEX_CODE, cursor->get_column_text(2));
        //主设备类型
        phone._dev_main_type = cursor->get_column_int32(2);
        //次设备类型
        phone._dev_sub_type = cursor->get_column_int32(3);
        //呼叫优先级
        phone._call_priority = cursor->get_column_int32(4);
        //设备网域id
        phone._net_zone_id = cursor->get_column_int32(5);

        //增加到通讯录
        addr_book_mgr_t::instance()->add_phone(phone);

        cursor->next();
    }

    delete cursor;

    return 0;
}
#endif

HPR_INT32 db_manager_t::load_addressbook(VT_PHONE_LIST& phone_list, HPR_INT32 count, HPR_INT32 offset, HPR_INT32& read_count)
{
    if (_db_interface == NULL)
    {
        return -1;
    }
    read_count = 0;

    char sql[SQLITE_SELECT_LEN];
    HPR_Snprintf(sql, SQLITE_SELECT_LEN, "select * from address_book limit %d offset %d;", count, offset);
    cursor_interface_t* cursor = _db_interface->create_cursor(sql);
    if (cursor == NULL)
    {
        return -1;
    }

    vt_phone_t phone;
    cursor->begin();

    while (cursor->is_eof() == HPR_FALSE)
    {
        /////////获取各列数据
        //电话号码
        //HPR_Snprintf(phone._phone_no, config_t::MAX_PHONE_NUM_LEN, cursor->get_column_text(1));
        phone._phone_no = cursor->get_column_text(sql_def_t::addrbook_phone_no_idx);
        phone._phone_name = cursor->get_column_text(sql_def_t::addrbook_phone_name_idx);
        //sip indexcode
        //HPR_Snprintf(phone._sip_indexcode, config_t::MAX_SIP_INDEX_CODE, cursor->get_column_text(2));
        //主设备类型
        phone._dev_main_type = cursor->get_column_int32(sql_def_t::addrbook_dev_main_type_idx);
        //次设备类型
        phone._dev_sub_type = cursor->get_column_int32(sql_def_t::addrbook_dev_sub_type_idx);
        //呼叫优先级
        phone._call_priority = cursor->get_column_int32(sql_def_t::addrbook_call_prioriry_idx);
        //设备网域id
        phone._net_zone_id = cursor->get_column_int32(sql_def_t::addrbook_net_zone_id_idx);
        //注册ip
        phone._register_ip = cursor->get_column_text(sql_def_t::addrbook_ip_idx);
        //端口
        phone._register_port = cursor->get_column_int32(sql_def_t::addrbook_port_idx);
        //最近一次注册时间
        phone._last_reg_time = cursor->get_column_int64(sql_def_t::addrbook_last_reg_time_idx);

        phone_list.push_back(phone);

        ++read_count;

        cursor->next();
    }

    delete cursor;

    return 0;
}

HPR_INT32 db_manager_t::get_local_config(db_manager_t::local_config_t& config)
{
    if (_db_interface == NULL)
    {
        SIP_SVR_ERROR("db interface is null!");
        return -1;
    }

    char sql[SQLITE_SELECT_LEN];
    HPR_Snprintf(sql, SQLITE_SELECT_LEN, "select * from local_config where indexcode = '0';");
    select_interface_t* select_obj = _db_interface->select(sql);

    if (select_obj == NULL)
    {
        SIP_SVR_ERROR("create select obj faild!");
        return -1;
    }

    select_obj->begin();

    if(select_obj->end() != 0)
    {
        /////////获取各列数据
        //indexcode，暂时不用
        config._index_code = select_obj->get_column_text(sql_def_t::local_cfg_indexcode_idx);
        //管理中心特殊号码
        config._manager_center_num = select_obj->get_column_text(sql_def_t::local_cfg_mgr_center_no_idx);
        //sip服务器用户名
        config._user_name = select_obj->get_column_text(sql_def_t::local_cfg_user_name_idx);
        //sip服务器密码
        config._pass_word = select_obj->get_column_text(sql_def_t::local_cfg_psw_idx);
        //sip服务器egent
        config._user_agent = select_obj->get_column_text(sql_def_t::local_cfg_user_agent_idx);
        //SIP协议端口
        config._sip_port = static_cast<HPR_UINT16>(select_obj->get_column_int32(sql_def_t::local_cfg_sip_port_idx));
        //webservice 端口
        config._webservice_port = static_cast<HPR_UINT16>(select_obj->get_column_int32(sql_def_t::local_cfg_webservice_port_idx));
        //http端口
        config._http_port = static_cast<HPR_UINT16>(select_obj->get_column_int32(sql_def_t::local_cfg_http_port_idx));
        //网管端口，上报设备状态
        config._net_manager_device_port = static_cast<HPR_UINT16>(select_obj->get_column_int32(sql_def_t::local_cfg_net_manager_device_port_idx));
        //网管端口，上报服务器状态
        config._net_manager_server_port = static_cast<HPR_UINT16>(select_obj->get_column_int32(sql_def_t::local_cfg_net_manager_server_port_idx));
        //最大通话时间
        config._max_talking_time = select_obj->get_column_int32(sql_def_t::local_cfg_max_talk_time_idx);
        //最大未响应时间
        config._max_no_rsp_time = select_obj->get_column_int32(sql_def_t::local_cfg_max_rsp_time_idx);
        //心跳超时时间
        config._hb_timeout = select_obj->get_column_int32(sql_def_t::local_cfg_hb_timeout_idx);
        //注册有效期
        config._valid_register_time = select_obj->get_column_int32(sql_def_t::local_cfg_valid_reg_time_idx);
    }
    else
    {
        delete select_obj;

        return -1;
    }

    delete select_obj;

    return 0;
}

HPR_INT32 db_manager_t::set_local_config(db_manager_t::local_config_t& config)
{
    if (_db_interface == NULL)
    {
        return -1;
    }

    char sql[2048];
    HPR_Snprintf(sql, 2047, "insert into local_config values ('%s', '%s', '%s', '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d)",\
        config._index_code.c_str(), config._manager_center_num.c_str(), config._user_name.c_str(),\
        config._pass_word.c_str(), config._user_agent.c_str(), static_cast<HPR_INT32>(config._sip_port),\
        static_cast<HPR_INT32>(config._webservice_port), static_cast<HPR_INT32>(config._http_port), \
        static_cast<HPR_INT32>(config._net_manager_device_port), static_cast<HPR_INT32>(config._net_manager_server_port),\
        config._max_talking_time, config._max_no_rsp_time, config._hb_timeout, config._valid_register_time);

    if (_db_interface->insert(sql) != 0)
    {
        SIP_SVR_ERROR("Set local config faild!");
        return -1;
    }

    return 0;
}

HPR_INT32 db_manager_t::get_remote_config(remote_config_t& config)
{
    if (_db_interface == NULL)
    {
        return -1;
    }

    char sql[SQLITE_SELECT_LEN];
    //读取配置信息
    HPR_Snprintf(sql, SQLITE_SELECT_LEN, "select * from remote_config where remote_id = '0';");

    select_interface_t* select_obj = _db_interface->select(sql);
    if (select_obj == NULL)
    {
        return -1;
    }

    select_obj->begin();
    //只取第一个
    if(select_obj->end() != 0)
    {
        //indexcode
        config._indexcode = select_obj->get_column_text(sql_def_t::remote_cfg_indexcode_idx);
        //webservice url
        config._remote_addr = select_obj->get_column_text(sql_def_t::remote_cfg_remote_url_idx);
    }

    delete select_obj;

    //读取网域信息
    if (read_net_zones(config._net_zone_info) != 0)
    {
        return -1;
    }

    return 0;
}

HPR_INT32 db_manager_t::set_remote_config(remote_config_t& config)
{
	SIP_SVR_INFO("==>set_remote_config...");
    if (_db_interface == NULL)
    {
        return -1;
    }

    char sql[SQLITE_SELECT_LEN_LONG];

    //读取配置信息
    HPR_Snprintf(sql, SQLITE_SELECT_LEN_LONG, "select count(*) from remote_config where remote_id = '0';");

    if (_db_interface->get_table_count(sql) > 0)
    {
        //配置信息已存在，直接覆盖
        HPR_Snprintf(sql, SQLITE_SELECT_LEN_LONG, "update remote_config set indexcode = '%s', remote_uri = '%s' where remote_id = '0';",\
            config._indexcode.c_str(), config._remote_addr.c_str());

        if (_db_interface->update(sql) != 0)
        {
            SIP_SVR_ERROR("update config faild! sql:%s", sql);
            return -1;
        }
    }
    else
    {
        //配置信息不存在,插入配置信息
        HPR_Snprintf(sql, SQLITE_SELECT_LEN_LONG, "insert into remote_config values('0', '%s', '%s');", config._indexcode.c_str(),\
            config._remote_addr.c_str());

        if (_db_interface->insert(sql) != 0)
        {
            SIP_SVR_ERROR("insert remote config faild!sql:%s",sql);
            return -1;
        }
    }



    //更新local config
    HPR_Snprintf(sql, SQLITE_SELECT_LEN_LONG, "update local_config set sip_port = %d, webservice_port = %d, net_manager_device_port = %d, \
                           net_manager_server_port = %d, user_agent = '%s' where indexcode = '0';", \
                           config._sip_port, config._webservice_port, config._net_manager_device_port, config._net_manager_server_port, config._user_agent.c_str());
    HPR_INT32 update_rt = _db_interface->update(sql);

    if (update_rt != 0)
    {
        SIP_SVR_ERROR("create select obj faild!");
        return -1;
    }

    //写入网域信息
    if (set_net_zones(config._net_zone_info) != 0)
    {
        return -1;
    }

    return 0;
}

#ifdef SYNC_ADDRESS_BOOK
HPR_INT32 db_manager_t::del_phone(const char* phone_no)
{
    char sql[SQLITE_SELECT_LEN];
    HPR_Snprintf(sql, SQLITE_SELECT_LEN, "delete from address_book where phone_no = '%s';", phone_no);

    push_task_to_list(sql);

    return 0;
}


HPR_INT32 db_manager_t::insert_phone(vt_phone_t& phone)
{
    char sql[SQLITE_SELECT_LEN];
    HPR_Snprintf(sql, SQLITE_SELECT_LEN, "insert into address_book values('%s', '%s', %d, %d, %d, %d, '%s', %d, %I64d);", phone._phone_no.c_str(),\
        phone._phone_name.c_str(), phone._dev_main_type, phone._dev_sub_type, phone._call_priority, phone._net_zone_id,\
        phone._register_ip.c_str(), phone._register_port, phone._last_reg_time);

    push_task_to_list(sql);

    return 0;
}

HPR_INT32 db_manager_t::update_phone(vt_phone_t& phone)
{
    char sql[SQLITE_SELECT_LEN];
    HPR_Snprintf(sql, SQLITE_SELECT_LEN, "update address_book set dev_main_type = %d, dev_sub_type = %d,\
                           call_priority = %d, net_zone_id = %d where phone_no = '%s';", phone._dev_main_type,\
                           phone._dev_sub_type, phone._call_priority, phone._net_zone_id, phone._phone_no.c_str());

    push_task_to_list(sql);

    return 0;
}

#endif

HPR_INT32 db_manager_t::add_phone_sync(VT_PHONE_LIST& add_list)
{
    if (_db_interface == NULL)
    {
        return -1;
    }

    char sql[256];

    if(_db_interface->begin_transaction() != 0)
    {
        return -1;
    }

    //增加通讯录
    VT_PHONE_LIST::iterator itor = add_list.begin();
    while (itor != add_list.end())
    {
    	add_single_phone_sync(*itor);
        ++itor;
    }

    //提交事物
    if (_db_interface->commit_transaction() != 0)
    {
        _db_interface->rollback_transaction();

        return -1;
    }

    return 0;
}

HPR_INT32 db_manager_t::add_single_phone_sync(vt_phone_t& phone)
{
    if (_db_interface == NULL)
    {
        return -1;
    }

    char sql[256];

    HPR_Snprintf(sql, 256, "insert or replace into address_book values('%s', '%s', %d, %d, %d, %d, '%s', %d, %llu);", phone._phone_no.c_str(),\
        phone._phone_name.c_str(), phone._dev_main_type, phone._dev_sub_type, phone._call_priority, phone._net_zone_id, phone._register_ip.c_str(),\
        phone._register_port, phone._last_reg_time);
    _db_interface->insert(sql);

    return 0;
}

HPR_INT32 db_manager_t::add_single_phone_sync(device_t& device)
{
    if (_db_interface == NULL)
    {
        return -1;
    }

    char sql[256];
	std::string device_ip;

   	device.get_ip(device_ip);

    HPR_Snprintf(sql, 256, "insert or replace into address_book values('%s', '%s', %d, %d, %d, %d, '%s', %d, %llu);", device.get_phone_number(),\
        "", device.get_main_type(), device.get_sub_type(), device.get_priority(), device.get_net_zone_id(), device_ip.c_str(),\
        device.get_port(), device.get_update_time());
    _db_interface->insert(sql);

    return 0;
}

HPR_INT32 db_manager_t::update_phone_sync(VT_PHONE_LIST& update_list)
{
    if (_db_interface == NULL)
    {
        return -1;
    }

    char sql[256];
    if(_db_interface->begin_transaction() != 0)
    {
        return -1;
    }

    //增加通讯录
    VT_PHONE_LIST::iterator itor = update_list.begin();
    while (itor != update_list.end())
    {
        //HPR_Snprintf(sql, 256, "update address_book set dev_main_type = %d, dev_sub_type = %d,\
        //                       call_priority = %d, net_zone_id = %d where phone_no = '%s';", itor->_dev_main_type,\
        //                       itor->_dev_sub_type, itor->_call_priority, itor->_net_zone_id, itor->_phone_no);

        HPR_Snprintf(sql, 256, "insert or replace into address_book values('%s', '%s', %d, %d, %d, %d, '%s', %d, %I64d);", itor->_phone_no.c_str(),\
            itor->_phone_name.c_str(), itor->_dev_main_type, itor->_dev_sub_type, itor->_call_priority, itor->_net_zone_id, itor->_register_ip.c_str(),\
            itor->_register_port, itor->_last_reg_time);

        _db_interface->update(sql);

        ++itor;
    }

    //提交事物
    if (_db_interface->commit_transaction() != 0)
    {
        _db_interface->rollback_transaction();

        return -1;
    }

    return 0;
}

HPR_INT32 db_manager_t::del_phone_sync(std::list<std::string>& del_list)
{
    if (_db_interface == NULL)
    {
        return -1;
    }

    char sql[SQLITE_SELECT_LEN];

    if(_db_interface->begin_transaction() != 0)
    {
        return -1;
    }

    //删除通讯录
    std::list<std::string>::iterator itor_del = del_list.begin();
    while (itor_del != del_list.end())
    {
        HPR_Snprintf(sql, SQLITE_SELECT_LEN, "delete from address_book where phone_no = '%s';", itor_del->c_str());

        _db_interface->delete_record(sql);

        ++itor_del;
    }

    //提交事物
    if (_db_interface->commit_transaction() != 0)
    {
        _db_interface->rollback_transaction();

        return -1;
    }

    return 0;
}

#ifdef SYNC_ADDRESS_BOOK
HPR_INT32 db_manager_t::load_sync_src(ADDRBOOK_SRC_LIST& addrbook_src_list)
{
    select_interface_t* select_obj = NULL;

    select_obj = _db_interface->select(SQL_SELECT_ALL_SYNC_SRC);
    if (select_obj == NULL)
    {
        return -1;
    }

    ADDRBOOK_SRC addrbook_src;

    select_obj->begin();
    while (select_obj->end() != 0)
    {
        //indexcode
        HPR_Snprintf(addrbook_src._src_indexcode, config_t::MAX_SIP_INDEX_CODE, select_obj->get_column_text(1));
        HPR_Snprintf(addrbook_src._src_ip, config_t::MAX_IP_ADDR_LEN, select_obj->get_column_text(2));

        addrbook_src._src_port = static_cast<HPR_UINT16>(select_obj->get_column_int32(3));
        addrbook_src._protocol = select_obj->get_column_int32(4);

        addrbook_src_list.push_back(addrbook_src);

        select_obj->next();
    }

    delete select_obj;

    return 0;
}

HPR_INT32 db_manager_t::insert_sync_src(const char* indexcode, const char* ip, HPR_UINT16 port, HPR_INT32 protocol)
{
    char sql[SQLITE_SELECT_LEN];

    HPR_Snprintf(sql, SQLITE_SELECT_LEN, "insert into sync_src values('%s', '%s', %d, %d);", indexcode, ip, static_cast<HPR_INT32>(port), protocol);

    push_task_to_list(sql);

    return 0;
}

HPR_INT32 db_manager_t::delete_sync_src(const char* indexcode)
{
    char sql[SQLITE_SELECT_LEN];
    HPR_Snprintf(sql, SQLITE_SELECT_LEN, "delete from sync_src where indexcode = '%s';", indexcode);

    push_task_to_list(sql);

    return 0;
}
#endif

HPR_INT32 db_manager_t::write_call_log_sync(std::list<call_record_t>& call_list)
{
    if (_db_interface == NULL)
    {
        SIP_SVR_ERROR("write log faild!");
        return -1;
    }

    char sql[256];
    //若数据库过大，则删除前面的记录
    HPR_INT32 log_count = get_call_log_count();
    if (log_count == -1)
    {
        SIP_SVR_ERROR("Get call_log record count faild!");
        return -1;
    }

    if (log_count + call_list.size() > config_t::MAX_CALL_RECORD_NUM_DB)
    {
        //删除最前面的记录
        if(del_call_log_sync(call_list.size() + log_count - config_t::MAX_CALL_RECORD_NUM_DB) != 0)
        {
        	SIP_SVR_ERROR("del_call_log_sync failed!\n");
            return -1;
        }
    }

    if (_db_interface->begin_transaction() != 0)
    {
        SIP_SVR_ERROR("Begin transaction faild!");
        return -1;
    }

    std::list<call_record_t>::iterator itor = call_list.begin();
    while (itor != call_list.end())
    {
        HPR_Snprintf(sql, 256, "insert into call_log values('%s', '%s', '%s', %I64d, %I64d, %d);", itor->_call_id.c_str(),\
            itor->_calling_phone.c_str(), itor->_called_phone.c_str(), itor->_start_time, itor->_stop_time, static_cast<HPR_INT32>(itor->_call_status));

        _db_interface->execute(sql);

        ++itor;
    }

    if (_db_interface->commit_transaction() != 0)
    {
        _db_interface->rollback_transaction();

        SIP_SVR_ERROR("Commit transaction faild!");
        return -1;
    }
    else
    {
        return 0;
    }
}

HPR_INT32 db_manager_t::read_call_log_sync(HPR_INT32 read_count, std::list<call_record_t>& call_list)
{
    if (_db_interface == NULL)
    {
        return -1;
    }

    select_interface_t* select_obj = NULL;
    char sql[SQLITE_SELECT_LEN];

    HPR_Snprintf(sql, SQLITE_SELECT_LEN, "select * from call_log limit %d;", read_count);

    select_obj = _db_interface->select(sql);
    if (select_obj == NULL)
    {
        return -1;
    }

    call_record_t call_info;

    select_obj->begin();
    while (select_obj->end() != 0)
    {
        //indexcode
        call_info._call_id = select_obj->get_column_text(sql_def_t::call_log_call_id_idx);
        call_info._calling_phone = select_obj->get_column_text(sql_def_t::call_log_calling_no_idx);
        call_info._called_phone = select_obj->get_column_text(sql_def_t::call_log_called_no_idx);
        call_info._start_time = select_obj->get_column_int64(sql_def_t::call_log_start_time_idx);
        call_info._stop_time = select_obj->get_column_int64(sql_def_t::call_log_stop_time_idx);
        call_info._call_status = static_cast<dev_call_status>(select_obj->get_column_int32(sql_def_t::call_log_event_type_idx));

        call_list.push_back(call_info);

        select_obj->next();
    }

    delete select_obj;

    return 0;
}

HPR_INT32 db_manager_t::del_call_log_sync(HPR_INT32 del_count)
{
    if (_db_interface == NULL)
    {
        return -1;
    }

    char sql[SQLITE_SELECT_LEN];
    HPR_Snprintf(sql, SQLITE_SELECT_LEN, "delete from call_log where call_id in (select call_id from call_log limit %d);", del_count);

    if (_db_interface->execute(sql) != 0)
    {
        SIP_SVR_ERROR("delete call log faild!");
        return -1;
    }
    else
    {
        return 0;
    }
}

HPR_INT32 db_manager_t::get_call_log_count()
{
    if (_db_interface == NULL)
    {
        return -1;
    }
    //若数据库过大，则删除前面的记录
    return _db_interface->get_table_count("select count(*) from call_log;");
}

HPR_INT32 db_manager_t::trancate_addrbook()
{
    if (_db_interface == NULL)
    {
        return -1;
    }
    return _db_interface->trancate_table("address_book");
}

HPR_INT32 db_manager_t::read_net_zones(std::list<net_zone_t>& net_zone_list)
{
    if (_db_interface == NULL)
    {
        return -1;
    }

    select_interface_t* select_obj = NULL;
    char sql[SQLITE_SELECT_LEN];

    HPR_Snprintf(sql, SQLITE_SELECT_LEN, "select * from net_zone;");

    select_obj = _db_interface->select(sql);
    if (select_obj == NULL)
    {
        return -1;
    }

    net_zone_t net_zone;
    select_obj->begin();
    while (select_obj->end() != 0)
    {
        //网域id
        net_zone._id = select_obj->get_column_int32(sql_def_t::net_zone_id_idx);
        //网域ip
        net_zone._ip = select_obj->get_column_text(sql_def_t::net_zone_ip_idx);
        //端口
        net_zone._port = static_cast<HPR_UINT16>(select_obj->get_column_int32(sql_def_t::net_zone_port_idx));
        net_zone_list.push_back(net_zone);

        select_obj->next();
    }

    delete select_obj;

    return 0;
}


char *db_manager_t::get_local_ip_str(char *ip)
{
	if ( NULL == ip)
	{
		SIP_SVR_ERROR("get_local_ip_str IP is NULL!");
		return ip;
	}
	char hostname[256]={0};
	gethostname(hostname,256);

	hostent *pHost = gethostbyname(hostname);
	in_addr addr;

	char *p = pHost->h_addr_list[0];
	memcpy(&addr.S_un.S_addr,p,pHost->h_length);

	strcpy(ip, inet_ntoa(addr));
	SIP_SVR_INFO("get_local_ip_str :%s",ip);
	return ip;
}
HPR_INT32 db_manager_t::set_net_zones(std::list<net_zone_t>& net_zone_list)
{
	SIP_SVR_INFO("==>set_net_zones...");
    if (_db_interface == NULL)
    {
        return -1;
    }

    char sql[SQLITE_SELECT_LEN];
    //先清空网域表
    _db_interface->trancate_table("net_zone");

    //插入网域信息
    std::list<net_zone_t>::iterator itor = net_zone_list.begin();
    while (itor != net_zone_list.end())
    {
        HPR_Snprintf(sql, SQLITE_SELECT_LEN, "insert into net_zone values('%d', '%s', '%d');", itor->_id,\
            itor->_ip.c_str(), static_cast<HPR_INT32>(itor->_port));

        if(_db_interface->insert(sql) != 0)
        {
            SIP_SVR_ERROR("insert net zone faild!");
            return -1;
        }

        ++itor;
    }

    return 0;
}

HPR_INT32 db_manager_t::modify_psw(const char* psw)
{
    if (_db_interface == NULL)
    {
        return -1;
    }

    //查找表是否存在
    char sql[SQLITE_SELECT_LEN];
    HPR_Snprintf(sql, SQLITE_SELECT_LEN, "select count(*) from local_config where indexcode = '0';");

    if (_db_interface->get_table_count(sql) > 0)
    {
        //更新密码
        HPR_Snprintf(sql, SQLITE_SELECT_LEN, "update local_config set pass_word = '%s' where indexcode = '0';", psw);

        if (_db_interface->update(sql) == 0)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        //本地配置表不存在，返回失败
        SIP_SVR_ERROR("modify psw faild!local_config table is empty!");
        return -1;
    }
}

HPR_INT32 db_manager_t::write_reg_status(std::list<register_status_t>& reg_status_list)
{
    if (_db_interface == NULL)
    {
        return -1;
    }

    char sql[SQLITE_SELECT_LEN];
    std::list<register_status_t>::iterator itor = reg_status_list.begin();
    if (_db_interface->begin_transaction() != 0)
    {
        SIP_SVR_ERROR("begin transaction faild!");
        return -1;
    }

    while (itor != reg_status_list.end())
    {
        HPR_Snprintf(sql, SQLITE_SELECT_LEN-1, "update address_book set ip = '%s', port = %d, last_reg_time = %I64d where phone_no = '%s';",\
            itor->_ip.c_str(), static_cast<HPR_INT32>(itor->_port), itor->_time, itor->_phone_no.c_str());

        if (_db_interface->update(sql) != 0)
        {
            SIP_SVR_ERROR("update reg persistence into faild!");
            break;
        }

        ++itor;
    }

    if (_db_interface->commit_transaction() != 0)
    {
        _db_interface->rollback_transaction();

        SIP_SVR_ERROR("commit transaction faild!");
        return -1;
    }

    return 0;
}

#ifdef SYNC_ADDRESS_BOOK
HPR_INT32 db_manager_t::push_task_to_list(const char* task)
{
    HPR_Guard sql_lock(&_sql_list_lock);
    if (_sql_list.size() >= MAX_TASK_COUNT_IN_LIST)
    {
        //删除最前边一条任务
        _sql_list.erase(_sql_list.begin());
        SIP_SVR_WARN("Task list > %d.", MAX_TASK_COUNT_IN_LIST);
    }

    //放到任务链表最后
    _sql_list.push_back(task);

    //若任务数 > 500,立即执行任务
    if (_sql_list.size() > EXE_IMMEDIATELY_TASK_NUM)
    {
        execute_immediately();
    }

    return 0;
}

HPR_VOID db_manager_t::work(HPR_VOID)
{
    if (_db_interface == NULL)
    {
        return;
    }

    std::list<std::string>::iterator itor;
    //每个事物最多1000个sql语句
    _db_interface->begin_transaction();

    {
        HPR_Guard sql_lock(&_sql_list_lock);

        itor = _sql_list.begin();

        for (HPR_INT32 i = 0; i < 1000; ++i)
        {
            if (itor == _sql_list.end())
            {
                break;
            }

            if(_db_interface->execute(itor->c_str()) != 0)
            {
                //此时需要使迭代器指向前一个位置，因为当前位置不会被提交
                --itor;
                SIP_SVR_ERROR("Execute sql in transaction faild!");
                break;
            }

            ++itor;
        }
    }

    //提交事物
    if (_db_interface->commit_transaction() == 0)
    {
        HPR_Guard sql_lock(&_sql_list_lock);

        //删除提交成功的sql语句
        _sql_list.erase(_sql_list.begin(), itor);

        //若任务数 < 500，挂起线程，等待超时执行
        if (_sql_list.size() < EXE_IMMEDIATELY_TASK_NUM)
        {
            execute_timeout();
        }
    }
    else
    {
        _db_interface->rollback_transaction();
    }

    return;
}
#endif
