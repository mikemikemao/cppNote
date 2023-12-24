// sipSvrHicore.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "sipDevSDK_base_function_def.h"
#include "sipDevSDK_base_function.h"


int sipDevSDK_cb_recv_sipsvr_msg(sipDevSDK_base_function_msg_to_dev* msg)
{

	if (NULL == msg)
	{
		return -1;
	}
	switch (msg->type)
	{
	case SIPDEVSDK_BASE_FUNC_ON_REGISTER_INFO:
	{	//获取组件注册信息
		if (NULL != msg->data)
		{
			sipDevSDK_base_function_register_info* reg_info = (sipDevSDK_base_function_register_info*)msg->data;
			printf("sipDevSDK_base_function_register_info callNum %s \n", reg_info->call_num);
		}
	}
	break;
	case SIPDEVSDK_BASE_FUNC_ON_UNREGISTER_INFO:
	{	//获取组件注销信息
		if (NULL != msg->data)
		{
			sipDevSDK_base_function_register_info* reg_info = (sipDevSDK_base_function_register_info*)msg->data;
			printf("dpi_delete_dev_reg_info callNum %s \n", reg_info->call_num);
		}

	}
	break;

	case SIPDEVSDK_BASE_FUNC_ON_REGISTER_UPDATE:
	{	//获取组件注销信息
		if (NULL != msg->data)
		{
			sipDevSDK_base_function_register_info* reg_info = (sipDevSDK_base_function_register_info*)msg->data;
			printf("dpi_update_dev_reg_info success callNum %s \n", reg_info->call_num);
		}

	}
	break;

	case SIPDEVSDK_BASE_FUNC_ON_SIPAUTH_INFO:
	{
		if (NULL != msg->data)
		{

		}
	}
	case SIPDEVSDK_BASE_FUNC_ON_SIPSVR_STATUS:
	{
		if (NULL != msg->data)
		{
			sipDevSDK_base_function_sipsvr_status* status = (sipDevSDK_base_function_sipsvr_status*)msg->data;
			printf("sipDevSDK_base_function_sipsvr_status %d\n", status->busy);
		}
	}
	break;
	default:
		break;

	}
	return 0;
}

int sipDevSDK_cb_get_runtime_info(sipDevSDK_base_function_runtime_info* info)
{
	int ret = 0;
	if (NULL == info)
	{
		return -1;
	}
	switch (info->type)
	{
	case SIPDEVSDK_BASE_FUNC_RT_INFO_GET_REGISTER_LIST:
	{
		sipDevSDK_base_function_register_list regList = { 0 };
		info->data = &regList;
		printf("sipDevSDK_base_function_register_list done\n");
	}
	break;
	case SIPDEVSDK_BASE_FUNC_RT_INFO_GET_DEV_TYPE:
	{
		sipDevSDK_base_function_dev_type dev_type;
		dev_type.dev_type = SIP_DEV_TYPE_ROOM;
		info->data = &dev_type;
	}
	break;
	case SIPDEVSDK_BASE_FUNC_RT_INFO_GET_DEV_OVERSEA:
	{
		sipDevSDK_base_function_dev_oversea oversea = { 0 };
		oversea.type = false;
		info->data = &oversea;
		printf("sipDevSDK_base_function_dev_oversea %d\n", oversea.type);
	}
	break;
	case SIPDEVSDK_BASE_FUNC_RT_INFO_GET_LOCAL_IP:
	{
		sipDevSDK_base_function_local_ip dev_ip;
		snprintf(dev_ip.data, sizeof(dev_ip.data), "%s", "127.0.0.0");
		info->data = &dev_ip;
		printf("dev_ip %s\n", dev_ip.data);
	}
	break;
	case SIPDEVSDK_BASE_FUNC_RT_INFO_GET_DEV_LONGNUM:
	{
		sipDevSDK_base_function_dev_longNum dev_longNUM;
		snprintf(dev_longNUM.data, sizeof(dev_longNUM.data), "%s", "10010100000");
		info->data = &dev_longNUM;
		printf("dev_longNUM %s\n", dev_longNUM.data);
		info->len = 16;
	}
	break;

	case SIPDEVSDK_BASE_FUNC_RT_INFO_GET_MANGER_CENTER:
	{
		sipDevSDK_base_function_manager_info reg_info;
		memset((char*)&reg_info, 0, sizeof(sipDevSDK_base_function_manager_info));
		reg_info.dev_type = SIP_DEV_TYPE_MANAGER;
		snprintf(reg_info.call_num, sizeof(reg_info.call_num), "%s", MANAGE_CENTER_CALL_NUM);
		snprintf(reg_info.dev_ip, sizeof(reg_info.dev_ip), "%s", "10.7.114.88");
		snprintf(reg_info.reg_time, sizeof(reg_info.reg_time), "2037-12-31 23:59:59");
		snprintf(reg_info.serial_no, sizeof(reg_info.serial_no), "999999999");
		snprintf(reg_info.mac_addr, sizeof(reg_info.mac_addr), "99:99:99:99:99:99");
		info->data = &reg_info;
		printf("manageCenterIp %s\n", reg_info.dev_ip);
		info->len = 156;
	}
	break;

	default:
		break;

	}
	return 0;
}



int main()
{
	sipDevSDK_base_function_init_info init_info;
	memset(&init_info, 0, sizeof(sipDevSDK_base_function_init_info));
	init_info.cb.recv_msg = sipDevSDK_cb_recv_sipsvr_msg;
	init_info.cb.get_runtime_info = sipDevSDK_cb_get_runtime_info;
	init_info.config._reg_Cap = 10;
	init_info.config._sip_port = 5065;
	init_info.config._standard_sip = false;
	init_info.config._sip_auth = false;
	printf("<YCS>sip_start...\n");
	sipDevSDK_Init(&init_info);
	sipDevSDK_Start();

	getchar();
	return 0;
}
