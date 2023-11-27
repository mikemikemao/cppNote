#include "sipTest.h"
#include "SipServer.h"
#include "SipClient.hpp"
#include "string.h"
void sipTest(){
   SipServer sipServer("10.6.120.2");
   std::cout << "Server has been started. Listening..." << std::endl;
   sockaddr_in sipClentAddr;
   memset(&sipClentAddr, 0, sizeof(sipClentAddr));
   SipClient sipClient("11111111111111111",sipClentAddr);
   sipClient.startListen();
   sleep(5);
   sipClient.doRegister();
   getchar();
}