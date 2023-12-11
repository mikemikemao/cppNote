#include "CRtspServer.h"
#include "RtspCom.h"
#include <winsock.h>
CRtspServer::CRtspServer(void)
    : m_pListener(nullptr)
    , m_pPortQueue(nullptr)
    , m_ListenPort(RtspCom::RTSP_PORT){

}

CRtspServer::~CRtspServer(void){
    release_res();
}

void CRtspServer::release_res(){
    if (m_pListener)
    {
        delete m_pListener;
        m_pListener = NULL;
    }

    if (m_pPortQueue)
    {
        delete m_pPortQueue;
        m_pPortQueue = NULL;
    }
}

int CRtspServer::start(const unsigned short port) {
    m_pListener = new(std::nothrow) CRtspListener();
    m_ListenPort = port;
    m_pPortQueue = CNet_PortPool::create_new(RtspCom::MIN_PORT, RtspCom::MAX_PORT);
    m_pListener->set_net_family(AF_INET);
    return 0;
}
