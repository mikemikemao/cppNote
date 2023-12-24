/*********************************************************************************
 * Version: 	libsipsvr.a       
 * ģ����       : sipsvr_api    
 * �ļ���       : sipsvr_api.cpp      
 * ����ļ�     :sipsvr_api.h / sipsvr_api.cpp  
 *     
 *     
 * �ļ�ʵ�ֹ��� : ��������װ�ӿ�      
 * ����:         syc  
 * ����ʱ��: 2019-7-13
 * �޸ļ�¼:        
 *     <����>   <�汾>    <�޸��� >   <�޸�����>
 * YYYY/MM/DD   X.Y      <���߻��޸�����>    <�޸�����>    
 * ����˵��    
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
�������ƣ�sipDevSDK_Init
����������sip�����ʼ��,ע��ص�����������
      IN��		sip_basic_notify *cb		�����¼��ص�
	 OUT����
  return��0 success,-1 failed  
ע�����
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
�������ƣ�sipDevSDK_Start
��������������sip���
      IN����	
      			
	 OUT����
     return��0 success,-1 failed  
ע�����sip_init֮��
*************************************************************************/
int sipDevSDK_Start()
{

    sip_application::instance()->start_app();

    return 0;
}



/*************************************************************************
�������ƣ�sipDevSDK_Stop
����������ֹͣsip���
	IN����	
					
	OUT����
	return��0 success,-1 failed  
ע�����
*************************************************************************/
int sipDevSDK_Stop()
{

    sip_application::instance()->stop_app();

    return 0;
}


/*************************************************************************
�������ƣ�sipDevSDK_Fini
����������sip�������ʼ��
	IN����	
					
	OUT����
	return��0 success,-1 failed  
ע�����sip_stop֮��
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
int sipDevSDK_set_log_level(int level)
{
	hlog_set_dedug_level(level);

#ifdef _CRT_DEBUG
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}


/*************************************************************************
�������ƣ�sipDevSDK_show
������������ӡע����Ϣ
      IN����	
      			
	 OUT����
     return��0 success,-1 failed  
ע�����
*************************************************************************/
int sipDevSDK_show()
{

    sip_application::instance()->show_addr_book_mgr();

    return 0;
}

/*************************************************************************
�������ƣ�sipDevSDK_config_group_call
����������Ⱥ������
      IN����	
      			
	 OUT����
     return��0 success,-1 failed  
ע�����
*************************************************************************/
int sipDevSDK_config_group_call(bool status)
{
	sip_application::instance()->set_group_call_config(status);
	return 0;
}


/*************************************************************************
�������ƣ�sipDevSDK_modify_devlist
�����������޸��豸��Ϣ
      IN����	
      			
	 OUT����
     return��0 success,-1 failed  
ע�����
*************************************************************************/
int sipDevSDK_change_devlist(REGISTER_INFO *regInfo, int type)
{
	return sip_application::instance()->change_devlist(regInfo, type);
}


