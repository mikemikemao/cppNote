/** @file:   db_sqlite.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  sqlite数据库操作接口
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

#ifndef _DB_SQLITE_H_
#define _DB_SQLITE_H_

#include<sqlite/sqlite3.h>

#include"db_interface.h"
#include"read_write_lock.h"

#define SQLITE_SELECT_LEN 128
#define SQLITE_SELECT_LEN_LONG 512

class db_sqlite_t
    : public db_interface_t
{
public:
    db_sqlite_t();
    virtual ~db_sqlite_t();

public:

    /**
     @function: connect_db 
     @brief:  连接或打开数据库
     @param: ip -- 数据库ip地址，sqlite忽略此参数
             port -- 数据库端口，sqlite忽略此参数
             name -- 数据库名称，sqlite中为数据库文件名称
             user_name -- 用户名，忽略
             psw -- 密码，忽略
             option -- 可选参数，主要用于兼容其他数据库，暂时不用
             res -- 同上
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 connect_db(const char* ip, HPR_UINT16 port, const char* name, const char* user_name, const char* psw, const char* options  = NULL , const char* res  = NULL );

    /**
     @function:disconnet_db 
     @brief:断开数据库连接
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 disconnet_db();

    //事物操作
    /**
     @function:begin_transaction 
     @brief:开始一个事物
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 begin_transaction();

    /**
     @function:commit_transaction 
     @brief:提交事物
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 commit_transaction();

    /**
     @function:rollback_transaction 
     @brief:回滚事物。
     @param:
     @notes:在开始事物时，会对数据库加锁，提交事物成功后对数据库解锁，但提交失败后不解锁；
            因此，在提交失败后，必须要调用回滚，对数据库解锁。
     @returns:sucess 0,other -1
     **/
    HPR_INT32 rollback_transaction();

    //创建表
    /**
     @function:create_table 
     @brief: 创建表
     @param: sql_table -- 创建表的sql语句
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 create_table(const char* sql_table);

    /**
     @function:delete_table 
     @brief:删除表
     @param:slq_table -- 删除表的sql语句
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 delete_table(const char* sql_table);

    //插入
    /**
     @function:insert 
     @brief:插入记录
     @param:sql_insert -- 插入语句
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 insert(const char* sql_insert);

    /**
     @function:update 
     @brief:更新记录
     @param:sql_update -- 更新语句
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 update(const char* sql_update);

    /**
     @function: select
     @brief:查询操作
     @param:
     @notes: 查询(返回null表示失败，其他表示查询句柄，需要用此句柄提取查询结果)
     **/
    select_interface_t* select(const char* sql_select);

    /**
     @function:delete_record 
     @brief:删除记录
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 delete_record(const char* sql_del);

    /**
     @function: create_cursor
     @brief:创建一个游标
     @param:
     @notes:与select相比，主要用来读取数据量非常大的记录
     @returns:sucess 0,other -1
     **/
    cursor_interface_t* create_cursor(const char* sql_select);

    /**
     @function:get_table_count 
     @brief:获取表的行数
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 get_table_count(const char* sql_count);

    /**
     @function:execute 
     @brief:执行一个sql语句，无返回结果
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 execute(const char* sql_exe);

    /**
     @function:table_exist 
     @brief: 判断一个表是否存在
     @param:table_name -- 表名
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 table_exist(const char* table_name);

    /**
     @function:trancate_table 
     @brief:清空一个表
     @param:table_name -- 表名
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 trancate_table(const char* table_name);

    /**
     @function:add_column_to_table 
     @brief: 向表中增加一列
     @param: table_name -- 表名
             new_column_name -- 新列名
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 add_column_to_table(const char* table_name, const char* new_column_name);

private:

    //数据库连接句柄
    sqlite3* _db_handle;
    read_write_lock_t _db_rwlock;
    
};

class select_sqlite_t
    : public select_interface_t
{
public:

    select_sqlite_t(char** table_data, HPR_INT32 row, HPR_INT32 column);
    virtual ~select_sqlite_t();

public:

    //HPR_INT32 do_select(const char* sql_select);
    /**
     @function: get_result_num 
     @brief: 获取结果的数量
     @param: row -- 返回结果的行数
             colum -- 返回结果的列数
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 get_result_num(HPR_INT32& row, HPR_INT32& column);

    /**
     @function: get_column_text
     @brief: 获取记录某列的字符串值
     @param: column--列索引，从0开始
     @notes:
     @returns:sucess 0,other -1
     **/
    const char* get_column_text(HPR_INT32 column);

    /**
     @function: get_column_int32
     @brief: 获取记录某列的32位整形值
     @param: column--列索引，从0开始
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 get_column_int32(HPR_INT32 column);

    /**
     @function: get_column_int64
     @brief: 获取记录某列的64位整形值
     @param: column--列索引，从0开始
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT64 get_column_int64(HPR_INT32 column);

    /**
     @function: begin
     @brief: 开始获取结果接
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 begin();

    /**
     @function: next
     @brief: 指向下一条记录
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 next();

    /**
     @function: end
     @brief: 是否到记录末尾
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 end();

    /**
     @function: clear
     @brief: 清除本次查询结果
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_VOID clear();

private:

    char** _table_data;
    HPR_INT32 _row;
    HPR_INT32 _column;
    HPR_INT32 _cur_row;

};

class cursor_sqlite_t
    : public cursor_interface_t
{
public:
    cursor_sqlite_t(sqlite3_stmt* stmt);
    virtual ~cursor_sqlite_t();

public:

    HPR_INT32 begin();
    HPR_INT32 next();

    /**
     @function: get_column_text
     @brief: 获取记录某列的字符串值
     @param: column--列索引，从0开始
     @notes:
     @returns:sucess 0,other -1
     **/
    const char* get_column_text(HPR_INT32 column);

    /**
     @function: get_column_int32
     @brief: 获取记录某列的32位整形值
     @param: column--列索引，从0开始
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 get_column_int32(HPR_INT32 column);

    /**
     @function: get_column_int64
     @brief: 获取记录某列的64位整形值
     @param: column--列索引，从0开始
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT64 get_column_int64(HPR_INT32 column);

    /**
     @function: is_eof
     @brief: 是否到记录末尾
     @param:
     @notes:
     @returns:sucess HPR_TRUE,other HPR_FALSE
     **/
    HPR_BOOL is_eof();

private:

    sqlite3_stmt* _stmt;
    HPR_BOOL _eof;

};

#endif
