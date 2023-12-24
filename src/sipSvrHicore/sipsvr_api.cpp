/*********************************************************************************
 * Version: 	libsipsvr.a       
 * 模块名       : sipsvr_api    
 * 文件名       : sipsvr_api.cpp      
 * 相关文件     :sipsvr_api.h / sipsvr_api.cpp  
 *     
 *     
 * 文件实现功能 : 组件对外封装接口      
 * 作者:         syc  
 * 创建时间: 2019-7-13
 * 修改记录:        
 *     <日期>   <版本>    <修改人 >   <修改内容>
 * YYYY/MM/DD   X.Y      <作者或修改者名>    <修改内容>    
 * 其它说明    
 *     
 *******************************************************************************/
#include"sipDevSDK_base_function.h"
#include"application.h"

#ifdef _CRT_DEBUG
#define _CRTDBG_MAP_ALLOC
#include<stdlib.h>
#include<crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif





/*************************************************************************
函数名称：sipDevSDK_Init
函数表述：sip组件初始化,注册回调及参数配置
      IN：		sip_basic_notify *cb		基础事件回调
	 OUT：无
  return：0 success,-1 failed  
注意事项：
*************************************************************************/
int sipDevSDK_Init(sipDevSDK_base_function_init_info  *init)
{
#ifdef _CRT_DEBUG
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
    if (sip_application::create_instance() != 0)
    {
        SIP_SVR_ERROR("sip devsdk init faild!");
        return -1;
    }

    return sip_application::instance()->init(init);
}


	

/*************************************************************************
函数名称：sipDevSDK_Start
函数表述：启动sip组件
      IN：无	
      			
	 OUT：无
     return：0 success,-1 failed  
注意事项：sip_init之后
*************************************************************************/
int sipDevSDK_Start()
{

    sip_application::instance()->start_app();

    return 0;
}



/*************************************************************************
函数名称：sipDevSDK_Stop
函数表述：停止sip组件
	IN：无	
					
	OUT：无
	return：0 success,-1 failed  
注意事项：
*************************************************************************/
int sipDevSDK_Stop()
{

    sip_application::instance()->stop_app();

    return 0;
}


/*************************************************************************
函数名称：sipDevSDK_Fini
函数表述：sip组件反初始化
	IN：无	
					
	OUT：无
	return：0 success,-1 failed  
注意事项：sip_stop之后
*************************************************************************/
int sipDevSDK_Fini()
{
    sip_application::instance()->finit();
    sip_application::del_instance();

#ifdef _CRT_DEBUG
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}

/*************************************************************************
函数名称：sipDevSDK_set_log_level
函数表述：设置日志打印等级
	IN：
	typedef enum {
    HLOG_LEVEL_OFF    = 7,
    HLOG_LEVEL_FATAL  = 6,
    HLOG_LEVEL_ERROR  = 5,
    HLOG_LEVEL_WARN   = 4,
    HLOG_LEVEL_INFO   = 3,
    HLOG_LEVEL_DEBUG  = 2,
    HLOG_LEVEL_TRACE  = 1,
    HLOG_LEVEL_ALL    = 0
	} HLOG_LEVEL;	
					
	OUT：无
	return：0 success,-1 failed  
注意事项：
*************************************************************************/
int sipDevSDK_set_log_level(int level)
{
	hlog_set_dedug_level(level);

#ifdef _CRT_DEBUG
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}


/*************************************************************************
函数名称：sipDevSDK_show
函数表述：打印注册信息
      IN：无	
      			
	 OUT：无
     return：0 success,-1 failed  
注意事项：
*************************************************************************/
int sipDevSDK_show()
{

    sip_application::instance()->show_addr_book_mgr();

    return 0;
}

/*************************************************************************
函数名称：sipDevSDK_config_group_call
函数表述：群呼配置
      IN：无	
      			
	 OUT：无
     return：0 success,-1 failed  
注意事项：
*************************************************************************/
int sipDevSDK_config_group_call(bool status)
{
	sip_application::instance()->set_group_call_config(status);
	return 0;
}


/*************************************************************************
函数名称：sipDevSDK_modify_devlist
函数表述：修改设备信息
      IN：无	
      			
	 OUT：无
     return：0 success,-1 failed  
注意事项：
*************************************************************************/
int sipDevSDK_change_devlist(REGISTER_INFO *regInfo, int type)
{
	return sip_application::instance()->change_devlist(regInfo, type);
}


