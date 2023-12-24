/**
 * \file		sipDevSDK_base_function.h
 *
 * \brief		sipsvr组件基本接口
 *
 * \copyright	HangZhou Hikvision System Technology Co.,Ltd. All Right Reserved.
 *
 * \author		YCS
 *
 * \date		2019/5/10
 */

#ifndef sipDevSDK_base_function_H
#define sipDevSDK_base_function_H

#include "sipDevSDK_error.h"
#include "sipDevSDK_domain_def.h"
#include "sipDevSDK_base_function_def.h"


#ifdef __cplusplus
	extern "C" {
#endif


/*************************************************************************
函数名称：sipDevSDK_Init
函数表述：sip组件初始化,注册回调及参数配置
      IN：sipDevSDK_base_function_init_info *cb 基础事件回调
 
	 OUT：无
  return：0 success,-1 failed  
注意事项：
*************************************************************************/
int sipDevSDK_Init(sipDevSDK_base_function_init_info *init);


/*************************************************************************
函数名称：sipDevSDK_Start
函数表述：启动sip组件
      IN：无	
      			
	 OUT：无
  return：0 success,-1 failed  
注意事项：sipDevSDK_Init之后
*************************************************************************/
int sipDevSDK_Start();


/*************************************************************************
函数名称：sipDevSDK_Stop
函数表述：停止sip组件
      IN：无	
      			
	 OUT：无
  return：0 success,-1 failed  
注意事项：
*************************************************************************/
int sipDevSDK_Stop();



/*************************************************************************
函数名称：sipDevSDK_Fini
函数表述：sip组件反初始化
      IN：无	
      			
	 OUT：无
  return：0 success,-1 failed  
注意事项：sipDevSDK_Stop之后
*************************************************************************/
int sipDevSDK_Fini();

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
int sipDevSDK_set_log_level(int level);


/*************************************************************************
函数名称：sipDevSDK_show
函数表述：打印注册信息
      IN：无	
      			
	 OUT：无
     return：0 success,-1 failed  
注意事项：
*************************************************************************/
int sipDevSDK_show();

/*************************************************************************
函数名称：sipDevSDK_config_group_call
函数表述：群呼配置
      IN：无	
      			
	 OUT：无
     return：0 success,-1 failed  
注意事项：
*************************************************************************/
int sipDevSDK_config_group_call(bool status);

/*************************************************************************
函数名称：sipDevSDK_modify_devlist
函数表述：修改设备信息
      IN：无	
      			
	 OUT：无
     return：0 success,-1 failed  
注意事项：
*************************************************************************/
int sipDevSDK_change_devlist(REGISTER_INFO *regInfo, int type);


#ifdef __cplusplus
}
#endif

#endif /*end of sipDevSDK_base_function_H*/
