#include<hpr/HPR_String.h>
#include<hpr/HPR_Error.h>
#include<hpr/HPR_Utils.h>
#include<hlog/logdef.h>

#include"db_sqlite.h"

db_sqlite_t::db_sqlite_t()
: _db_handle(NULL)
{
}

db_sqlite_t::~db_sqlite_t()
{
    _db_handle = NULL;
}

HPR_INT32 db_sqlite_t::connect_db(const char* ip, HPR_UINT16 port, const char* name, const char* user_name, const char* psw, const char* options /* = NULL  */, const char* res /* = NULL */ )
{
    HPR_INT32 ret = -1;
    do 
    {
        {
            if (name == NULL)
            {
                SIP_SVR_ERROR("Open db faild! name is null!");
                break;
            }

            read_write_lock_t::write_lock_t w_lock(&_db_rwlock);

            HPR_INT32 open_ret = sqlite3_open(name, &_db_handle);

            if (open_ret != 0)
            {
                SIP_SVR_ERROR("Open db(%s) faild!Error: %d", name, HPR_GetSystemLastError());
                break;
            }
        }

        ret = 0;

    } while (0);

    if (ret != 0)
    {
        disconnet_db();
    }

    return ret;
}

HPR_INT32 db_sqlite_t::disconnet_db()
{
    //关闭数据库
    {
        read_write_lock_t::write_lock_t w_lock(&_db_rwlock);

        if(_db_handle != NULL)
        {
            sqlite3_close(_db_handle);
            _db_handle = NULL;
        }
    }

    return 0;
}

HPR_INT32 db_sqlite_t::begin_transaction()
{
    char* error_msg = NULL;
    //read_write_lock_t::write_lock_t w_lock(&_db_rwlock);
	//整个事物过程需要加锁，因此，总是在事物开始时加写锁，在事物提交成功后解锁
	//若事物提交失败，则在rollback中解锁
	_db_rwlock.enter_write();
    if (sqlite3_exec(_db_handle, "begin transaction", NULL, NULL, &error_msg) != SQLITE_OK)
    {
		_db_rwlock.leave_write();
        SIP_SVR_ERROR("Execute [begin transaction] faild! Error is %s.", (error_msg == NULL ? "" : error_msg));
        sqlite3_free(error_msg);

        return -1;
    }

    return 0;
}

HPR_INT32 db_sqlite_t::commit_transaction()
{
    if(execute("commit transaction") != 0)
	{
		SIP_SVR_ERROR("commit faild!");
		return -1;
	}
	else
	{
		_db_rwlock.leave_write();
		return 0;
	}
}

HPR_INT32 db_sqlite_t::rollback_transaction()
{
    HPR_INT32 ret = execute("rollback transaction");
	_db_rwlock.leave_write();

	return ret;
}

HPR_INT32 db_sqlite_t::create_table(const char* sql_table)
{
    return execute(sql_table);
}

HPR_INT32 db_sqlite_t::delete_table(const char* sql_table)
{
    return execute(sql_table);
}

HPR_INT32 db_sqlite_t::insert(const char* sql_insert)
{
    return execute(sql_insert);
}

HPR_INT32 db_sqlite_t::update(const char* sql_update)
{
    return execute(sql_update);
}


HPR_INT32 db_sqlite_t::delete_record(const char* sql_del)
{
    return execute(sql_del);
}

select_interface_t* db_sqlite_t::select(const char* sql_select)
{
    if (sql_select == NULL)
    {
        SIP_SVR_ERROR("Select faild! Sql is null!");
        return NULL;
    }

    char** table_data = NULL;
    HPR_INT32 row = 0;
    HPR_INT32 column = 0;
    char* error_msg = NULL;
    select_interface_t* select_ret = NULL;
    do
    {
        read_write_lock_t::read_lock_t r_lock(&_db_rwlock);
        if(sqlite3_get_table(_db_handle, sql_select, &table_data, &row, &column, &error_msg) != SQLITE_OK)
        {
            SIP_SVR_ERROR("Execute [%s] faild! Error is %s.", sql_select, (error_msg == NULL ? "" : error_msg));
            sqlite3_free(error_msg);
            break;
        }

        select_ret = new(std::nothrow)select_sqlite_t(table_data, row, column);
        if (select_ret == NULL)
        {
            SIP_SVR_ERROR("New select result faild!Error is %d.", HPR_GetSystemLastError());
            break;
        }

    }while(0);

    if (select_ret == NULL)
    {
        if (table_data != NULL)
        {
            sqlite3_free_table(table_data);
        }
    }

    return select_ret;
}

