#include "CRtspListener.h"
#include "net/SocketUtil.h"

CRtspListener::CRtspListener(void)
    : m_running(false)
    , m_sock(INVALID_SOCKET)
    , m_Server_family(AF_INET)
{

}

CRtspListener::~CRtspListener()
{
    close();
}

int CRtspListener::open(const unsigned short port)
{
    sockaddr_in LocalAddr;
    memset(&LocalAddr, 0, sizeof(sockaddr_in));

    m_sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == m_sock)
    {
        printf("HPR_CreateSocket failed \n");
        return -1;
    }

    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    xop::SocketUtil::SetReuseAddr(m_sock);
    xop::SocketUtil::SetSendBufSize(m_sock, RTSPS_SOCKET_SEND_BUFFER);
    xop::SocketUtil::SetRecvBufSize(m_sock, RTSPS_SOCKET_RECV_BUFFER);

    if (::bind(m_sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        printf(" <socket=%d> bind <%u> failed.\n", m_sock, port);
        return -1;
    }

    if (::listen(m_sock, RTSPS_MAX_LISTENER_QUEUE_LEN) == SOCKET_ERROR) {
        printf("<socket=%d> listen failed.\n", m_sock);
        return false;
    }
    m_running = true;
    printf("HPR_Thread_Create, the port is [%d]\n", port);
    m_ThreadListen = std::thread([=]()
        {
            this->accept_handler_svc();

        });

    return 0;

}

void CRtspListener::accept_handler_svc()
{
    
}

void CRtspListener:: set_net_family(unsigned short family)
{
    m_Server_family = family; 
}

int CRtspListener::close()
{
    if (INVALID_SOCKET != m_sock)
    {
        closesocket(m_sock);
        m_sock = INVALID_SOCKET;
    }
    m_running = false;
    m_ThreadListen.join();
    return 0;
}

