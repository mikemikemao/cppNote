#include"sipsvr.h"
#include"application.h"

//sip_application app;

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include<stdlib.h>
#include<crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

SIP_HSERVICE(int) sip_init()
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
    if (sip_application::create_instance() != 0)
    {
        return -1;
    }

    return sip_application::instance()->init();
}

SIP_HSERVICE(int) sip_fini()
{
    sip_application::instance()->finit();
    sip_application::del_instance();

#ifdef _DEBUG
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}

SIP_HSERVICE(int) sip_start()
{

    sip_application::instance()->start_app();

    return 0;
}

SIP_HSERVICE(int) sip_stop()
{

    sip_application::instance()->stop_app();

    return 0;
}

SIP_HSERVICE(int) sip_get_last_err()
{
    return 0;
}

SIP_HSERVICE(const char*) sip_get_version()
{
    return "V1.0.0";
}

SIP_HSERVICE(int) sip_gtest( int argc, char* argv[] )
{
    return 0;
}