cursor_interface_t* db_sqlite_t::create_cursor(const char* sql_select)
{
    if (sql_select == NULL)
    {
        SIP_SVR_ERROR("Create cursor faild! Sql is null!");
        return NULL;
    }

    sqlite3_stmt* stmt = NULL;
    cursor_interface_t* cursor = NULL;

    do 
    {
        read_write_lock_t::read_lock_t r_lock(&_db_rwlock);
        if (sqlite3_prepare_v2(_db_handle, sql_select, -1, &stmt, NULL) != SQLITE_OK)
        {
			SIP_SVR_ERROR("sqlite prepare faild!sql is %s.", sql_select);
            break;
        }

        cursor = new(std::nothrow)cursor_sqlite_t(stmt);
        if (cursor == NULL)
        {
            SIP_SVR_ERROR("New select result faild!Error is %d.", HPR_GetSystemLastError());
            break;
        }

    } while (0);

    if (cursor == NULL)
    {
        if (stmt != NULL)
        {
            sqlite3_finalize(stmt);
        }
    }

    return cursor;
}

HPR_INT32 db_sqlite_t::get_table_count(const char* sql_count)
{
    if (sql_count == NULL)
    {
        SIP_SVR_ERROR("Get table count faild! Sql is null!");
        return -1;
    }

    char** table_data = NULL;
    HPR_INT32 row = 0;
    HPR_INT32 column = 0;
    char* error_msg = NULL;

    read_write_lock_t::read_lock_t r_lock(&_db_rwlock);
    if(sqlite3_get_table(_db_handle, sql_count, &table_data, &row, &column, &error_msg) != SQLITE_OK)
    {
        SIP_SVR_ERROR("Get table [%s] faild! Error is %s.", sql_count, (error_msg == NULL ? "" : error_msg));
        sqlite3_free(error_msg);

        return -1;
    }

    HPR_INT32 count = HPR_Atoi32(*(table_data + 1*column + 0));

    sqlite3_free_table(table_data);

    return count;
}


HPR_INT32 db_sqlite_t::execute(const char* sql_exe)
{
    if (sql_exe == NULL)
    {
        SIP_SVR_ERROR("Execute sql faild! Sql is null!");
        return -1;
    }

    char* error_msg = NULL;
    read_write_lock_t::write_lock_t r_lock(&_db_rwlock);
    if (sqlite3_exec(_db_handle, sql_exe, NULL, NULL, &error_msg) != SQLITE_OK)
    {
        SIP_SVR_ERROR("Execute [%s] faild! Error is %s.", sql_exe, (error_msg == NULL ? "" : error_msg));
        sqlite3_free(error_msg);

        return -1;
    }

    return 0;
}

