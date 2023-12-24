#include<hlog/logdef.h>
#include<hpr/HPR_Utils.h>
#include<hpr/HPR_Guard.h>
#include<hpr/HPR_Timer.h>

#include"definer.h"
#include"net_manager.h"
#include"addr_book_mgr.h"

HPR_VOIDPTR CALLBACK receiver(HPR_VOIDPTR p)
{
    net_manager_t* obj = static_cast<net_manager_t*>(p);
    obj->do_recv();
    return NULL;
}

HPR_VOIDPTR CALLBACK handler(HPR_VOIDPTR p)
{
    task_buffer* obj = static_cast<task_buffer*>(p);
    if (obj == NULL || obj->_user_data == NULL)
    {
        return NULL;
    }

    net_manager_t* n = static_cast<net_manager_t*>(obj->_user_data);

    std::string msg(obj->_buf, obj->_buf_len);

    n->handle_message(msg, obj->_client_addr);

    n->free_memory_from_out(obj->_buf);

    return NULL;
}

HPR_VOIDPTR CALLBACK clear(HPR_VOIDPTR p)
{
    task_buffer* task = static_cast<task_buffer*>(p);
    if (task == NULL || task->_user_data == NULL)
    {
        return NULL;
    }

    net_manager_t* net_obj = static_cast<net_manager_t*>(task->_user_data);

    net_obj->free_memory_from_out(task->_buf);

    return NULL;
}

HPR_VOIDPTR send_timer(HPR_INT32 event, HPR_VOIDPTR user_data)
{
    SIP_SVR_INFO("send changed device, event:%d, userdata:%d", event, user_data);
    if (user_data == NULL)
    {
        return NULL;
    }

    net_manager_t* p = static_cast<net_manager_t*>(user_data);

    p->send_changed_device_status();
    return NULL;
}

net_manager_t* net_manager_t::_instance = NULL;

net_manager_t::net_manager_t( HPR_UINT16 net_manage_port )
: _net_manage_port(net_manage_port)
, _sock(HPR_INVALID_SOCKET)
, _run_status(HPR_FALSE)
, _status(0)
, _timer(-1)
{

}

net_manager_t::~net_manager_t()
{

}

HPR_INT32 net_manager_t::init()
{
    HPR_INT32 rt = HPR_ERROR;
    HPR_ADDR_T sock_addr;

    do 
    {
        _run_status = HPR_TRUE;

        //创建内存池
        if(_memory_pool.init(NETMANAGE_MESSAGE_BUF_LEN+1, 100) != 0)
        {
            SIP_SVR_ERROR("create udp buffer faild!");
            break;
        }

        //创建线程池
        if (_thread_pool.init(2*HPR_GetCPUNumber()+2) != HPR_OK)
        {
            SIP_SVR_ERROR("create thread pool failed.");
            break;
        }

        _sock = HPR_CreateSocket(AF_INET, SOCK_DGRAM, 0);
        if (_sock == HPR_INVALID_SOCKET)
        {
            SIP_SVR_ERROR("HPR_CreateSocket fail.");
            break;
        }

        SIP_SVR_INFO("HPR_CreateSocket socket:%d", _sock);

        HPR_MakeAddrByString(AF_INET, NULL, _net_manage_port, &sock_addr);
        if (HPR_ERROR == HPR_Bind(_sock, &sock_addr) )
        {
            SIP_SVR_ERROR("HPR_Bind fail.");
            break;
        }

        //创建接收线程
        work_data_t w_data;
        w_data.work_info = this;
        w_data.work_route = receiver;
        _thread_pool.push_work_list(w_data);

        if (-1 == HPR_SetTimer(send_timer, this, 3*60*1000, &_timer, HPR_FALSE) ) //每隔3分钟上传一次状态改变的设备
        {
            SIP_SVR_ERROR("HPR_SetTimer failed.");
            break;
        }

        SIP_SVR_INFO("Timer:%d, userdata:%d", _timer, this);

        if (-1 == NetManagerAgent_InitLib())
        {
            SIP_SVR_ERROR("NetManagerAgent_InitLib");
            break;
        }

        if (-1 == NetManagerAgent_StartService(config_t::instance()->get_net_manager_server_port()))
        {
            SIP_SVR_ERROR("NetManagerAgent_StartService failed.");
            break;
        }

        NetManagerAgent_Set_SIP_collector(this);

        rt = HPR_OK;
    } while (0);

    if (HPR_ERROR == rt)
    {
        fini();
    }

    return rt;
}

HPR_VOID net_manager_t::fini()
{
    SIP_SVR_INFO("begin stop net manager.");
    _run_status = HPR_FALSE;

    NetManagerAgent_StopService();
    NetManagerAgent_FiniLib();

    if (_sock != HPR_INVALID_SOCKET)
    {
        SIP_SVR_INFO("close socket, sock:%d.", _sock);
        ////此处需要先关闭socket的读操作，否则在centos下recvfrom不会返回
        HPR_ShutDown(_sock, HPR_SHUT_RDWR);
        HPR_CloseSocket(_sock);
        _sock = HPR_INVALID_SOCKET;
    }

    SIP_SVR_INFO("before kill timer.");
    if (_timer >= 0)
    {
        HPR_KillTimer(_timer);
        _timer = -1;
    }

    SIP_SVR_INFO("before clear map.");
    clear_changed_devices();

    SIP_SVR_INFO("before destroy thread pool.");
    _thread_pool.fini();

    SIP_SVR_INFO("before destroy memory pool.");
    _memory_pool.finit();

    SIP_SVR_INFO("exit stop net manager.");
    return;
}

