/** @file:   device_factory.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  设备工厂类
 *
 *  @author: wangfeida@hikvision.com.cn
 *  @data:   2014/07/15
 *  
 *  @note:   
 *
 *  @Modification History:
 *  <version>       <time>          <author>        <desc>
 *    V1.0        2014/07/15        wangfeida       Create
 *
 *  @warning: <No waining>        
*/

#ifndef _DEVICE_FACTORY_H_
#define _DEVICE_FACTORY_H_

#include"definer.h"
#include"device.h"
#include"service_mgr.h"

class device_factory_t
{
private:
    device_factory_t(){}
    virtual ~device_factory_t(){}

public:

    static device_t* create_device(const vt_phone_t& vt_phone)
    {
        device_t* device = NULL;
        device = new(std::nothrow)device_t(vt_phone);

        return device;
    }

};

#endif
