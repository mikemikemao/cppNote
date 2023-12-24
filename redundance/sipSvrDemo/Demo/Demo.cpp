// Demo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "sipsvr.h"

#pragma comment(lib,"hlog.lib")
#pragma comment(lib,"hpr.lib")
#pragma comment(lib,"osipparser.lib")
#pragma comment(lib,"tinyxml.lib")
#pragma comment(lib,"NetManagerAgent.lib")
#pragma comment(lib,"sqlite3.lib")
#pragma comment(lib,"libeay32.lib")
#pragma comment(lib,"ssleay32.lib")

#pragma comment(lib,"iconv.lib")
#pragma comment(lib,"rutil.lib")
#pragma comment(lib,"snmpAgent.lib")

#pragma comment(lib,"gsoap.lib")
#pragma comment(lib,"gsoap_server.lib")
#pragma comment(lib,"sipws_service.lib")
#pragma comment(lib,"cmsws_sip_service.lib")
int _tmain(int argc, _TCHAR* argv[])
{
	int ret = 0;
	ret = sip_init();
	if (ret != 0)
	{
		printf("sip init failed ret =%d \n",ret);
		return ret;
	}
	ret = sip_start();
	if (ret != 0)
	{
		printf("sip_start failed ret =%d \n",ret);
		return ret;
	}
	getchar();
	return 0;
}

