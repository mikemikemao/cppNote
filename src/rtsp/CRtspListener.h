#ifndef __RTSPS_LISTENER_H__
#define __RTSPS_LISTENER_H__

#include <net\Socket.h>
#include <atomic>
#include <thread>

class CRtspListener
{
	static constexpr int RTSPS_SOCKET_SEND_BUFFER = 108 * 1024;
	static constexpr int RTSPS_SOCKET_RECV_BUFFER = 32 * 1024;
	static constexpr int RTSPS_MAX_LISTENER_QUEUE_LEN = 24;
public:
	CRtspListener();
	~CRtspListener();
	int open(const unsigned short port);
	int close();
	void set_net_family(unsigned short family);
private:
	void accept_handler_svc();
private:
	std::atomic<bool> m_running; // listener is running or not!
	SOCKET  m_sock;     //listen socket fd
	unsigned short m_Server_family; /* AF_INET, AF_INET6 */
	std::thread m_ThreadListen; //!< thread handle of alarm host server.

};
#endif
