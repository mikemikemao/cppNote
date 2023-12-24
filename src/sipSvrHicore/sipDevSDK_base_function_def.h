/**
 * \file		sipDevSDK_base_function_def.h
 *
 * \brief		�������ṹ�嶨�塢�궨��
 *
 * \copyright	HangZhou Hikvision System Technology Co.,Ltd. All Right Reserved.
 *
 * \author		
 *
 * \date	2019/06/13	
**/
#ifndef H_EZDEVSDK_BASE_FUNCTION_DEF_H_
#define H_EZDEVSDK_BASE_FUNCTION_DEF_H_
#include <time.h>
#include "sipDevSDK_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

//-------------------------------------������------------------------------------------------------

/**
 * \brief �ӿڴ����룬��������ظ�������
 * \note ���ô����룬��ο�sipDevSDK_error.h
 */
typedef enum {
    SIPDEVSDK_BASE_FUNC_SUCESS                   = 0X00000000, ///< �ɹ�
    SIPDEVSDK_BASE_FUNC_CODE_BASE                = 0XE1000000,   ///< ��������ʼֵ��E��ʾ�ӿڴ����룬1000����ʾ���������λ��ʾ�������

    ///< 0XE1000000 - 0XE100001F ����������
    SIPDEVSDK_BASE_FUNC_CODE_FAIL                = SIPDEVSDK_BASE_FUNC_CODE_BASE + SIPDEVSDK_ERROR_FAIL,
    SIPDEVSDK_BASE_FUNC_CODE_NOT_INITED          = SIPDEVSDK_BASE_FUNC_CODE_BASE + SIPDEVSDK_ERROR_NOT_INITED,
    SIPDEVSDK_BASE_FUNC_CODE_INVALID_PARAM       = SIPDEVSDK_BASE_FUNC_CODE_BASE + SIPDEVSDK_ERROR_INVALID_PARAM,
    SIPDEVSDK_BASE_FUNC_CODE_NOT_SUPPORT         = SIPDEVSDK_BASE_FUNC_CODE_BASE + SIPDEVSDK_ERROR_NOT_SUPPORT,
    SIPDEVSDK_BASE_FUNC_CODE_NOT_ENOUGH_MEMERY   = SIPDEVSDK_BASE_FUNC_CODE_BASE + SIPDEVSDK_ERROR_NOT_ENOUGH_MEMERY,
    SIPDEVSDK_BASE_FUNC_CODE_INIT_FAIL           = SIPDEVSDK_BASE_FUNC_CODE_BASE + SIPDEVSDK_ERROR_INIT_FAIL,
    SIPDEVSDK_BASE_FUNC_CODE_THREAD_CREATE_FAIL  = SIPDEVSDK_BASE_FUNC_CODE_BASE + SIPDEVSDK_ERROR_THREAD_CREATE_FAIL,
    SIPDEVSDK_BASE_FUNC_CODE_RSP_TIMEOUT         = SIPDEVSDK_BASE_FUNC_CODE_BASE + SIPDEVSDK_ERROR_RSP_TIMEOUT,
    SIPDEVSDK_BASE_FUNC_CODE_MSG_DATA_INVAILD    = SIPDEVSDK_BASE_FUNC_CODE_BASE + SIPDEVSDK_ERROR_MSG_DATA_INVAILD,

}SIPDEVSDK_BASE_FUNC_CODE_E;

/*��ҵ���*/
typedef enum _SIP_INDUSTARY_TYPE_E
{
	SIP_BUILDINGS_INDUSTRY = 0,				//¥����ҵ
	SIP_PRISON_INDUSTRY_JP = 1,				//������ҵ(JP)
	SIP_MEDICAL_TREATMENT_INDUSTRY_HI = 2,	//ҽ����ҵ(HI)
	SIP_BROADCASTING_GB = 3,               //�㲥��ҵ(GB)
	SIP_GENERAL_INDUSTRY = 4               //ͨ����ҵ(NM)
}SIP_INDUSTARY_TYPE_E;

