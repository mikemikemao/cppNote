/** @file:   application.h
 *  @note:   HangZhou Hikvision System Technology Co.,Ltd All Right Reserved
 *  @brief:  sip������Ӧ��
 *
 *  @author: wangfeida@hikvision.com.cn
 *  @data:   2014/08/05
 *  
 *  @note:   
 *
 *  @Modification History:
 *  <version>       <time>          <author>        <desc>
 *    V1.0        2014/08/05        wangfeida       Create
 *
 *  @warning: <No waining>        
*/

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include<hpr/HPR_Mutex.h>

class sip_application
{
private:
    sip_application();
    virtual ~sip_application();

public:

    HPR_INT32 init();
    HPR_INT32 finit();

    HPR_INT32 start_app();
    HPR_INT32 stop_app();

    //����
    HPR_INT32 restart();

    //���������߳�
    HPR_INT32 startup_restart();

    HPR_INT32 set_exit();

    static HPR_INT32 create_instance();
    static HPR_VOID del_instance();
    static sip_application* instance();

private:

    static HPR_VOIDPTR CALLBACK restart_thread(HPR_VOIDPTR thread_param);

private:

    static sip_application* _instance;
    //�Ƿ�������
    HPR_BOOL _start;
    HPR_Mutex _app_mutex;
    HPR_Mutex _restart_mutex;
    HPR_BOOL _exit;

    //�����߳�
    HPR_BOOL _restart_thrd;
    HPR_BOOL _restarting;

};

#endif
