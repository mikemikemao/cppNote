/** @file:   plat_sdk_manager.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  平台相关接口管理
 *
 *  @author: wangfeida@hikvision.com.cn
 *  @data:   2014/08/04
 *  
 *  @note:   
 *
 *  @Modification History:
 *  <version>       <time>          <author>        <desc>
 *    V1.0        2014/08/04        wangfeida       Create
 *
 *  @warning: <No waining>        
*/

#ifndef _PLAT_SDK_MANAGER_H_
#define _PLAT_SDK_MANAGER_H_

#include"definer.h"
#include"plat_sdk_def.h"

class plat_sdk_t
{
private:

    plat_sdk_t();
    virtual ~plat_sdk_t();

public:

    static HPR_INT32 create_instance();
    static HPR_VOID del_instance();

    inline static plat_sdk_t* instance()
    {
        return _instance;
    }

    HPR_INT32 init();
    HPR_VOID finit();

    HPR_INT32 upload_record(std::list<call_record_t>& call_info);

private:

    static plat_sdk_t* _instance;

    static const std::string null_interface;

    //sdk句柄
    HPR_HANDLE _sdk_handle;

    //sdk接口
    sip_sdk_init _init_interface;
    sip_sdk_finit _finit_interface;
    sip_sdk_upload_record _upload_record_interface;
    sip_sdk_get_last_error _get_last_error_interface;
    sip_sdk_version _get_version_interface;
	sip_sdk_free_temporary_data _free_temporary_data;

};

#endif