//����Ŀǰ�Խ��豸����
typedef enum _SIP_DEV_TYPE_E
{
	SIP_DEV_TYPE_DOOR = 1,				/* �ſڻ� */
	SIP_DEV_TYPE_MANAGER = 2,			/* ����� */
	SIP_DEV_TYPE_ROOM = 3,				/* ���ڻ� */
	SIP_DEV_TYPE_WALL = 4,              /* Χǽ�� */
	SIP_DEV_TYPE_VILLA = 5,				/* �����ſڻ�*/
	SIP_DEV_TYPE_CONFIRM = 6,			/* ����ȷ�ϻ�*/
	SIP_DEV_TYPE_CLIENT_87 = 7,			/* 8700�ͻ��� */
	SIP_DEV_TYPE_CLIENT_42 = 8,			/* 4200�ͻ��� */
	SIP_DEV_TYPE_APP = 9,				/* APP */
	SIP_DEV_TYPE_MEDICAL_MANAGE = 10,	/*��ʿվ����*/
	SIP_DEV_TYPE_TERMINAL = 13,			/*�ն��豸*/	
	SIP_DEV_TYPE_NET_SOUND = 14,		/*���������豸 */	
	SIP_DEV_TYPE_MEDICAL_EXTENSION = 15,/*��ͷ�ֻ�*/
	SIP_DEV_TYPE_BEDSIDE_EXTENSION = 16, /*���߷ֻ�*/	
	SIP_DEV_TYPE_WARD_EXTENSION = 17,	/*̽�ӷֻ�*/
	SIP_DEV_TYPE_ACS = 18,			    /*�Ž��豸*/	

}SIP_DEV_TYPE_E;



//�豸״̬
typedef enum _DEV_STATUS_E
{
    DEV_STATUS_NONE = 0,				/* ����δʹ�� */
	DEV_STATUS_OFFLINE = 1,			    /* ���� */
	DEV_STATUS_ONINE = 2,				/* ���ߣ������У� */
	DEV_STATUS_BUSY = 3,                /* ����ͨ���� */	
}DEV_STATUS_E;

typedef enum _CHANGE_DEVLIST_TYPE_E
{
	SIPDEVSDK_ADD_DEVLIST = 1,      /*�����豸��Ϣ*/
	SIPDEVSDK_MODIFY_DEVLIST = 2,   /*�޸��豸��Ϣ*/
	SIPDEVSDK_DEL_DEVLIST = 3,      /*ɾ���豸��Ϣ*/
	SIPDEVSDK_CLEAR_DEVLIST = 4,    /*����豸��Ϣ*/
}CHANGE_DEVLIST_TYPE_E;

//����ת������
typedef enum _CALL_FORWARDING_TYPE_E
{
    CALL_FORWARDING_TYPE_NONE			= 0,		/* δ���ú���ת�� */
	CALL_FORWARDING_TYPE_UNCONDITIONAL	= 1,		/* ����������ת�� */
	CALL_FORWARDING_TYPE_BUSY			= 2,		/* ��æ����ת�� */
	CALL_FORWARDING_TYPE_UNREACHABLE	= 3,		/* ���ɴ����ת�� */
	CALL_FORWARDING_TYPE_NO_REPLY		= 4,		/* ��Ӧ�����ת�� */
	CALL_FORWARDING_TYPE_INVAILD,
}CALL_FORWARDING_TYPE_E;

#ifndef NAME_LEN 
#define NAME_LEN                        (32)    /* �û������� */
#endif
#ifndef PASSWORD_LEN 
#define PASSWORD_LEN                    (32)    /* SIP���볤�� */
#endif

