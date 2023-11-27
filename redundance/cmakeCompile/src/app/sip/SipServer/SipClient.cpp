#include "SipClient.hpp"
using namespace std;

static constexpr int SIP_PORT = 5060;

SipClient::SipClient(std::string number, sockaddr_in address) : _number(std::move(number)), _address(std::move(address))
{
}

int SipClient::proc_event(eXosip_event_t *evnt)
{
	switch (evnt->type)
	{
	case EXOSIP_REGISTRATION_SUCCESS: // 注册成功
		cout << "EXOSIP_REGISTRATION_SUCCESS " << endl;
		break;
	default:
		break;
	}
	return 0;
}

void SipClient::startListen()
{
	_keepRunning = true;
	_listenThread = std::thread([=]() // 隐式捕获 以值捕获方式
								{	
			//更改线程名
			pthread_setname_np(pthread_self(), "listenThread");
			int ret = 0;
			ret = eXosip_init();
			if(ret != OSIP_SUCCESS){
				cout << "eXosip_init failed ret = " << ret << endl;
				return;
			}
			ret = eXosip_listen_addr(IPPROTO_UDP, NULL, SIP_PORT, AF_INET, 0);
			if(ret != OSIP_SUCCESS){
				cout << "eXosip_listen_addr failed ret = " << ret << endl;
				return;
			}
			eXosip_set_user_agent("HKVS/2.0.0");

			while (_keepRunning)
			{
				if (!_keepRunning) return;
				auto evnt = eXosip_event_wait(1, 0); 
				if (NULL != evnt)
				{
					proc_event(evnt);
					eXosip_event_free(evnt);
					evnt = NULL;
				}
			} });
}

void SipClient::doRegister()
{
	int ret = 0;
	char fromSipAddr[256] = { 0 };     //from : sip:主叫用户名@被叫IP地址, 
    char proxy[256] = { 0 };    //to : sip:被叫IP地址:被叫IP端口, 
	char  contact[128] = {0}; 
	char  local_ip[128] = {0}; 
	osip_message_t *reg = NULL;

	const char* displayName = "mike";
	const char* phoneno     = "123456789";
	const char* localIpAddr = "10.6.120.2";
	// snprintf(fromSipAddr, sizeof(fromSipAddr), "%s<sip:%s@%s>", displayName, phoneno, localIpAddr);
	// cout << "fromSipAddr: " << fromSipAddr << endl;
	// snprintf(proxy, sizeof(proxy), "sip:%s@:5060",localIpAddr);
	// cout << "proxy: " << proxy << endl;

	ret = eXosip_guess_localip(AF_INET, local_ip, 128);
	if(ret != OSIP_SUCCESS){
		cout << "eXosip_guess_localip failed ret = : " << ret << endl;
		return;
	}

	sprintf(fromSipAddr, "sip:%s@%s:%d", displayName, local_ip, SIP_PORT);
	sprintf(contact, "sip:%s@%s:%d", displayName, local_ip, SIP_PORT);
	//sprintf(contact,"<sip:%s@%s:%d>",deviceId,server_ip,server_port);
	sprintf(proxy, "sip:%s@%s:%d", "mikeproxy", local_ip, SIP_PORT);

	ret = eXosip_clear_authentication_info();
	if(ret != OSIP_SUCCESS){
		cout << "eXosip_clear_authentication_info failed ret = : " << ret << endl;
		return;
	}

	int nRegisterId = eXosip_register_build_initial_register(fromSipAddr, proxy, contact, 3600, &reg);
	if (nRegisterId < 0)
	{
		cout << "eXosip_register_build_initial_register failed nRegisterId = " << nRegisterId << endl;
		return;
	}

	eXosip_lock();
	ret = eXosip_register_send_register(nRegisterId, reg);
	eXosip_unlock();
	if (OSIP_SUCCESS != ret)
	{
		cout << "eXosip_register_send_register failed nRegisterId = " << ret << endl;
		return;
	}
}

void SipClient::closeListen()
{
	_keepRunning = false;
	_listenThread.join();
}

bool SipClient::operator==(SipClient other)
{
	if (_number == other.getNumber())
	{
		return true;
	}

	return false;
}

std::string SipClient::getNumber() const
{
	return _number;
}

sockaddr_in SipClient::getAddress() const
{
	return _address;
}

SipClient::~SipClient(){
	closeListen();
}