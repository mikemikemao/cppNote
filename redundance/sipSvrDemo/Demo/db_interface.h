/** @file:   db_interface.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  ���ݿ����ӿڶ���
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

#ifndef _DB_INTERFACE_H_
#define _DB_INTERFACE_H_

#include<hpr/HPR_Hpr.h>

#define DB_MAX_SELECT_RESULT_NUM       32

class select_interface_t
{
public:
    select_interface_t(){}
    virtual ~select_interface_t(){}

public:

    virtual HPR_INT32 get_result_num(HPR_INT32& row, HPR_INT32& column) = 0;
    virtual const char* get_column_text(HPR_INT32 column) = 0;
    virtual HPR_INT32 get_column_int32(HPR_INT32 column) = 0;
    virtual HPR_INT64 get_column_int64(HPR_INT32 column) = 0;

    virtual HPR_INT32 begin() = 0;
    virtual HPR_INT32 next() = 0;
    virtual HPR_INT32 end() = 0;

    virtual HPR_VOID clear() = 0;

};

class cursor_interface_t
{
public:
    cursor_interface_t(){}
    virtual ~cursor_interface_t(){}

public:

    virtual HPR_INT32 begin() = 0;
    virtual HPR_INT32 next() = 0;
    virtual const char* get_column_text(HPR_INT32 column) = 0;
    virtual HPR_INT32 get_column_int32(HPR_INT32 column) = 0;
    virtual HPR_INT64 get_column_int64(HPR_INT32 column) = 0;
    virtual HPR_BOOL is_eof() = 0;

};

class db_interface_t
{
public:
    db_interface_t(){}
    virtual ~db_interface_t(){}

public:
    
    //�������ݿ�
    virtual HPR_INT32 connect_db(const char* ip, HPR_UINT16 port, const char* name, const char* user_name, const char* psw, const char* options = NULL, const char* res = NULL) = 0;
    virtual HPR_INT32 disconnet_db() = 0;

    //�������
    virtual HPR_INT32 begin_transaction() = 0;
    virtual HPR_INT32 commit_transaction() = 0;
    virtual HPR_INT32 rollback_transaction() = 0;

    //������
    virtual HPR_INT32 create_table(const char* sql_table) = 0;
    virtual HPR_INT32 delete_table(const char* sql_table) = 0;

    //����
    virtual HPR_INT32 insert(const char* sql_insert) = 0;
    //����
    virtual HPR_INT32 update(const char* sql_update) = 0;
    
    //��ѯ(����null��ʾʧ�ܣ�������ʾ��ѯ�������Ҫ�ô˾����ȡ��ѯ���)
    virtual select_interface_t* select(const char* sql_select) = 0;

    //ɾ��
    virtual HPR_INT32 delete_record(const char* sql_del) = 0;

    //�����α�
    virtual cursor_interface_t* create_cursor(const char* sql_select) = 0;

    //��ȡ��������
    virtual HPR_INT32 get_table_count(const char* sql_count) = 0;

    //ִ��sql���
    virtual HPR_INT32 execute(const char* sql_exe) = 0;
    //�жϱ��Ƿ����
    virtual HPR_INT32 table_exist(const char* table_name) = 0;
    //�����
    virtual HPR_INT32 trancate_table(const char* table_name) = 0;

    //��������һ��
    virtual HPR_INT32 add_column_to_table(const char* table_name, const char* new_column_name) = 0;

};

#endif