#ifndef ANDROID_MANAGER_SIPSVR
#define MAX_REGNUM                      (1000)//���ע����Ŀ
#define MAX_ROOM_REGNUM                 (15)//���ڷֻ����ע����Ŀ
#else
#define MAX_REGNUM                      (10000)//��׿��������ע����Ŀ��1��·
#endif
#define SIP_LONG_NUM_LEN				(11)

#define MANAGE_CENTER_CALL_NUM			("ManageCenter")
#define AGAIN_DEV_LONG_NUM				("againdevice")
#define VILLA_SINGLE_NUM				("10010199999")
#define SIP_INDUSTRY_MANAGE				("KM")
#define SIP_INDUSTRY_DOOR				("KD")
#define SIP_INDUSTRY_EXTENSION			("KH")//��Ҫ�޸�

#define SIP_MEDICAL_EXTENSION           ("HI#KD#V10#")//�����ж���̽�ӷֻ�����

#define SIP_MANAGE_CLIENT_MIN_IDX		(1)		/* �ͻ�����С���*/
#define SIP_MANAGE_CLIENT_MAX_IDX		(50)	/* �ͻ��������*/
#define SIP_MANAGE_DEVICE_MIN_IDX		(51)	/* �������С���*/
#define SIP_MANAGE_DEVICE_MAX_IDX		(99)	/* ����������*/

#define SIP_MAX_WAITINGS_NUM            (16)    /* ��������ȴ� */
#define SIP_MIN_CALL_PRIORITY           (1)     /* �������С�������ȼ� */
#define SIP_MAX_CALL_PRIORITY           (100)   /* ��������������ȼ� */

typedef struct _SIP_DEV_SEPARATE_NUM
{
    unsigned char   devType;             	// �豸����(���1λ)      
	unsigned short	zoneNo;					// �ں�(���1λ��)
	unsigned short	buildingNo;				// ¥��(���3λ��)
	unsigned short	unitNo;					// ��Ԫ��(�����λ��)
	unsigned short	floorNo;				// ���(���3λ��)   
	unsigned short	devNo;					// �豸��ţ����ڻ���ʾ�����(���������)���ſڻ��͹������ʾ���
	unsigned short	industryType;			// ��ҵ����      ��VIS_INDUSTARY_TYPE_E
} SIP_DEV_SEPARATE_NUM;

/*
	��ʿվ��������ṹ
*/
typedef struct
{
	unsigned int	hospital;				// Ժ��(���3λ��)
	unsigned int	branch;				    // ��Ժ(���3λ��)
	unsigned int	inpatient;				// ����(���3λ��)
	unsigned int	depart;					// ���ұ��(���3λ��)
	unsigned int	hostNo;				    // ���(���3λ��)
	unsigned char   devType;             	// �豸����(���1λ)  
} HOSPITAL_DEV_SEPARATE_NUM;


//-------------------------------------��ʼ����Ϣ--------------------------------------------------

/**
 *  \brief����
 */
typedef struct {
	unsigned short  _sip_port;				//SIPЭ��˿�
	int  			_reg_Cap;		        //ע������
	bool  			_standard_sip;			//��׼SIP(false,˽�У�true,��׼)
	bool  			_sip_auth;				//��Ȩ(false,��֧�֣�true,֧��)
	bool            _is_group_call;         //�Ƿ�Ⱥ����false-��֧��Ⱥ����֧�����ȼ���true֧��Ⱥ������֧�����ȼ�����ѡһ��
	char            _sip_password[32];      //�������sip����
	unsigned int  	_thread_pool_Cap;		//�̳߳�����
	unsigned int  	_memory_pool_Cap;		//�ڴ������
	unsigned char  	_reg_real_time;		    //ʵʱˢ��ע����Ϣ��0 - ��ʱˢ��ע����Ϣ��1 - ʵʱˢ��ע����Ϣ
	unsigned char  	_res[3];		        //Ԥ��
} sipDevSDK_base_function_config;

/**
 *  \brief ������豸ͨѶ��ͨ����Ϣ
 */
