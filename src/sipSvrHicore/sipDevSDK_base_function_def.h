/**
 * \file		sipDevSDK_base_function_def.h
 *
 * \brief		组件对外结构体定义、宏定义
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

//-------------------------------------错误码------------------------------------------------------

/**
 * \brief 接口错误码，由组件返回给调用者
 * \note 公用错误码，请参考sipDevSDK_error.h
 */
typedef enum {
    SIPDEVSDK_BASE_FUNC_SUCESS                   = 0X00000000, ///< 成功
    SIPDEVSDK_BASE_FUNC_CODE_BASE                = 0XE1000000,   ///< 错误码起始值，E表示接口错误码，1000，表示领域，最后三位表示具体错误

    ///< 0XE1000000 - 0XE100001F 公共错误码
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

/*行业类别*/
typedef enum _SIP_INDUSTARY_TYPE_E
{
	SIP_BUILDINGS_INDUSTRY = 0,				//楼宇行业
	SIP_PRISON_INDUSTRY_JP = 1,				//监所行业(JP)
	SIP_MEDICAL_TREATMENT_INDUSTRY_HI = 2,	//医疗行业(HI)
	SIP_BROADCASTING_GB = 3,               //广播行业(GB)
	SIP_GENERAL_INDUSTRY = 4               //通用行业(NM)
}SIP_INDUSTARY_TYPE_E;

//兼容目前对讲设备类型
typedef enum _SIP_DEV_TYPE_E
{
	SIP_DEV_TYPE_DOOR = 1,				/* 门口机 */
	SIP_DEV_TYPE_MANAGER = 2,			/* 管理机 */
	SIP_DEV_TYPE_ROOM = 3,				/* 室内机 */
	SIP_DEV_TYPE_WALL = 4,              /* 围墙机 */
	SIP_DEV_TYPE_VILLA = 5,				/* 别墅门口机*/
	SIP_DEV_TYPE_CONFIRM = 6,			/* 二次确认机*/
	SIP_DEV_TYPE_CLIENT_87 = 7,			/* 8700客户端 */
	SIP_DEV_TYPE_CLIENT_42 = 8,			/* 4200客户端 */
	SIP_DEV_TYPE_APP = 9,				/* APP */
	SIP_DEV_TYPE_MEDICAL_MANAGE = 10,	/*护士站主机*/
	SIP_DEV_TYPE_TERMINAL = 13,			/*终端设备*/	
	SIP_DEV_TYPE_NET_SOUND = 14,		/*网络音响设备 */	
	SIP_DEV_TYPE_MEDICAL_EXTENSION = 15,/*床头分机*/
	SIP_DEV_TYPE_BEDSIDE_EXTENSION = 16, /*床边分机*/	
	SIP_DEV_TYPE_WARD_EXTENSION = 17,	/*探视分机*/
	SIP_DEV_TYPE_ACS = 18,			    /*门禁设备*/	

}SIP_DEV_TYPE_E;



//设备状态
typedef enum _DEV_STATUS_E
{
    DEV_STATUS_NONE = 0,				/* 从来未使用 */
	DEV_STATUS_OFFLINE = 1,			    /* 离线 */
	DEV_STATUS_ONINE = 2,				/* 在线（空闲中） */
	DEV_STATUS_BUSY = 3,                /* 正在通话中 */	
}DEV_STATUS_E;

typedef enum _CHANGE_DEVLIST_TYPE_E
{
	SIPDEVSDK_ADD_DEVLIST = 1,      /*新增设备信息*/
	SIPDEVSDK_MODIFY_DEVLIST = 2,   /*修改设备信息*/
	SIPDEVSDK_DEL_DEVLIST = 3,      /*删除设备信息*/
	SIPDEVSDK_CLEAR_DEVLIST = 4,    /*清空设备信息*/
}CHANGE_DEVLIST_TYPE_E;

//呼叫转移类型
typedef enum _CALL_FORWARDING_TYPE_E
{
    CALL_FORWARDING_TYPE_NONE			= 0,		/* 未配置呼叫转移 */
	CALL_FORWARDING_TYPE_UNCONDITIONAL	= 1,		/* 无条件呼叫转移 */
	CALL_FORWARDING_TYPE_BUSY			= 2,		/* 遇忙呼叫转移 */
	CALL_FORWARDING_TYPE_UNREACHABLE	= 3,		/* 不可达呼叫转移 */
	CALL_FORWARDING_TYPE_NO_REPLY		= 4,		/* 无应答呼叫转移 */
	CALL_FORWARDING_TYPE_INVAILD,
}CALL_FORWARDING_TYPE_E;

#ifndef NAME_LEN 
#define NAME_LEN                        (32)    /* 用户名长度 */
#endif
#ifndef PASSWORD_LEN 
#define PASSWORD_LEN                    (32)    /* SIP密码长度 */
#endif

#ifndef ANDROID_MANAGER_SIPSVR
#define MAX_REGNUM                      (1000)//最大注册数目
#define MAX_ROOM_REGNUM                 (15)//室内分机最大注册数目
#else
#define MAX_REGNUM                      (10000)//安卓管理机最大注册数目是1万路
#endif
#define SIP_LONG_NUM_LEN				(11)

#define MANAGE_CENTER_CALL_NUM			("ManageCenter")
#define AGAIN_DEV_LONG_NUM				("againdevice")
#define VILLA_SINGLE_NUM				("10010199999")
#define SIP_INDUSTRY_MANAGE				("KM")
#define SIP_INDUSTRY_DOOR				("KD")
#define SIP_INDUSTRY_EXTENSION			("KH")//需要修改

#define SIP_MEDICAL_EXTENSION           ("HI#KD#V10#")//用于判断是探视分机类型

#define SIP_MANAGE_CLIENT_MIN_IDX		(1)		/* 客户端最小编号*/
#define SIP_MANAGE_CLIENT_MAX_IDX		(50)	/* 客户端最大编号*/
#define SIP_MANAGE_DEVICE_MIN_IDX		(51)	/* 管理机最小编号*/
#define SIP_MANAGE_DEVICE_MAX_IDX		(99)	/* 管理机最大编号*/

#define SIP_MAX_WAITINGS_NUM            (16)    /* 管理机最大等待 */
#define SIP_MIN_CALL_PRIORITY           (1)     /* 管理机最小呼叫优先级 */
#define SIP_MAX_CALL_PRIORITY           (100)   /* 管理机最大呼叫优先级 */

typedef struct _SIP_DEV_SEPARATE_NUM
{
    unsigned char   devType;             	// 设备类型(最多1位)      
	unsigned short	zoneNo;					// 期号(最多1位数)
	unsigned short	buildingNo;				// 楼号(最多3位数)
	unsigned short	unitNo;					// 单元号(最多两位数)
	unsigned short	floorNo;				// 层号(最多3位数)   
	unsigned short	devNo;					// 设备编号，室内机表示房间号(不包括层号)，门口机和管理机表示编号
	unsigned short	industryType;			// 行业类型      见VIS_INDUSTARY_TYPE_E
} SIP_DEV_SEPARATE_NUM;

/*
	护士站主机号码结构
*/
typedef struct
{
	unsigned int	hospital;				// 院区(最多3位数)
	unsigned int	branch;				    // 分院(最多3位数)
	unsigned int	inpatient;				// 病区(最多3位数)
	unsigned int	depart;					// 科室编号(最多3位数)
	unsigned int	hostNo;				    // 编号(最多3位数)
	unsigned char   devType;             	// 设备类型(最多1位)  
} HOSPITAL_DEV_SEPARATE_NUM;


//-------------------------------------初始化信息--------------------------------------------------

/**
 *  \brief参数
 */
typedef struct {
	unsigned short  _sip_port;				//SIP协议端口
	int  			_reg_Cap;		        //注册容量
	bool  			_standard_sip;			//标准SIP(false,私有；true,标准)
	bool  			_sip_auth;				//鉴权(false,不支持；true,支持)
	bool            _is_group_call;         //是否群呼（false-不支持群呼，支持优先级，true支持群呼，不支持优先级；二选一）
	char            _sip_password[32];      //主管理机sip密码
	unsigned int  	_thread_pool_Cap;		//线程池容量
	unsigned int  	_memory_pool_Cap;		//内存池容量
	unsigned char  	_reg_real_time;		    //实时刷新注册信息：0 - 定时刷新注册信息；1 - 实时刷新注册信息
	unsigned char  	_res[3];		        //预留
} sipDevSDK_base_function_config;

/**
 *  \brief 组件和设备通讯的通用消息
 */
typedef struct {
    int  type;									///< 消息类型，\ref SIPDEVSDK_BASE_FUNCTION_MSG_TO_DEV_TYPE_E
    void *data;									///< 消息内容，根据type解释为不同具体类型(data异步使用需要先拷贝)
    int  len;									///< 消息内容缓冲区大小
} sipDevSDK_base_function_msg_to_dev;

/**
 *  \brief 获取设备信息的通用载体
 */
typedef struct {
    int	 type;									///< 信息类型，\ref SIPDEVSDK_BASE_FUNCTION_RUNTIME_INFO_E
    void *data;									///< 信息内容，根据type解释为不同具体类型
    int  len;									///< 信息内容缓冲区大小
} sipDevSDK_base_function_runtime_info;

/**
 * \brief   设备的回调函数定义，用来接收组件的消息和信息查询
 * \note    get_runtime_info回调函数中，对于不支持的功能，返回值请填SIPDEVSDK_ERROR_NOT_SUPPORT
 */
typedef struct 
{
    int (*recv_msg)(sipDevSDK_base_function_msg_to_dev *msg);            ///< 消息回调接口，设备通过此接口接收组件的消息
	int (*get_runtime_info)(sipDevSDK_base_function_runtime_info *info); ///< 组件获取设备运行时信息的回调接口
	int (*get_reg_random)(const char *phone_num,char *key_random, unsigned long random_len,time_t *gen_time,int create);	///< 组件获取注册使用的随机数的回调接口
	int (*get_reg_key_base64)(const char * key_source, const char *key_random, char *key_regist_base64, unsigned long max_out_key_len);///< 组件获取设备密钥2与协商密钥
	int (*get_reg_password)(const char * in_authName,char *out_password);///< 获取注册密码
	int (*save_reg_key)(const char * in_authName,const char *key_regist_base64);///< 保存协商秘钥
	int (*get_visit_local_port)(const char *phone_num, int *port);//探视场景根据长号获取
} sipDevSDK_base_function_callback;

/**
* \brief	组件初始化参数
*/
typedef struct 
{
    sipDevSDK_base_function_config config;					        ///< 参数信息
    sipDevSDK_base_function_callback cb;							///< 回调函数
} sipDevSDK_base_function_init_info;


//-------------------------------------组件通知应用层的消息-----------------------------------------

/**
* \brief 组件和设备之间通讯的消息类型集合
*/
typedef enum {
	SIPDEVSDK_BASE_FUNC_ON_REGISTER_INFO,	        ///< 透传注册信息，消息体: \c sipDevSDK_base_function_register_info
	SIPDEVSDK_BASE_FUNC_ON_UNREGISTER_INFO,	        ///< 透传注销信息，消息体: \c sipDevSDK_base_function_register_info
	SIPDEVSDK_BASE_FUNC_ON_REGISTER_UPDATE,			///< 透传更新信息，消息体: \c sipDevSDK_base_function_register_info
	SIPDEVSDK_BASE_FUNC_ON_SIPAUTH_INFO,			///< 透传认证信息，消息体: \c osip_proxy_authorization_t(osip结构体)
	SIPDEVSDK_BASE_FUNC_ON_SIPSVR_STATUS,			///< 透传SIP状态， 消息体: \c sipDevSDK_base_function_sipsvr_status
	SIPDEVSDK_BASE_FUNC_ON_UPLOAD_RECORD,			///< 透传通话记录，消息体: \c sipDevSDK_base_function_register_info
	SIPDEVSDK_BASE_FUNC_ON_VISIT_AUTH				///< 透传探视请求，消息体: \c sipDevSDK_base_function_visit_info
} SIPDEVSDK_BASE_FUNCTION_MSG_TO_DEV_TYPE_E;




/**
 * \brief   注册，对应消息类型: SIPDEVSDK_BASE_FUNC_ON_REGISTER_INFO
 */
typedef struct {
	char   serial_no[48];		        //序列号
	char   call_num[32];		        //设备长号;     SIP账号号码
	char   dev_name[NAME_LEN*2];        //设备名称,兼容各种编码格式; SIP账号名称
    char   sip_pwd[PASSWORD_LEN];       //SIP账号密码
	char   dev_ip[16];			        //设备IP
    char   mask_ip[16];                 //子网掩码
    char   gateway_ip[16];              //默认网关
    char   dev_model[NAME_LEN];         //设备型号，string
    int    call_priority;               //呼叫优先级，1-100：数字越大优先级越高，针对管理机
    char   dev_version[NAME_LEN];       //当前软件版本号
    int    dev_status;                  //设备当前状态；未使用、在线、离线、通话中；具体值与ISAPI保持一一对应
	char   reg_time[32];		        //最近一次注册时间
	int    valid_time;			        //注册有效期（过期时长）
	int    dev_type;			        //设备类型
    int    floorNum;					//海外版本使用
	long   gen_time;					//sip2.0随机数更新时间
	char   mac_addr[20];				//mac地址
	char   reg_random[64];				//sip2.0随机数
	char   reg_key[64];					//sip2.0协商秘钥
	char   call_forwarding_num[36];		//呼叫转移对象号码
	int    call_forwarding_type;		//呼叫转移( 见 : CALL_FORWARDING_TYPE_E )
	int    call_forwarding_enable;		//呼叫转移使能
	char   last_sipserver_ip[16];		//下级sip服务器注册地址
	int    last_sipserver_port;         //下级sip服务器注册端口
	
} sipDevSDK_base_function_register_info;
typedef sipDevSDK_base_function_register_info REGISTER_INFO;



/**
 * \brief   探视，对应消息类型: SIPDEVSDK_BASE_FUNC_ON_VISIT_AUTH
 */
typedef struct {
	char   from_call_num[36];		        //来电设备长号;     SIP账号号码
	char   from_dev_ip[16];			        //来电设备IP
    int    from_sdp_port;       			//来电设备SDP audio 端口
    int    from_local_port;       			//来电本地音频 audio 端口
	char   to_call_num[36];		        	//去电设备长号;     SIP账号号码
	char   to_dev_ip[16];			        //去电设备IP
    int    to_sdp_port;         			//去电设备SDP audio 端口
    int    to_local_port;       			//去电本地音频 audio 端口
    int    dev_status;                      //设备当前状态；未鉴权、回铃、接听、挂断
} sipDevSDK_base_function_visit_info;
typedef sipDevSDK_base_function_visit_info VISIT_INFO;



/**
 * \brief   SIP状态
 */
typedef struct {
    bool busy;				///< SIP状态,ture忙碌，false空闲
} sipDevSDK_base_function_sipsvr_status;

//-------------------------------------组件从应用层获取的消息---------------------------------------

/**
 * \brief   组件从从设备获取信息
 */
typedef enum {
    //组件从设备处获取
    SIPDEVSDK_BASE_FUNC_RT_INFO_GET_DEV_LONGNUM,          		///< 从设备获取设备长号，消息体: sipDevSDK_base_function_dev_longNum
    SIPDEVSDK_BASE_FUNC_RT_INFO_GET_DEV_TYPE,          		    ///< 从设备获取设备类型，消息体: sipDevSDK_base_function_dev_type
	SIPDEVSDK_BASE_FUNC_RT_INFO_GET_LOCAL_IP,          		    ///< 从设备获取本地IP，消息体: \c sipDevSDK_base_function_local_ip
	SIPDEVSDK_BASE_FUNC_RT_INFO_GET_MANGER_CENTER,          	///< 从设备管理中心信息，消息体: \c sipDevSDK_base_function_manager_info
    SIPDEVSDK_BASE_FUNC_RT_INFO_GET_REGISTER_LIST,         		///< 从设备获取所有注册列表，消息体: \c ezDevSDK_base_function_register_list
    SIPDEVSDK_BASE_FUNC_RT_INFO_GET_DEV_OVERSEA,         		///< 从设备获取是否海外版本，消息体: \c sipDevSDK_base_function_dev_oversea
    SIPDEVSDK_BASE_FUNC_RT_INFO_GET_TIME_FOR_REPLY,         	///< 获取本地时间用于注册回复报文,GMT格式，消息体: \c sipDevSDK_base_function_reply_time
    SIPDEVSDK_BASE_FUNC_RT_INFO_GET_SIP_STATUS,         		///< 从设备获取SIP客户端状态: \c sipDevSDK_base_function_sip_status
 	SIPDEVSDK_BASE_FUNC_RT_INFO_GET_DEV_INDUS_LONGNUM,          ///< 从设备获取设备行业新长号，消息体: sipDevSDK_base_function_dev_longNum
 	SIPDEVSDK_BASE_FUNC_RT_INFO_GET_TRUSTEESHIP_INFO,          ///< 从设备获取托管信息，消息体: sipDevSDK_base_function_trusteeship_info
 	SIPDEVSDK_BASE_FUNC_RT_INFO_GET_ADVANCED_SIPADDR,          ///< 从设备获取高级sip服务地址，消息体: sipDevSDK_base_function_advSipServer_ip
 	SIPDEVSDK_BASE_FUNC_RT_INFO_GET_ISNEED_TRANS,              ///< 从设备获取当前message是否需要转发，消息体: sipDevSDK_base_function_advSipServer_ip
}SIPDEVSDK_BASE_FUNCTION_RUNTIME_INFO_E;


/**
 * \brief   探视回调接口返回状态
 */
typedef enum {
    SIPDEVSDK_RET_ERR = -1, 			///异常返回
    SIPDEVSDK_RET_OK = 1,          		///正常返回
    SIPDEVSDK_RET_AUTH_SUSS,          	///鉴权成功
    SIPDEVSDK_RET_AUTH_REJECT,          ///鉴权拒绝
    SIPDEVSDK_RET_DUPLICATE_VISIT,      ///重复的探视请求
}SIPDEVSDK_VISIT_RET_INFO;

/*探视事件 */
typedef enum
{
	VISIT_EVENT_TYPE_INVLID = -1,			/* 无效类型 */
	VISIT_EVENT_TYPE_REQUEST = 1,			    /* 收到探视来电 */	
	VISIT_EVENT_TYPE_ANSWER,			    /* 探视正常接听 */	
	VISIT_EVENT_TYPE_RING,			        /* 探视双方*/	
	VISIT_EVENT_TYPE_CANCEL,			    /* 探视通话挂断或取消*/
	VISIT_EVENT_TYPE_FAILED,			    /* 探视失败*/	
} VISIT_EVENT_TYPE;


/**
 * \brief   从设备获取SIP客户端状态
 */
typedef struct {
    bool sip_busy;				///< 从设备获取SIP客户端状态,ture:忙碌，false空闲
} sipDevSDK_base_function_sip_status;

/**
 * \brief   获取本地设备长号
 */
typedef struct {
    char data[32];				///< 获取本地时间用于注册回复报文,GMT格式
} sipDevSDK_base_function_reply_time;


/**
 * \brief   获取本地设备长号
 */
typedef struct {
    char data[64];				///< 获取本地设备长号
} sipDevSDK_base_function_dev_longNum;

/**
 * \brief   获取本地设备类型
 */
typedef struct {
    int dev_type;				///< 获取本地设备类型 SIP_DEV_TYPE_E
} sipDevSDK_base_function_dev_type;

/**
 * \brief   获取本地设备版本
 */
typedef struct {
    bool type;				///< 获取本地设备海外版本,true为海外版本
} sipDevSDK_base_function_dev_oversea;


/**
 * \brief   获取本地IP
 */
typedef struct {
    char data[16];				///< 获取本地IP
} sipDevSDK_base_function_local_ip;


typedef struct {
  int num;
  #ifdef ANDROID_MANAGER_SIPSVR 
  REGISTER_INFO reg[MAX_REGNUM];///< 门口机/室内机所有注册列表，安卓管理机最大支持1万路
  #else
   REGISTER_INFO reg[MAX_ROOM_REGNUM];///< 室内机所有注册列表(国内最多分机5台，海外最多分机15台)
  #endif
} sipDevSDK_base_function_register_list;

/**
 * \brief   管理机信息，对应消息类型: SIPDEVSDK_BASE_FUNC_RT_INFO_GET_MANGER_CENTER
 */
typedef struct {
	int    dev_type;		//设备类型
	char   dev_name[32];	//设备名称
	char   call_num[20];	//设备长号
	char   dev_ip[16];		//设备IP
	char   reg_time[32];	//注册时效
	char   mac_addr[20];	//mac地址
	char   serial_no[48];	//序列号
} sipDevSDK_base_function_manager_info;

/**
 * \brief   托管信息，对应消息类型: SIPDEVSDK_BASE_FUNC_RT_INFO_GET_TRUSTEESHIP_INFO
 */
typedef struct {
	bool   trusteeshipEnableFlag;	//托管使能标志
	char	trusteeshipNum[32];		   /* 托管长号*/
} sipDevSDK_base_function_trusteeship_info;

/**
 * \brief   高级sip服务地址，对应消息类型: SIPDEVSDK_BASE_FUNC_RT_INFO_GET_ADVANCED_SIPADDR
 */
typedef struct {
	char   advSipServerIpAddr[16];	//高级sip服务地址
	int    advSipServer_port;       //高级sip服务端口
} sipDevSDK_base_function_advSipServer_ip;

/**
 * \brief   是否，对应消息类型: SIPDEVSDK_BASE_FUNC_RT_INFO_GET_ISNEED_TRANS
 */
typedef struct {
	char    uri[64];		   /* message uri*/
	char    fromNum[32];	   /* message from num*/
	bool    needFlag;	      //是否转发标志
} sipDevSDK_base_function_message_check_info;

#ifdef __cplusplus
}
#endif

#endif //H_SIPDEVSDK_BASE_FUNCTION_DEF_H_