HPR_INT32 db_sqlite_t::table_exist(const char* table_name)
{
    if (table_name == NULL)
    {
        SIP_SVR_ERROR("Check table exit faild! table name is null!");
        return -1;
    }

    char sql[SQLITE_SELECT_LEN];
    HPR_Snprintf(sql, SQLITE_SELECT_LEN, "select count(*) from sqlite_master where type = 'table' and name = '%s'", table_name);

    if (get_table_count(sql) > 0)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

HPR_INT32 db_sqlite_t::trancate_table(const char* table_name)
{
    if (table_name == NULL)
    {
        SIP_SVR_ERROR("trancate faild! table name is null!");
        return -1;
    }

    char sql[SQLITE_SELECT_LEN];
    HPR_Snprintf(sql, SQLITE_SELECT_LEN, "delete from %s;", table_name);

    return execute(sql);
}

HPR_INT32 db_sqlite_t::add_column_to_table(const char* table_name, const char* new_column_name)
{
    if (table_name == NULL || new_column_name == NULL)
    {
        SIP_SVR_ERROR("table_name or new_column_name is null!");
        return -1;
    }

    char sql[SQLITE_SELECT_LEN];
    HPR_Snprintf(sql, SQLITE_SELECT_LEN, "alter table %s add column name %s;", table_name, new_column_name);

    return execute(sql);
}


select_sqlite_t::select_sqlite_t(char** table_data, HPR_INT32 row, HPR_INT32 column)
: _table_data(table_data)
, _row(row)
, _column(column)
, _cur_row(1)
{
}

select_sqlite_t::~select_sqlite_t()
{
    try
    {
        if (_table_data != NULL)
        {
            sqlite3_free_table(_table_data);
        }
    }
    catch(...)
    {
    }
}


HPR_INT32 select_sqlite_t::get_result_num(HPR_INT32& row, HPR_INT32& column)
{
    if(_table_data == NULL)
    {
        return -1;
    }

    row = _row;
    column = _column;

    return 0;
}

const char* select_sqlite_t::get_column_text(HPR_INT32 column)
{
    if (_table_data == NULL)
    {
        SIP_SVR_ERROR("Get value faild!table_data is null!");
        return NULL;
    }

    return *(_table_data + _cur_row*_column + column - 1);
}

HPR_INT32 select_sqlite_t::get_column_int32(HPR_INT32 column)
{
    if (_table_data == NULL)
    {
        SIP_SVR_ERROR("Get value faild!table_data is null!");
        return NULL;
    }

    return HPR_Atoi32(*(_table_data + _cur_row*_column + column - 1));
}

HPR_INT64 select_sqlite_t::get_column_int64(HPR_INT32 column)
{
    if (_table_data == NULL)
    {
        SIP_SVR_ERROR("Get value faild!table_data is null!");
        return NULL;
    }

    return HPR_Atoi64(*(_table_data + _cur_row*_column + column - 1));
}

HPR_INT32 select_sqlite_t::begin()
{
    _cur_row = 1;
    return 0;
}

HPR_INT32 select_sqlite_t::next()
{
    if (_cur_row > _row)
    {
        return -1;
    }

    ++_cur_row;
    return 0;
}

HPR_INT32 select_sqlite_t::end()
{
    return (_cur_row > _row ? 0:-1);
}

HPR_VOID select_sqlite_t::clear()
{
    if (_table_data != NULL)
    {
        sqlite3_free_table(_table_data);
        _table_data = NULL;
    }

    _row = 0;
    _cur_row = 1;
    _column = 0;
    return;
}

cursor_sqlite_t::cursor_sqlite_t(sqlite3_stmt* stmt)
: _stmt(stmt)
, _eof(HPR_FALSE)
{
}

cursor_sqlite_t::~cursor_sqlite_t()
{
    try
    {
        sqlite3_finalize(_stmt);
    }
    catch(...)
    {
    }
}

HPR_INT32 cursor_sqlite_t::begin()
{
    if (sqlite3_step(_stmt) != SQLITE_ROW)
    {
        _eof = HPR_TRUE;
    }

    return 0;
}

HPR_INT32 cursor_sqlite_t::next()
{
    if (sqlite3_step(_stmt) != SQLITE_ROW)
    {
        _eof = HPR_TRUE;
    }
    return 0;
}

const char* cursor_sqlite_t::get_column_text(HPR_INT32 column)
{
    if (column < 1)
    {
        SIP_SVR_ERROR("column = %d < 1!", column);
        return NULL;
    }

    return (const char*)(sqlite3_column_text(_stmt, column - 1));
}

HPR_INT32 cursor_sqlite_t::get_column_int32(HPR_INT32 column)
{
    if (column < 1)
    {
        SIP_SVR_ERROR("column = %d < 1!", column);
        return NULL;
    }

    return HPR_Atoi32((const char*)sqlite3_column_text(_stmt, column - 1));
}

HPR_INT64 cursor_sqlite_t::get_column_int64(HPR_INT32 column)
{
    if (column < 1)
    {
        SIP_SVR_ERROR("column = %d < 1!", column);
        return NULL;
    }

    return HPR_Atoi64((const char*)sqlite3_column_text(_stmt, column - 1));
}

HPR_BOOL cursor_sqlite_t::is_eof()
{
    return _eof;
}