typedef struct {
    int  type;									///< ��Ϣ���ͣ�\ref SIPDEVSDK_BASE_FUNCTION_MSG_TO_DEV_TYPE_E
    void *data;									///< ��Ϣ���ݣ�����type����Ϊ��ͬ��������(data�첽ʹ����Ҫ�ȿ���)
    int  len;									///< ��Ϣ���ݻ�������С
} sipDevSDK_base_function_msg_to_dev;

/**
 *  \brief ��ȡ�豸��Ϣ��ͨ������
 */
typedef struct {
    int	 type;									///< ��Ϣ���ͣ�\ref SIPDEVSDK_BASE_FUNCTION_RUNTIME_INFO_E
    void *data;									///< ��Ϣ���ݣ�����type����Ϊ��ͬ��������
    int  len;									///< ��Ϣ���ݻ�������С
} sipDevSDK_base_function_runtime_info;

/**
 * \brief   �豸�Ļص��������壬���������������Ϣ����Ϣ��ѯ
 * \note    get_runtime_info�ص������У����ڲ�֧�ֵĹ��ܣ�����ֵ����SIPDEVSDK_ERROR_NOT_SUPPORT
 */
typedef struct 
{
    int (*recv_msg)(sipDevSDK_base_function_msg_to_dev *msg);            ///< ��Ϣ�ص��ӿڣ��豸ͨ���˽ӿڽ����������Ϣ
	int (*get_runtime_info)(sipDevSDK_base_function_runtime_info *info); ///< �����ȡ�豸����ʱ��Ϣ�Ļص��ӿ�
	int (*get_reg_random)(const char *phone_num,char *key_random, unsigned long random_len,time_t *gen_time,int create);	///< �����ȡע��ʹ�õ�������Ļص��ӿ�
	int (*get_reg_key_base64)(const char * key_source, const char *key_random, char *key_regist_base64, unsigned long max_out_key_len);///< �����ȡ�豸��Կ2��Э����Կ
	int (*get_reg_password)(const char * in_authName,char *out_password);///< ��ȡע������
	int (*save_reg_key)(const char * in_authName,const char *key_regist_base64);///< ����Э����Կ
	int (*get_visit_local_port)(const char *phone_num, int *port);//̽�ӳ������ݳ��Ż�ȡ
} sipDevSDK_base_function_callback;

/**
* \brief	�����ʼ������
*/
typedef struct 
{
    sipDevSDK_base_function_config config;					        ///< ������Ϣ
    sipDevSDK_base_function_callback cb;							///< �ص�����
} sipDevSDK_base_function_init_info;


//-------------------------------------���֪ͨӦ�ò����Ϣ-----------------------------------------

/**
* \brief ������豸֮��ͨѶ����Ϣ���ͼ���
*/
typedef enum {
	SIPDEVSDK_BASE_FUNC_ON_REGISTER_INFO,	        ///< ͸��ע����Ϣ����Ϣ��: \c sipDevSDK_base_function_register_info
	SIPDEVSDK_BASE_FUNC_ON_UNREGISTER_INFO,	        ///< ͸��ע����Ϣ����Ϣ��: \c sipDevSDK_base_function_register_info
	SIPDEVSDK_BASE_FUNC_ON_REGISTER_UPDATE,			///< ͸��������Ϣ����Ϣ��: \c sipDevSDK_base_function_register_info
	SIPDEVSDK_BASE_FUNC_ON_SIPAUTH_INFO,			///< ͸����֤��Ϣ����Ϣ��: \c osip_proxy_authorization_t(osip�ṹ��)
	SIPDEVSDK_BASE_FUNC_ON_SIPSVR_STATUS,			///< ͸��SIP״̬�� ��Ϣ��: \c sipDevSDK_base_function_sipsvr_status
	SIPDEVSDK_BASE_FUNC_ON_UPLOAD_RECORD,			///< ͸��ͨ����¼����Ϣ��: \c sipDevSDK_base_function_register_info
	SIPDEVSDK_BASE_FUNC_ON_VISIT_AUTH				///< ͸��̽��������Ϣ��: \c sipDevSDK_base_function_visit_info
} SIPDEVSDK_BASE_FUNCTION_MSG_TO_DEV_TYPE_E;




