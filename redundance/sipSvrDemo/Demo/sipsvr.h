#ifndef sipsvr_H
#define sipsvr_H

#if (defined(_WIN32) || defined(_WIN64))
    #if defined(SIPSVR_EXPORTS)
        #define SIP_HSERVICE(type) extern "C" __declspec(dllexport) type __stdcall
    #else
        #define SIP_HSERVICE(type) type
    #endif
#else
    #define SIP_HSERVICE(type) extern "C" type
#endif

SIP_HSERVICE(int) sip_init();

SIP_HSERVICE(int) sip_fini();

SIP_HSERVICE(int) sip_start();

SIP_HSERVICE(int) sip_stop();

SIP_HSERVICE(int) sip_get_last_err();

SIP_HSERVICE(const char*) sip_get_version();

SIP_HSERVICE(int) sip_gtest(int argc, char* argv[]);

#endif /*end of sipsvr_H*/
