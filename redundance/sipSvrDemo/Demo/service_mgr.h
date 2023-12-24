/********************************************************************
    created:    2014/08/10 13:30
    filename:   d:\Documents\Code\VisualTalkSipServer\sipsvr\src\sipsvr\service_mgr.h
    file path:  d:\Documents\Code\VisualTalkSipServer\sipsvr\src\sipsvr
    file base:  service_mgr
    file ext:   h
    author:     zhanghaifengyf1

    purpose:    服务管理类
*********************************************************************/
#ifndef service_mgr_h__
#define service_mgr_h__

#include<map>

#include"definer.h"
#include"service_base.h"

class service_mgr_t
{
    typedef std::map<service_type,service_base_t*> SERVICEMAP;
    typedef std::map<service_type,service_base_t*>::iterator SERVICEMAP_ITER;

public:
    static service_mgr_t* instance();
    static HPR_VOID del_instance();
    HPR_INT32 init();
    HPR_INT32 fini();

    service_base_t* get_service_object(service_type type);
private:
    service_mgr_t();
    ~service_mgr_t();

private:
    static service_mgr_t* _instance;
    SERVICEMAP _service_map;
};

#endif // service_mgr_h__