/**
 * \brief   ע�ᣬ��Ӧ��Ϣ����: SIPDEVSDK_BASE_FUNC_ON_REGISTER_INFO
 */
typedef struct {
	char   serial_no[48];		        //���к�
	char   call_num[32];		        //�豸����;     SIP�˺ź���
	char   dev_name[NAME_LEN*2];        //�豸����,���ݸ��ֱ����ʽ; SIP�˺�����
    char   sip_pwd[PASSWORD_LEN];       //SIP�˺�����
	char   dev_ip[16];			        //�豸IP
    char   mask_ip[16];                 //��������
    char   gateway_ip[16];              //Ĭ������
    char   dev_model[NAME_LEN];         //�豸�ͺţ�string
    int    call_priority;               //�������ȼ���1-100������Խ�����ȼ�Խ�ߣ���Թ����
    char   dev_version[NAME_LEN];       //��ǰ����汾��
    int    dev_status;                  //�豸��ǰ״̬��δʹ�á����ߡ����ߡ�ͨ���У�����ֵ��ISAPI����һһ��Ӧ
	char   reg_time[32];		        //���һ��ע��ʱ��
	int    valid_time;			        //ע����Ч�ڣ�����ʱ����
	int    dev_type;			        //�豸����
    int    floorNum;					//����汾ʹ��
	long   gen_time;					//sip2.0���������ʱ��
	char   mac_addr[20];				//mac��ַ
	char   reg_random[64];				//sip2.0�����
	char   reg_key[64];					//sip2.0Э����Կ
	char   call_forwarding_num[36];		//����ת�ƶ������
	int    call_forwarding_type;		//����ת��( �� : CALL_FORWARDING_TYPE_E )
	int    call_forwarding_enable;		//����ת��ʹ��
	char   last_sipserver_ip[16];		//�¼�sip������ע���ַ
	int    last_sipserver_port;         //�¼�sip������ע��˿�
	
} sipDevSDK_base_function_register_info;
typedef sipDevSDK_base_function_register_info REGISTER_INFO;



/**
 * \brief   ̽�ӣ���Ӧ��Ϣ����: SIPDEVSDK_BASE_FUNC_ON_VISIT_AUTH
 */
typedef struct {
	char   from_call_num[36];		        //�����豸����;     SIP�˺ź���
	char   from_dev_ip[16];			        //�����豸IP
    int    from_sdp_port;       			//�����豸SDP audio �˿�
    int    from_local_port;       			//���籾����Ƶ audio �˿�
	char   to_call_num[36];		        	//ȥ���豸����;     SIP�˺ź���
	char   to_dev_ip[16];			        //ȥ���豸IP
    int    to_sdp_port;         			//ȥ���豸SDP audio �˿�
    int    to_local_port;       			//ȥ�籾����Ƶ audio �˿�
    int    dev_status;                      //�豸��ǰ״̬��δ��Ȩ�����塢�������Ҷ�
} sipDevSDK_base_function_visit_info;
typedef sipDevSDK_base_function_visit_info VISIT_INFO;



/**
 * \brief   SIP״̬
 */
typedef struct {
    bool busy;				///< SIP״̬,tureæµ��false����
} sipDevSDK_base_function_sipsvr_status;

//-------------------------------------�����Ӧ�ò��ȡ����Ϣ---------------------------------------

/**
 * \brief   ����Ӵ��豸��ȡ��Ϣ
 */
