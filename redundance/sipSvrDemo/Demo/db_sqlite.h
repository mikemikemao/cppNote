/** @file:   db_sqlite.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  sqlite���ݿ�����ӿ�
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
     @brief:  ���ӻ�����ݿ�
     @param: ip -- ���ݿ�ip��ַ��sqlite���Դ˲���
             port -- ���ݿ�˿ڣ�sqlite���Դ˲���
             name -- ���ݿ����ƣ�sqlite��Ϊ���ݿ��ļ�����
             user_name -- �û���������
             psw -- ���룬����
             option -- ��ѡ��������Ҫ���ڼ����������ݿ⣬��ʱ����
             res -- ͬ��
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 connect_db(const char* ip, HPR_UINT16 port, const char* name, const char* user_name, const char* psw, const char* options  = NULL , const char* res  = NULL );

    /**
     @function:disconnet_db 
     @brief:�Ͽ����ݿ�����
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 disconnet_db();

    //�������
    /**
     @function:begin_transaction 
     @brief:��ʼһ������
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 begin_transaction();

    /**
     @function:commit_transaction 
     @brief:�ύ����
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 commit_transaction();

    /**
     @function:rollback_transaction 
     @brief:�ع����
     @param:
     @notes:�ڿ�ʼ����ʱ��������ݿ�������ύ����ɹ�������ݿ���������ύʧ�ܺ󲻽�����
            ��ˣ����ύʧ�ܺ󣬱���Ҫ���ûع��������ݿ������
     @returns:sucess 0,other -1
     **/
    HPR_INT32 rollback_transaction();

    //������
    /**
     @function:create_table 
     @brief: ������
     @param: sql_table -- �������sql���
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 create_table(const char* sql_table);

    /**
     @function:delete_table 
     @brief:ɾ����
     @param:slq_table -- ɾ�����sql���
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 delete_table(const char* sql_table);

    //����
    /**
     @function:insert 
     @brief:�����¼
     @param:sql_insert -- �������
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 insert(const char* sql_insert);

    /**
     @function:update 
     @brief:���¼�¼
     @param:sql_update -- �������
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 update(const char* sql_update);

    /**
     @function: select
     @brief:��ѯ����
     @param:
     @notes: ��ѯ(����null��ʾʧ�ܣ�������ʾ��ѯ�������Ҫ�ô˾����ȡ��ѯ���)
     **/
    select_interface_t* select(const char* sql_select);

    /**
     @function:delete_record 
     @brief:ɾ����¼
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 delete_record(const char* sql_del);

    /**
     @function: create_cursor
     @brief:����һ���α�
     @param:
     @notes:��select��ȣ���Ҫ������ȡ�������ǳ���ļ�¼
     @returns:sucess 0,other -1
     **/
    cursor_interface_t* create_cursor(const char* sql_select);

    /**
     @function:get_table_count 
     @brief:��ȡ�������
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 get_table_count(const char* sql_count);

    /**
     @function:execute 
     @brief:ִ��һ��sql��䣬�޷��ؽ��
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 execute(const char* sql_exe);

    /**
     @function:table_exist 
     @brief: �ж�һ�����Ƿ����
     @param:table_name -- ����
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 table_exist(const char* table_name);

    /**
     @function:trancate_table 
     @brief:���һ����
     @param:table_name -- ����
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 trancate_table(const char* table_name);

    /**
     @function:add_column_to_table 
     @brief: ���������һ��
     @param: table_name -- ����
             new_column_name -- ������
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 add_column_to_table(const char* table_name, const char* new_column_name);

private:

    //���ݿ����Ӿ��
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
     @brief: ��ȡ���������
     @param: row -- ���ؽ��������
             colum -- ���ؽ��������
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 get_result_num(HPR_INT32& row, HPR_INT32& column);

    /**
     @function: get_column_text
     @brief: ��ȡ��¼ĳ�е��ַ���ֵ
     @param: column--����������0��ʼ
     @notes:
     @returns:sucess 0,other -1
     **/
    const char* get_column_text(HPR_INT32 column);

    /**
     @function: get_column_int32
     @brief: ��ȡ��¼ĳ�е�32λ����ֵ
     @param: column--����������0��ʼ
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 get_column_int32(HPR_INT32 column);

    /**
     @function: get_column_int64
     @brief: ��ȡ��¼ĳ�е�64λ����ֵ
     @param: column--����������0��ʼ
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT64 get_column_int64(HPR_INT32 column);

    /**
     @function: begin
     @brief: ��ʼ��ȡ�����
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 begin();

    /**
     @function: next
     @brief: ָ����һ����¼
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 next();

    /**
     @function: end
     @brief: �Ƿ񵽼�¼ĩβ
     @param:
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 end();

    /**
     @function: clear
     @brief: ������β�ѯ���
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
     @brief: ��ȡ��¼ĳ�е��ַ���ֵ
     @param: column--����������0��ʼ
     @notes:
     @returns:sucess 0,other -1
     **/
    const char* get_column_text(HPR_INT32 column);

    /**
     @function: get_column_int32
     @brief: ��ȡ��¼ĳ�е�32λ����ֵ
     @param: column--����������0��ʼ
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT32 get_column_int32(HPR_INT32 column);

    /**
     @function: get_column_int64
     @brief: ��ȡ��¼ĳ�е�64λ����ֵ
     @param: column--����������0��ʼ
     @notes:
     @returns:sucess 0,other -1
     **/
    HPR_INT64 get_column_int64(HPR_INT32 column);

    /**
     @function: is_eof
     @brief: �Ƿ񵽼�¼ĩβ
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
