#include<hlog/logdef.h>

#include"service_mgr.h"
#include"service_sip.h"

service_mgr_t* service_mgr_t::_instance = NULL;

service_mgr_t::service_mgr_t()
{

}

service_mgr_t::~service_mgr_t()
{

}

service_mgr_t* service_mgr_t::instance()
{
    if (_instance == NULL)
    {
        _instance = new(std::nothrow) service_mgr_t;
        if (_instance == NULL)
        {
            SIP_SVR_ERROR("NO MEMORY.");
        }
    }

    return _instance;
}

HPR_VOID service_mgr_t::del_instance()
{
    if (_instance != NULL)
    {
        delete _instance;
        _instance = NULL;
    }
    return;
}

HPR_INT32 service_mgr_t::init()
{
    HPR_INT32 rt = HPR_ERROR;
    service_base_t* service = NULL;

    do 
    {
        service = new(std::nothrow) service_sip_t;
        if (service == NULL)
        {
            SIP_SVR_ERROR("NO MEMORY.");
            break;
        }

        _service_map[SIP_SERVICE] = service;

        if (HPR_ERROR == service->init())
        {
            SIP_SVR_ERROR("SIP SERVICE init FAIL.");
            break;
        }

        rt = HPR_OK;
    } while (0);

    return rt;
}

HPR_INT32 service_mgr_t::fini()
{
    SIP_SVR_INFO("enter fini.");
    HPR_INT32 rt = HPR_ERROR;
    do 
    {
        SERVICEMAP_ITER it = _service_map.begin();
        for (; it != _service_map.end(); it++)
        {
            it->second->fini();
            delete it->second;
            it->second = NULL;
        }

        _service_map.clear();
    } while (0);
    
    return rt;
}

service_base_t* service_mgr_t::get_service_object( service_type type )
{
    SERVICEMAP_ITER it = _service_map.find(type);
    if (it != _service_map.end() )
    {
        return it->second;
    }

    return NULL;
}
