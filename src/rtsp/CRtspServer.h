#pragma once
#include "CNet_PortPool.h"
#include "CRtspListener.h"
#include <mutex>
class CRtspServer
{
public:
	CRtspServer();
	~CRtspServer();
	int start(const unsigned short port);
private:
	void release_res();
private:
	CRtspListener* m_pListener;
	CNet_PortPool* m_pPortQueue;
	unsigned short m_ListenPort;
};

