/** @file:   register_persistence.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  ×¢²á×´Ì¬¹ÜÀí
 *
 *  @author: wangfeida@hikvision.com.cn
 *  @data:   2014/08/20
 *  
 *  @note:   
 *
 *  @Modification History:
 *  <version>       <time>          <author>        <desc>
 *    V1.0        2014/08/20        wangfeida       Create
 *
 *  @warning: <No waining>        
*/

#ifndef _REGISTER_STATUS_H_
#define _REGISTER_STATUS_H_

#include<hpr/HPR_Mutex.h>
#include"work_thread.h"
#include"definer.h"

class register_persistence_t
    : public work_thread_t
{
private:

    register_persistence_t();
    virtual ~register_persistence_t();

    typedef std::map<std::string, register_status_t> persistence_map_t;
    typedef persistence_map_t::iterator persistence_iter_t;

public:

    inline static register_persistence_t* instance()
    {
        return _instance;
    }

    static HPR_INT32 creat_instance();
    static HPR_VOID del_instance();

    HPR_INT32 init();
    HPR_VOID finit();

    HPR_INT32 persistence_reg_status(register_status_t& reg_info);
    HPR_INT32 persistence_offline_status(const char* phone_num);

private:

    HPR_VOID work(HPR_VOID);

public:

    static register_persistence_t* _instance;

    persistence_map_t _reg_persistence_map;
    HPR_Mutex _reg_map_mutex;



};

#endif
