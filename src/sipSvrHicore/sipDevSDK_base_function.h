/**
 * \file		sipDevSDK_base_function.h
 *
 * \brief		sipsvr��������ӿ�
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
�������ƣ�sipDevSDK_Init
����������sip�����ʼ��,ע��ص�����������
      IN��sipDevSDK_base_function_init_info *cb �����¼��ص�
 
	 OUT����
  return��0 success,-1 failed  
ע�����
*************************************************************************/
int sipDevSDK_Init(sipDevSDK_base_function_init_info *init);


/*************************************************************************
�������ƣ�sipDevSDK_Start
��������������sip���
      IN����	
      			
	 OUT����
  return��0 success,-1 failed  
ע�����sipDevSDK_Init֮��
*************************************************************************/
int sipDevSDK_Start();


/*************************************************************************
�������ƣ�sipDevSDK_Stop
����������ֹͣsip���
      IN����	
      			
	 OUT����
  return��0 success,-1 failed  
ע�����
*************************************************************************/
int sipDevSDK_Stop();



/*************************************************************************
�������ƣ�sipDevSDK_Fini
����������sip�������ʼ��
      IN����	
      			
	 OUT����
  return��0 success,-1 failed  
ע�����sipDevSDK_Stop֮��
*************************************************************************/
int sipDevSDK_Fini();

/*************************************************************************
�������ƣ�sipDevSDK_set_log_level
����������������־��ӡ�ȼ�
	IN��
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
					
	OUT����
	return��0 success,-1 failed  
ע�����
*************************************************************************/
int sipDevSDK_set_log_level(int level);


/*************************************************************************
�������ƣ�sipDevSDK_show
������������ӡע����Ϣ
      IN����	
      			
	 OUT����
     return��0 success,-1 failed  
ע�����
*************************************************************************/
int sipDevSDK_show();

/*************************************************************************
�������ƣ�sipDevSDK_config_group_call
����������Ⱥ������
      IN����	
      			
	 OUT����
     return��0 success,-1 failed  
ע�����
*************************************************************************/
int sipDevSDK_config_group_call(bool status);

/*************************************************************************
�������ƣ�sipDevSDK_modify_devlist
�����������޸��豸��Ϣ
      IN����	
      			
	 OUT����
     return��0 success,-1 failed  
ע�����
*************************************************************************/
int sipDevSDK_change_devlist(REGISTER_INFO *regInfo, int type);


#ifdef __cplusplus
}
#endif

#endif /*end of sipDevSDK_base_function_H*/
