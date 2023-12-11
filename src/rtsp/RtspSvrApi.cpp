#include <stdio.h>
#include <new>
#include "RtspSvrApi.h"
#include "RtspCom.h"

RtspSvrApi::RtspSvrApi()
	:p_RtspServer(nullptr)
	, p_RtspSessionMgr(nullptr) {}


void RtspSvrApi::init_rtsp_server() 
{
    int ret = 0;
    p_RtspSessionMgr = new(std::nothrow) CRtspSessionManager();
    p_RtspServer = new(std::nothrow) CRtspServer();
    if (nullptr == p_RtspSessionMgr || nullptr == p_RtspServer)
    {
        printf("New p_RtspSessionMgr or p_RtspServer is NULL\n");
        return;
    }
    ret = p_RtspServer->start(RtspCom::RTSP_PORT);
    if (ret != 0)
    {
        printf("RtspServer start failed\n");
        return;
    }
}
