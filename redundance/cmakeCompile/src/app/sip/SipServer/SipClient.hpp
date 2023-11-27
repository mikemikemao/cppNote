#ifndef SIP_CLIENT_HPP
#define SIP_CLIENT_HPP

#ifdef __linux__
#include <netinet/in.h>
#elif defined _WIN32 || defined _WIN64
#include <WinSock2.h>
#endif

#include <iostream>
#include "eXosip2/eX_setup.h"
#include "eXosip2/eXosip.h"

#include <iostream>
#include <atomic>
#include <functional>
#include <thread>
#include "string.h"
using namespace std;

class SipClient
{
public:
	SipClient(std::string number, sockaddr_in address);
	~SipClient();

	bool operator==(SipClient other);

	std::string getNumber() const;
	sockaddr_in getAddress() const;
	void startListen();
	void closeListen();
	void doRegister();

private:
	int proc_event(eXosip_event_t* evnt);

private:
	std::string _number;
	sockaddr_in _address;
	std::atomic<bool> _keepRunning;
	std::thread _listenThread;
};

#endif