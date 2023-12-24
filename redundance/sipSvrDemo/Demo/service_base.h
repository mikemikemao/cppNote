/**
 * @file service_base.h
 * @brief 基础服务
 * @author zhanghaifengyf1@hikvision.com.cn
 * @version 0.0.1
 * @date 2014-08-08 15:35:29
 */

#ifndef SERVICE_BASE_APBRU4P8
#define SERVICE_BASE_APBRU4P8

class service_base_t
{
public:
    service_base_t (/* arguments*/) {}
    virtual ~service_base_t () {}

    virtual HPR_INT32 init() = 0;
    virtual HPR_VOID fini() = 0;

private:
} /* service_base_t */;

#endif /* end of include guard: SERVICE_BASE_APBRU4P8 */