typedef enum {
    //������豸����ȡ
    SIPDEVSDK_BASE_FUNC_RT_INFO_GET_DEV_LONGNUM,          		///< ���豸��ȡ�豸���ţ���Ϣ��: sipDevSDK_base_function_dev_longNum
    SIPDEVSDK_BASE_FUNC_RT_INFO_GET_DEV_TYPE,          		    ///< ���豸��ȡ�豸���ͣ���Ϣ��: sipDevSDK_base_function_dev_type
	SIPDEVSDK_BASE_FUNC_RT_INFO_GET_LOCAL_IP,          		    ///< ���豸��ȡ����IP����Ϣ��: \c sipDevSDK_base_function_local_ip
	SIPDEVSDK_BASE_FUNC_RT_INFO_GET_MANGER_CENTER,          	///< ���豸����������Ϣ����Ϣ��: \c sipDevSDK_base_function_manager_info
    SIPDEVSDK_BASE_FUNC_RT_INFO_GET_REGISTER_LIST,         		///< ���豸��ȡ����ע���б���Ϣ��: \c ezDevSDK_base_function_register_list
    SIPDEVSDK_BASE_FUNC_RT_INFO_GET_DEV_OVERSEA,         		///< ���豸��ȡ�Ƿ���汾����Ϣ��: \c sipDevSDK_base_function_dev_oversea
    SIPDEVSDK_BASE_FUNC_RT_INFO_GET_TIME_FOR_REPLY,         	///< ��ȡ����ʱ������ע��ظ�����,GMT��ʽ����Ϣ��: \c sipDevSDK_base_function_reply_time
    SIPDEVSDK_BASE_FUNC_RT_INFO_GET_SIP_STATUS,         		///< ���豸��ȡSIP�ͻ���״̬: \c sipDevSDK_base_function_sip_status
 	SIPDEVSDK_BASE_FUNC_RT_INFO_GET_DEV_INDUS_LONGNUM,          ///< ���豸��ȡ�豸��ҵ�³��ţ���Ϣ��: sipDevSDK_base_function_dev_longNum
 	SIPDEVSDK_BASE_FUNC_RT_INFO_GET_TRUSTEESHIP_INFO,          ///< ���豸��ȡ�й���Ϣ����Ϣ��: sipDevSDK_base_function_trusteeship_info
 	SIPDEVSDK_BASE_FUNC_RT_INFO_GET_ADVANCED_SIPADDR,          ///< ���豸��ȡ�߼�sip�����ַ����Ϣ��: sipDevSDK_base_function_advSipServer_ip
 	SIPDEVSDK_BASE_FUNC_RT_INFO_GET_ISNEED_TRANS,              ///< ���豸��ȡ��ǰmessage�Ƿ���Ҫת������Ϣ��: sipDevSDK_base_function_advSipServer_ip
}SIPDEVSDK_BASE_FUNCTION_RUNTIME_INFO_E;


/**
 * \brief   ̽�ӻص��ӿڷ���״̬
 */
typedef enum {
    SIPDEVSDK_RET_ERR = -1, 			///�쳣����
    SIPDEVSDK_RET_OK = 1,          		///��������
    SIPDEVSDK_RET_AUTH_SUSS,          	///��Ȩ�ɹ�
    SIPDEVSDK_RET_AUTH_REJECT,          ///��Ȩ�ܾ�
    SIPDEVSDK_RET_DUPLICATE_VISIT,      ///�ظ���̽������
}SIPDEVSDK_VISIT_RET_INFO;

/*̽���¼� */
typedef enum
{
	VISIT_EVENT_TYPE_INVLID = -1,			/* ��Ч���� */
	VISIT_EVENT_TYPE_REQUEST = 1,			    /* �յ�̽������ */	
	VISIT_EVENT_TYPE_ANSWER,			    /* ̽���������� */	
	VISIT_EVENT_TYPE_RING,			        /* ̽��˫��*/	
	VISIT_EVENT_TYPE_CANCEL,			    /* ̽��ͨ���Ҷϻ�ȡ��*/
	VISIT_EVENT_TYPE_FAILED,			    /* ̽��ʧ��*/	
} VISIT_EVENT_TYPE;


