#pragma once
#include "CRtspServer.h"
#include "CRtspSessionManager.h"

class RtspSvrApi
{
public:
	static constexpr int RTSP_PORT = 554;
	RtspSvrApi();
	void init_rtsp_server();
private:
	CRtspServer* p_RtspServer;
	CRtspSessionManager* p_RtspSessionMgr;
};