HPR_VOID net_manager_t::do_recv()
{
    HPR_ADDR_T client_addr;
    HPR_INT32 count_of_bytes = 0;
    char* buf = NULL;
    task_buffer* task_buf = NULL;

    while (_run_status)
    {
        buf = static_cast<char*>(_memory_pool.get_memory());
        if (buf == NULL)
        {
            //若内存申请失败，则等待50ms后继续申请
            HPR_Sleep(5);
            continue;
        }

        count_of_bytes = HPR_RecvFrom(_sock, buf, NETMANAGE_MESSAGE_BUF_LEN, &client_addr);

        if (count_of_bytes > 80)
        {
            //合法请求
            //设置最后一个字节为0
            *(buf + count_of_bytes) = 0;

            task_buf = new(std::nothrow)task_buffer;
            if (task_buf == NULL)
            {
                SIP_SVR_ERROR("malloc memory faild!");
                _memory_pool.free_memory(buf);
                continue;
            }

            task_buf->_buf = buf;
            task_buf->_buf_len = count_of_bytes;
            task_buf->_user_data = this;
            memcpy(&task_buf->_client_addr, &client_addr, sizeof(client_addr) );

            //收到的信令，放入任务列表中
            work_data_t w_data;
            w_data.work_info = task_buf;
            w_data.work_route = handler;
            w_data.clear_route = clear;
            _thread_pool.push_work_list(w_data);
        }
        else if (count_of_bytes == HPR_ERROR)
        {
            _memory_pool.free_memory(buf);
            SIP_SVR_ERROR("HPR_RecvFrom failed.");
        }
        else
        {
            _memory_pool.free_memory(buf);
            SIP_SVR_WARN("Dummy net manage message recved.");
        }
    }
}

HPR_VOID net_manager_t::handle_message( std::string& msg, HPR_ADDR_T& addr )
{
    net_manage_request_t request;
    net_manage_response_t response;
    std::string res;

    if (std::string::npos != msg.find("request"))
    {
        request.parser(msg.c_str());

        std::string op = request.get_op_type();
        if (op == "REGISTER")
        {
            on_register(addr);

            response.set_op_type(op.c_str());
            response.set_result("true");
            response.set_result_msg("ok");
            response.set_err_code(0);
            res = response.builder();

            HPR_SendTo(_sock, (HPR_VOIDPTR)res.c_str(), res.length(), &addr);
        }
        else if (op == "UNREGISTER")
        {
            on_unregister();

            response.set_op_type(op.c_str());
            response.set_result("true");
            response.set_result_msg("ok");
            response.set_err_code(0);
            res = response.builder();

            HPR_SendTo(_sock, (HPR_VOIDPTR)res.c_str(), res.length(), &addr);
        }
        else if (op == "HEARTBEAT")
        {
            if (is_register()== HPR_TRUE)
            {
                response.set_op_type(op.c_str());
                response.set_result("true");
                response.set_result_msg("ok");
                response.set_err_code(0);
                res = response.builder();
            }
            else
            {
                response.set_op_type(op.c_str());
                response.set_result("false");
                response.set_result_msg("net manager is not online.");
                response.set_err_code(404);
                res = response.builder();
            }

            HPR_SendTo(_sock, (HPR_VOIDPTR)res.c_str(), res.length(), &addr);
        }
        else if (op == "GETALL")
        {
            if (HPR_TRUE == set_getall_status())
            {
                HPR_INT32 num_per_page = request.get_number_to_get();
                HPR_INT32 total_num = 0;
                NET_MANAGE_DEVICE_INFO_T* status = addr_book_mgr_t::instance()->get_devices_status(&total_num);
                HPR_INT32 page = (total_num%num_per_page==0)?(total_num/num_per_page):(total_num/num_per_page+1);
                for (int i=0; i<page; i++)
                {
                    response.set_op_type(op.c_str());
                    response.set_result("true");
                    response.set_result_msg("ok");
                    response.set_err_code(0);
                    response.set_number_to_send(num_per_page);
                    response.set_offset(i*num_per_page);
                    response.set_total_num(total_num);
                    if (i == (page-1) )
                    {
                        response.set_device_list( (total_num-i*num_per_page), status);
                    }
                    else
                    {
                        response.set_device_list(num_per_page, status);
                    }

                    res = response.builder();
                    HPR_SendTo(_sock, (HPR_VOIDPTR)res.c_str(), res.length(), &addr);
                }
                addr_book_mgr_t::instance()->free_devices_status(status);
                clear_changed_devices();
                set_idle_status();
            }
            else
            {
                response.set_op_type(op.c_str());
                response.set_result("false");
                response.set_result_msg("getall cmd have not been handled yet.");
                response.set_err_code(400);

                res = response.builder();
                HPR_SendTo(_sock, (HPR_VOIDPTR)res.c_str(), res.length(), &addr);
            }
        }
        else if (op == "GETPART")
        {
        }
        else
        {
            LOG_WARN("Unrecgnize req, op:%s", op.c_str());
            if (is_register()== HPR_TRUE)
            {
                response.set_op_type(op.c_str());
                response.set_result("true");
                response.set_result_msg("ok");
                response.set_err_code(0);
                res = response.builder();
            }
            else
            {
                response.set_op_type(op.c_str());
                response.set_result("false");
                response.set_result_msg("net manager is not online.");
                response.set_err_code(404);
                res = response.builder();
            }

            HPR_SendTo(_sock, (HPR_VOIDPTR)res.c_str(), res.length(), &addr);
        }
    }
    else if (std::string::npos != msg.find("response"))
    {

        response.parser(msg.c_str());

        std::string op = response.get_op_type();
        if (op == "SEND")
        {
        }
        else
        {
            LOG_ERROR("Unrecgnize rsp, op:%s", op.c_str());
        }
    }
}

