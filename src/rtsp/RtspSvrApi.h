#pragma once
#include "CRtspServer.h"
#include "CRtspSessionManager.h"

class RtspSvrApi
{
public:
	RtspSvrApi();
	void init_rtsp_server();
private:
	CRtspServer* p_RtspServer;
	CRtspSessionManager* p_RtspSessionMgr;
};

