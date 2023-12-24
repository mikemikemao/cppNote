/**
 * @file sql_exe.h
 * @brief 数据库处理类
 * @author zhanghaifengyf1@hikvision.com.cn
 * @version 0.0.1
 * @date 2014-07-08 16:28:50
 */

#ifndef SQL_EXE_Y1R08JS8
#define SQL_EXE_Y1R08JS8

class sql_exe_t
{
private:
    sql_exe_t (/* arguments*/);
    virtual ~sql_exe_t ();

public:
    static sql_exe_t* instance();
    static HPR_VOID del_instance();
    HPR_INT32 init();
    HPR_INT32 fini();

private:
    static sql_exe_t* _instance;
} /* sql_exe_t */;

#endif /* end of include guard: SQL_EXE_Y1R08JS8 */