HPR_VOID net_manager_t::on_register( HPR_ADDR_T& addr )
{
    HPR_Guard lock(&_status_lock);

    memcpy(&_net_manage_addr, &addr, sizeof(addr));
    HPR_AtomicCas(&_status, 1, 0);
}

HPR_VOID net_manager_t::on_unregister()
{
    HPR_Guard lock(&_status_lock);
    HPR_AtomicSet(&_status, 0);
}

HPR_BOOL net_manager_t::set_getall_status()
{
    HPR_Guard lock(&_status_lock);
    if (0 == HPR_AtomicCas(&_status, 0, 0) )
    {
        return HPR_FALSE;
    }
    else if (1 == HPR_AtomicCas(&_status, 17, 1))
    {
        return HPR_TRUE;
    }
    else if (16 == HPR_AtomicCas(&_status, 17, 16))
    {
        return HPR_TRUE;
    }
    else
    {
        return HPR_FALSE;
    }
}

HPR_VOID net_manager_t::set_idle_status()
{
    HPR_Guard lock(&_status_lock);
    HPR_AtomicCas(&_status, 16, 17);
}

HPR_VOID net_manager_t::free_memory_from_out( HPR_VOIDPTR buf )
{
    _memory_pool.free_memory(buf);
}

HPR_BOOL net_manager_t::is_register()
{
    HPR_Guard lock(&_status_lock);
    return HPR_AtomicCas(&_status, 0, 0)==0?HPR_FALSE:HPR_TRUE;
}

HPR_INT32 net_manager_t::send_changed_device_status()
{
    if (is_idle() == HPR_FALSE) //是否在第一次获取全量数据之前、是否注册、是否正在获取全量。空闲则继续
    {
        return 0;
    }

    net_manage_request_t request;
    NET_MANAGE_DEVICE_INFO_T* info = NULL;
    HPR_INT32 total_num = 0;
    HPR_INT32 num_per_page = 200;
    std::string req_str;
    {
        HPR_Guard lock(&_changed_devices_lock);

        total_num = _changed_devices.size();
        if (total_num > 0)
        {
            info = new(std::nothrow) NET_MANAGE_DEVICE_INFO_T[total_num];
            std::map<std::string,NET_MANAGE_DEVICE_INFO_T>::iterator it = _changed_devices.begin();
            HPR_INT32 i = 0;
            for (; it!=_changed_devices.end(); it++)
            {
                info[i] = it->second;
                i++;
            }
        }
    }
    HPR_INT32 page = (total_num%num_per_page==0)?(total_num/num_per_page):(total_num/num_per_page+1);
    for (int i=0; i<page; i++)
    {
        request.set_op_type("SEND");
        if (i == (page-1) )
        {
            request.set_device_list( (total_num-i*num_per_page), info);
        }
        else
        {
            request.set_device_list(num_per_page, info);
        }

        req_str = request.builder();
        if (-1 == HPR_SendTo(_sock, (HPR_VOIDPTR)req_str.c_str(), req_str.length(), &_net_manage_addr) )
        {
            SIP_SVR_ERROR("HPR_SendTo Send err. req:%s", req_str.c_str() );
        }
    }

    if (info != NULL)
    {
        delete[] info;
        info = NULL;
    }
    clear_changed_devices();

    return 0;
}

HPR_BOOL net_manager_t::is_idle()
{
    HPR_Guard lock(&_status_lock);
    if (16 == HPR_AtomicCas(&_status, 16, 16) )
    {
        return HPR_TRUE;
    }
    else
    {
        return HPR_FALSE;
    }
}

HPR_VOID net_manager_t::add_changed_device( std::string no, NET_MANAGE_DEVICE_INFO_T& info )
{
    HPR_Guard lock(&_changed_devices_lock);
    _changed_devices[no] = info;
}

HPR_VOID net_manager_t::clear_changed_devices()
{
    HPR_Guard lock(&_changed_devices_lock);
    _changed_devices.clear();
}