/**
 * \brief   ���豸��ȡSIP�ͻ���״̬
 */
typedef struct {
    bool sip_busy;				///< ���豸��ȡSIP�ͻ���״̬,ture:æµ��false����
} sipDevSDK_base_function_sip_status;

/**
 * \brief   ��ȡ�����豸����
 */
typedef struct {
    char data[32];				///< ��ȡ����ʱ������ע��ظ�����,GMT��ʽ
} sipDevSDK_base_function_reply_time;


/**
 * \brief   ��ȡ�����豸����
 */
typedef struct {
    char data[64];				///< ��ȡ�����豸����
} sipDevSDK_base_function_dev_longNum;

/**
 * \brief   ��ȡ�����豸����
 */
typedef struct {
    int dev_type;				///< ��ȡ�����豸���� SIP_DEV_TYPE_E
} sipDevSDK_base_function_dev_type;

/**
 * \brief   ��ȡ�����豸�汾
 */
typedef struct {
    bool type;				///< ��ȡ�����豸����汾,trueΪ����汾
} sipDevSDK_base_function_dev_oversea;


/**
 * \brief   ��ȡ����IP
 */
typedef struct {
    char data[16];				///< ��ȡ����IP
} sipDevSDK_base_function_local_ip;


typedef struct {
  int num;
  #ifdef ANDROID_MANAGER_SIPSVR 
  REGISTER_INFO reg[MAX_REGNUM];///< �ſڻ�/���ڻ�����ע���б���׿��������֧��1��·
  #else
   REGISTER_INFO reg[MAX_ROOM_REGNUM];///< ���ڻ�����ע���б�(�������ֻ�5̨���������ֻ�15̨)
  #endif
} sipDevSDK_base_function_register_list;

/**
 * \brief   �������Ϣ����Ӧ��Ϣ����: SIPDEVSDK_BASE_FUNC_RT_INFO_GET_MANGER_CENTER
 */
typedef struct {
	int    dev_type;		//�豸����
	char   dev_name[32];	//�豸����
	char   call_num[20];	//�豸����
	char   dev_ip[16];		//�豸IP
	char   reg_time[32];	//ע��ʱЧ
	char   mac_addr[20];	//mac��ַ
	char   serial_no[48];	//���к�
} sipDevSDK_base_function_manager_info;

/**
 * \brief   �й���Ϣ����Ӧ��Ϣ����: SIPDEVSDK_BASE_FUNC_RT_INFO_GET_TRUSTEESHIP_INFO
 */
typedef struct {
	bool   trusteeshipEnableFlag;	//�й�ʹ�ܱ�־
	char	trusteeshipNum[32];		   /* �йܳ���*/
} sipDevSDK_base_function_trusteeship_info;

/**
 * \brief   �߼�sip�����ַ����Ӧ��Ϣ����: SIPDEVSDK_BASE_FUNC_RT_INFO_GET_ADVANCED_SIPADDR
 */
typedef struct {
	char   advSipServerIpAddr[16];	//�߼�sip�����ַ
	int    advSipServer_port;       //�߼�sip����˿�
} sipDevSDK_base_function_advSipServer_ip;

/**
 * \brief   �Ƿ񣬶�Ӧ��Ϣ����: SIPDEVSDK_BASE_FUNC_RT_INFO_GET_ISNEED_TRANS
 */
typedef struct {
	char    uri[64];		   /* message uri*/
	char    fromNum[32];	   /* message from num*/
	bool    needFlag;	      //�Ƿ�ת����־
} sipDevSDK_base_function_message_check_info;

#ifdef __cplusplus
}
#endif

#endif //H_SIPDEVSDK_BASE_FUNCTION_DEF_H_
