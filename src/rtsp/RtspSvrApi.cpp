#include <stdio.h>
#include <new>
#include "RtspSvrApi.h"

RtspSvrApi::RtspSvrApi()
	:p_RtspServer(nullptr)
	, p_RtspSessionMgr(nullptr) {}


void RtspSvrApi::init_rtsp_server() {
    p_RtspSessionMgr = new(std::nothrow) CRtspSessionManager();
    p_RtspServer = new(std::nothrow) CRtspServer();
    if (nullptr == p_RtspSessionMgr || nullptr == p_RtspServer)
    {
        printf("New p_RtspSessionMgr or p_RtspServer is NULL\n");
        return;
    }
    //p_RtspServer.start(RTSP_PORT);
}
