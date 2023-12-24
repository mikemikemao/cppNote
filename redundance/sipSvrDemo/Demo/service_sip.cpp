#include<hpr/HPR_Thread.h>
#include<hpr/HPR_ThreadPool.h>
#include<hpr/HPR_Guard.h>
#include<hpr/HPR_Utils.h>
#include<hpr/HPR_String.h>
#include<hpr/HPR_Error.h>
#include<tinyxml/tinyxml.h>
#include<stdio.h>

#include"service_sip.h"
#include"addr_book_mgr.h"
#include"time_convert.h"
#include"ppl_md5.h"
#include"call_manager.h"
#include"db_manager.h"

HPR_VOIDPTR CALLBACK recv_worker(HPR_VOIDPTR p)
{
    service_sip_t* svc = (service_sip_t*)p;
    svc->recv_msg();
    return NULL;
}

service_sip_t::service_sip_t( /* arguments*/ )
: _run_status(HPR_FALSE)
, _sock(HPR_INVALID_SOCKET)
, _recv_thread(HPR_INVALID_THREAD)
, _send_thread(HPR_INVALID_THREAD)
#if (defined(_WIN32) || defined(_WIN64))
, _sem(HPR_INVALID_HANDLE)
#endif
{

}

/*lint -save -e1540*/
service_sip_t::~service_sip_t()
{
    _recv_thread = NULL;
    _send_thread = NULL;
}
/*lint -restore*/

HPR_INT32 service_sip_t::init()
{
    HPR_INT32 rt = HPR_ERROR;
    HPR_ADDR_T sock_addr;

    do
    {
        _run_status = HPR_TRUE;

        //初始化sip解析库
        parser_init();

        //创建信号量
        HPR_SemCreate(&_sem, 0);

        //创建内存池
        if(_memory_pool.init(SIP_MESSAGE_BUF_LEN+1, 3000) != 0)
        {
            SIP_SVR_ERROR("create udp buffer faild!");
            break;
        }

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

        HPR_MakeAddrByString(AF_INET, NULL, config_t::instance()->get_sip_port(), &sock_addr);
        if (HPR_ERROR == HPR_Bind(_sock, &sock_addr) )
        {
            SIP_SVR_ERROR("HPR_Bind fail.");
            break;
        }

        //创建发送线程
        _send_thread = HPR_Thread_Create(send_worker, this, 0);
        if (_send_thread == HPR_INVALID_THREAD)
        {
            SIP_SVR_ERROR("create send thread fail.");
            break;
        }

        //创建接收线程
        _recv_thread = HPR_Thread_Create(recv_worker, this, 0);
        if (_recv_thread == HPR_INVALID_THREAD)
        {
            SIP_SVR_ERROR("HPR_Thread_Create fail.");
            break;
        }

        rt = HPR_OK;
    } while (0);

    if (rt != HPR_OK)
    {
        fini();
    }

    return rt;
}

HPR_VOID service_sip_t::fini()
{
    SIP_SVR_INFO("enter fini.");
    _run_status = HPR_FALSE;

    if (_sock != HPR_INVALID_SOCKET)
    {
        SIP_SVR_INFO("close socket, sock:%d.", _sock);
        ////此处需要先关闭socket的读操作，否则在centos下recvfrom不会返回
        HPR_ShutDown(_sock, HPR_SHUT_RDWR);
        HPR_CloseSocket(_sock);
        _sock = HPR_INVALID_SOCKET;
    }


#if (defined(_WIN32) || defined(_WIN64))
    if (_sem != HPR_INVALID_HANDLE)
#endif
    {
        HPR_SemPost(&_sem);
    }

    if (_recv_thread != HPR_INVALID_THREAD)
    {
        HPR_Thread_Wait(_recv_thread);
        _recv_thread = HPR_INVALID_THREAD;
    }

    //销毁发送线程
    if (_send_thread != HPR_INVALID_THREAD)
    {
        HPR_Thread_Wait(_send_thread);
        _send_thread = HPR_INVALID_THREAD;
    }

    _thread_pool.fini();

#if (defined(_WIN32) || defined(_WIN64))
    if (_sem != HPR_INVALID_HANDLE)
#endif
    {
        HPR_SemDestroy(&_sem);
#if (defined(_WIN32) || defined(_WIN64))
        _sem = HPR_INVALID_HANDLE;
#endif
    }

    {
        HPR_Guard lock(&_send_task_mutex);
        std::list<sip_buffer*>::iterator itor = _send_task_list.begin();
        while (itor != _send_task_list.end())
        {
            osip_free((*itor)->_buf);
            delete (*itor);

            ++itor;
        }

        _send_task_list.clear();
    }

    //销毁内存池
    _memory_pool.finit();

    return;
}

HPR_VOIDPTR CALLBACK service_sip_t::send_worker(HPR_VOIDPTR thread_param)
{
    service_sip_t* svc = (service_sip_t*)thread_param;
    svc->send_msg();
    return NULL;
}

HPR_VOID service_sip_t::send_msg()
{
    sip_buffer* buf = NULL;
    std::list<sip_buffer*>::iterator itor;

    while (_run_status == HPR_TRUE)
    {
        HPR_SemWait(&_sem);
        {
            HPR_Guard task_lock(&_send_task_mutex);
            if (_send_task_list.empty())
            {
                continue;
            }
            buf = _send_task_list.front();
            _send_task_list.pop_front();
        }

        //发送数据
        if (HPR_SendTo(_sock, buf->_buf, buf->_buf_len, &(buf->_client_addr)) == -1)
        {
            //发送数据失败，一般为系统出现问题
            SIP_SVR_ERROR("HPR_SendTo failed, socket:%d, dest ip:%s, port:%d, err:%d", _sock, \
                           HPR_GetAddrString(&(buf->_client_addr)), HPR_GetAddrPort(&(buf->_client_addr)), HPR_GetLastError());
        }
        else
        {
            SIP_SVR_TRACE("HPR_SendTo buf:%s, ip:%s, port:%d", buf->_buf, HPR_GetAddrString(&(buf->_client_addr)), HPR_GetAddrPort(&(buf->_client_addr)) );
        }

        //释放资源
        osip_free(buf->_buf);
        delete buf;
    }

    return;
}

HPR_INT32 service_sip_t::send_data(sip_buffer* buf)
{
    do
    {
        HPR_Guard task_lock(&_send_task_mutex);
        if (_send_task_list.size() > 10000)
        {
            //删除第一条数据
            sip_buffer* del_buffer = _send_task_list.front();
            _send_task_list.pop_front();
            osip_free(del_buffer->_buf);
            delete del_buffer;
        }

        _send_task_list.push_back(buf);
    } while (0);

    HPR_SemPost(&_sem);

    return 0;
}

HPR_VOID service_sip_t::recv_msg()
{
    HPR_ADDR_T client_addr;
    HPR_INT32 count_of_bytes;

    sip_buffer* task_buf;
    char* buf;

    while(_run_status == HPR_TRUE)
    {
        buf = static_cast<char*>(_memory_pool.get_memory());
        if (buf == NULL)
        {
            //若内存申请失败，则等待50ms后继续申请
            HPR_Sleep(5);
            continue;
        }

        count_of_bytes = HPR_RecvFrom(_sock, buf, SIP_MESSAGE_BUF_LEN, &client_addr);

        if (count_of_bytes > 5)
        {
            //合法请求
            //设置最后一个字节为0
            *(buf + count_of_bytes) = 0;

            SIP_SVR_TRACE("recv:%s", buf);

            task_buf = new(std::nothrow)sip_buffer;
            if (task_buf == NULL)
            {
                SIP_SVR_ERROR("malloc memory faild!");
                _memory_pool.free_memory(buf);
                continue;
            }

            task_buf->_buf = buf;
            task_buf->_buf_len = count_of_bytes;
            memcpy(&task_buf->_client_addr, &client_addr, sizeof(HPR_ADDR_T));
            task_buf->_user_data = this;

            //收到的信令，放入任务列表中
            work_data_t w_data;
            w_data.work_info = task_buf;
            w_data.work_route = work_routine;
            w_data.clear_route = clear_routine;
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
            //SIP_SVR_WARN("Dummy SIP message recved.");
        }
    }
}

HPR_VOIDPTR CALLBACK service_sip_t::work_routine(HPR_VOIDPTR thread_param)
{
    sip_buffer* buf = static_cast<sip_buffer*>(thread_param);
    service_sip_t* svc = static_cast<service_sip_t*>(buf->_user_data);
    char* buf_after_proc = NULL;
    HPR_INT32 buf_len_after_proc = 0;

    do
    {
        HPR_INT32 rt = svc->handle_incoming_message(buf->_buf, buf->_buf_len, &buf_after_proc, &buf_len_after_proc, buf->_client_addr);
        if (HPR_ERROR == rt)
        {
            SIP_SVR_ERROR("handle_incoming_message failed.");
            //释放资源
            svc->free_memory_from_out(buf->_buf);
            delete buf;
            break;
        }
        else if (0 < rt)
        {
            SIP_SVR_INFO("send to all center manager.");

            addr_book_mgr_t::instance()->lock_address_book_r();
            addr_book_mgr_t::manage_center_list mc_list = addr_book_mgr_t::instance()->get_center_list();
            addr_book_mgr_t::manage_center_list::iterator it = mc_list.begin();
            char* buf_to_send = NULL;
            sip_buffer* buf_tmp = NULL;
            for (; it!=mc_list.end();it++)
            {
                if ( (HPR_TRUE == (*it)->is_online_status() ) && ( rt == ( (*it)->get_phone_number()[0]) - '0') )
                {
                    buf_tmp = new sip_buffer;
                    memcpy(buf_tmp, buf, sizeof(sip_buffer));
                    buf_to_send = (char*)osip_malloc(buf_len_after_proc + 1);
                    memcpy(buf_to_send, buf_after_proc, buf_len_after_proc);
                    buf_to_send[buf_len_after_proc] = '\0';

                    std::string dev_ip;
                    (*it)->get_ip(dev_ip);
                    buf_tmp->_buf = buf_to_send;
                    buf_tmp->_buf_len = buf_len_after_proc;
                    HPR_MakeAddrByString(AF_INET, dev_ip.c_str(), (*it)->get_port(), &(buf_tmp->_client_addr) );
                    svc->send_data(buf_tmp);
                }
                else
                {
                    SIP_SVR_ERROR("manager center offline, index:%s", (*it)->get_phone_number());
                }
            }
            addr_book_mgr_t::instance()->unlock_address_book_r();

            //释放资源
            svc->free_memory_from_out(buf->_buf);
            osip_free(buf_after_proc);
            delete buf;
            break;
        }
        else
        {
            //释放资源，该资源已无用
            svc->free_memory_from_out(buf->_buf);

            buf->_buf = buf_after_proc;
            buf->_buf_len = buf_len_after_proc;
            svc->send_data(buf);
        }

    } while (0);

    return NULL;
}

HPR_INT32 service_sip_t::handle_incoming_message( char* buf, HPR_INT32 buf_len, char** buf_after_handle, HPR_INT32* buf_len_after_handle, HPR_ADDR_T& client_addr )
{
    HPR_INT32 rt = HPR_ERROR;
    osip_message_t* message = NULL;
    osip_message_t* answer = NULL;

    do
    {
        if (0 != osip_message_init(&message) )
        {
            SIP_SVR_ERROR("osip_message_init failed.");
            break;
        }

        if (0 != osip_message_parse(message, buf, buf_len))
        {
            SIP_SVR_ERROR("could not parse message.");
            break;
        }

        if (MSG_IS_REQUEST(message))
        {
            if (message->sip_method == NULL || message->req_uri == NULL)
            {
                SIP_SVR_ERROR("syntax err.");
                break;
            }
        }
        else
        {
            if (message->status_code < 100 || message->status_code > 699)
            {
                SIP_SVR_ERROR("syntax err.");
                break;
            }
        }

        //修正请求来源的via头，保证设备和服务器在不同网域时返回信息能够成功发送
        char dest[128] = {0};
        osip_message_fix_last_via_header(message, HPR_InetNtop(AF_INET, (unsigned char*)&(client_addr.SA.sin4.sin_addr), dest, 128), HPR_GetAddrPort(&client_addr));

        rt = proccess_message(message, &answer, client_addr);
        if (HPR_ERROR == rt) //此处复用client_addr作为发送地址
        {
            SIP_SVR_ERROR("proccess message failed.");
            break;
        }

        //将message结构体转化为字符串
        size_t to_str_len;
        if (answer != NULL)
        {
            if (0 != osip_message_to_str(answer, buf_after_handle, &to_str_len) )
            {
                SIP_SVR_ERROR("answer osip_message_to_str failed.");
                rt = HPR_ERROR;
                break;
            }
        }
        else
        {
            if (0 != osip_message_to_str(message, buf_after_handle, &to_str_len) )
            {
                SIP_SVR_ERROR("request osip_message_to_str failed.");
                rt = HPR_ERROR;
                break;
            }
        }


        //覆盖原来的buf
        *buf_len_after_handle = to_str_len;

    } while (0);

    if (message != NULL)
    {
        osip_message_free(message);
        message = NULL;
    }
    if (answer != NULL)
    {
        osip_message_free(answer);
        answer = NULL;
    }

    return rt;
}

HPR_INT32 service_sip_t::proccess_message( osip_message_t* message, osip_message_t** answer, HPR_ADDR_T& dest_addr )
{
    HPR_INT32 rt = HPR_ERROR;

    do
    {
        if (MSG_IS_REQUEST(message))
        {
            if (MSG_IS_MESSAGE(message))
            {
                rt = handle_message(message, answer, dest_addr);
            }
            else if (MSG_IS_INVITE(message))
            {
                SIP_SVR_INFO("recv invite, call-id:%s", message->call_id->number);
                rt = handle_invite(message, answer, dest_addr);
            }
            else if (MSG_IS_ACK(message))
            {
                SIP_SVR_INFO("recv ack, call-id:%s", message->call_id->number);
                rt = handle_ack(message, dest_addr);
            }
            else if (MSG_IS_BYE(message))
            {
                SIP_SVR_INFO("recv bye, call-id:%s", message->call_id->number);
                rt = handle_bye(message, answer, dest_addr);
            }
            else if (MSG_IS_CANCEL(message))
            {
                SIP_SVR_INFO("recv cancel, call-id:%s", message->call_id->number);
                rt = handle_bye(message, answer, dest_addr);
            }
            else if (MSG_IS_REGISTER(message))
            {
                rt = handle_register(message, answer, dest_addr);
            }
        }
        else
        {
            rt = handle_response(message, dest_addr);
        }

    } while (0);

    return rt;
}

HPR_INT32 service_sip_t::handle_register( osip_message_t* message, osip_message_t** answer, HPR_ADDR_T& client_addr )
{
    HPR_INT32 rt = HPR_OK;

    device_t* device = NULL;
    HPR_INT32 status = UAS_STATUS_200;

    HPR_INT32 expire = 0;
    char* str_expire = NULL;
    osip_header_t* head = NULL;
    osip_authorization_t* auth = NULL;

	char* phone_num = message->from->url->username;
	SIP_SVR_INFO("handle_register...");
    addr_book_mgr_t::instance()->lock_address_book_r();
    do
    {
        device = addr_book_mgr_t::instance()->get_address_book_for_register(phone_num);
        if (NULL == device)
        {
            SIP_SVR_ERROR("can not find phone number[%s] in address book.", phone_num);
            status = UAS_STATUS_404;
        }
		else
        {
            if (osip_message_get_expires(message, 0, &head) >= 0)
            {
                str_expire = head->hvalue;
            }
            if (str_expire != NULL)
            {
                expire = HPR_Atoi32(str_expire);
            }
            if (config_t::instance()->get_auth_status() == HPR_TRUE)
            {
                status = UAS_STATUS_401;
                char* realm = (char *)osip_malloc(config_t::instance()->get_realm().length() + 3);
                if (realm != NULL)
                {
                    sprintf(realm, "\"%s\"", config_t::instance()->get_realm().c_str());

                    HPR_INT32 pos = 0;
                    osip_message_get_authorization(message, 0, &auth);
                    while (auth != NULL)
                    {
                        if (HPR_Strcmp(auth->realm, realm) == 0)
                        {
                            if (validate_credential(auth) == HPR_OK)
                            {
                                osip_list_remove(&(message->authorizations), pos); //移除验证头
                                osip_authorization_free(auth);
                                auth = NULL;

                                status = UAS_STATUS_200;
                                break;
                            }
                        }
                        pos++;
                        osip_message_get_authorization(message, pos, &auth);
                    }
                    osip_free(realm);
                    realm = NULL;
                }
            }

            if (UAS_STATUS_200 == status)
            {
                if (expire <= 0)
                {
                    device->on_unregister();
                }
                else
                {
                    char dest[128] = {0};
                    HPR_TIME_T t = (HPR_TimeNow()/HPR_US_PER_SEC) + expire;
                    device->on_register(HPR_InetNtop(AF_INET, (unsigned char*)&(client_addr.SA.sin4.sin_addr), dest, 128), HPR_GetAddrPort(&client_addr), t);
                }
            }
        }

        if (HPR_OK == build_response(answer, status, message) )
        {
            if (status == UAS_STATUS_200)
            {
                HPR_TIME_T hpr_time = HPR_TimeNow();
                time_t timet = HPR_AnsiTimeFromTime(hpr_time);
                std::string str_time = "";
                time_timet2sip(timet, str_time);
                osip_message_set_date(*answer, str_time.c_str());
            }
            else if (status == UAS_STATUS_401)
            {
                if (device != NULL)
                {
                    add_credential(message, config_t::instance()->get_realm().c_str(), *answer, device->get_net_zone_id());
                }
                osip_message_set_expires(*answer, str_expire);
            }
        }
        else
        {
            SIP_SVR_ERROR("build_response failed.");
            break;
        }

    } while (0);
    addr_book_mgr_t::instance()->unlock_address_book_r();

    return rt;
}

HPR_INT32 service_sip_t::build_response( osip_message_t** rsp, HPR_INT32 status, osip_message_t* req )
{
    HPR_INT32 rt = HPR_ERROR;
    osip_generic_param_t *tag;
    osip_message_t *response;
    int pos;
    int i;

    do
    {
        *rsp = NULL;
        if (req == NULL)
        {
            SIP_SVR_ERROR("request is null.");
            break;
        }

        i = osip_message_init (&response);
        if (i != 0)
        {
            SIP_SVR_ERROR("osip_message_init failed.");
            break;
        }
        /* initialise osip_message_t structure */
        /* yet done... */

        response->sip_version = (char *) osip_malloc (8 * sizeof (char));
        if (response->sip_version == NULL)
        {
            SIP_SVR_ERROR("no memory.");
            osip_message_free (response);
            break;
        }
        sprintf (response->sip_version, "SIP/2.0");
        osip_message_set_status_code (response, status);

        /* handle some internal reason definitions. */
        if (MSG_IS_NOTIFY (req) && status == 481)
        {
            response->reason_phrase = osip_strdup ("Subscription Does Not Exist");
        }
        else if (MSG_IS_SUBSCRIBE (req) && status == 202)
        {
            response->reason_phrase = osip_strdup ("Accepted subscription");
        }
        else
        {
            response->reason_phrase = osip_strdup (osip_message_get_reason (status));
            if (response->reason_phrase == NULL)
            {
                if (response->status_code == 101)
                {
                    response->reason_phrase = osip_strdup ("Dialog Establishement");
                }
                else
                {
                    response->reason_phrase = osip_strdup ("Unknown code");
                }
            }
            response->req_uri = NULL;
            response->sip_method = NULL;
        }

        if (response->reason_phrase == NULL)
        {
            SIP_SVR_ERROR("no memory.");
            osip_message_free (response);
            break;
        }

        i = osip_to_clone (req->to, &(response->to));
        if (i != 0)
        {
            SIP_SVR_ERROR("osip_to_clone fail.");
            osip_message_free (response);
            break;
        }

        i = osip_to_get_tag (response->to, &tag);
        if (i != 0)       //可能有问题
        {                 /* we only add a tag if it does not already contains one! */
            if (status != 100)
            {
                char *tmp = (char *) osip_malloc (33);
                unsigned int number = osip_build_random_number ();

                if (tmp == NULL)
                    return NULL;

                sprintf (tmp, "%u", number);
                osip_to_set_tag (response->to, tmp);
            }
        }

        i = osip_from_clone (req->from, &(response->from));
        if (i != 0)
        {
            SIP_SVR_ERROR("osip_from_clone failed.");
            osip_message_free (response);
            break;
        }

        pos = 0;
        while (!osip_list_eol (&req->vias, pos)) {
            osip_via_t *via;
            osip_via_t *via2;

            via = (osip_via_t *) osip_list_get (&req->vias, pos);
            i = osip_via_clone (via, &via2);
            if (i != 0)
            {
                osip_message_free (response);
                return HPR_ERROR;
            }
            osip_list_add (&response->vias, via2, -1);
            pos++;
        }

        i = osip_call_id_clone (req->call_id, &(response->call_id));
        if (i != 0)
        {
            SIP_SVR_ERROR("osip_call_id_clone failed.");
            osip_message_free (response);
            break;
        }
        i = osip_cseq_clone (req->cseq, &(response->cseq));
        if (i != 0)
        {
            SIP_SVR_ERROR("osip_cseq_clone failed.");
            osip_message_free (response);
            break;
        }

        if (MSG_IS_SUBSCRIBE (req))
        {
            osip_header_t *exp;
            osip_header_t *evt_hdr;

            osip_message_header_get_byname (req, "event", 0, &evt_hdr);
            if (evt_hdr != NULL && evt_hdr->hvalue != NULL)
            {
                osip_message_set_header (response, "Event", evt_hdr->hvalue);
            }
            else
            {
                osip_message_set_header (response, "Event", "presence");
            }
            i = osip_message_get_expires (req, 0, &exp);
            if (exp == NULL)
            {
                osip_header_t *cp;

                i = osip_header_clone (exp, &cp);
                if (cp != NULL)
                {
                    osip_list_add (&response->headers, cp, 0);
                }
            }
        }

        osip_message_set_user_agent (response, config_t::instance()->get_user_agent() );

        *rsp = response;

        rt = HPR_OK;
    } while (0);

    return rt;
}

HPR_INT32 service_sip_t::validate_credential( osip_proxy_authorization_t* auth )
{
    HPR_INT32 rt = HPR_ERROR;
    char* rsp = osip_proxy_authorization_get_response(auth);
    char* nonce = osip_proxy_authorization_get_nonce(auth);
    char* opaque = osip_proxy_authorization_get_opaque(auth);
    char* realm = osip_proxy_authorization_get_realm(auth);
    char* uri = osip_proxy_authorization_get_uri(auth);
    if (opaque != NULL && nonce != NULL && realm != NULL && rsp != NULL && uri != NULL)
    {
        char *cnonce = NULL;
        char *sz_realm = osip_strdup_without_quote(realm);
        char *sz_nonce = osip_strdup_without_quote(nonce);
        char *sz_uri = osip_strdup_without_quote(uri);
        char *sz_rsp = osip_strdup_without_quote(rsp);

        HASHHEX ha1;
        HASHHEX ha2 = "";
        HASHHEX response;
        ppl_md5_DigestCalcHA1((char *)"MD5", (char*)config_t::instance()->get_user_name().c_str(), sz_realm, (char*)config_t::instance()->get_password().c_str(), sz_nonce, cnonce, ha1);
        ppl_md5_DigestCalcResponse(ha1, sz_nonce, NULL, cnonce, NULL, (char *)"REGISTER", sz_uri, ha2, response);

        if (HPR_Strcmp(sz_rsp, response) == 0)
        {
            rt = HPR_OK;
        }
        osip_free(sz_realm);
        osip_free(sz_nonce);
        osip_free(sz_uri);
        osip_free(sz_rsp);
    }

    return rt;
}

HPR_INT32 service_sip_t::add_credential( osip_message_t* message, const char* realm, osip_message_t* rsp, HPR_INT32 netzone_id )
{
    MD5_CTX md5_ctx;
    HASH hash_tmp;
    HASHHEX hash_tmp_hex;
    char tmp[64] = {0};

    ppl_MD5Init(&md5_ctx);
    memset(tmp, 0, 64);
    HPR_Snprintf(tmp, 63, "%s", config_t::instance()->get_local_ip(netzone_id) );
    ppl_MD5Update(&md5_ctx, (unsigned char *)tmp, strlen(tmp));

    memset(tmp, 0, 64);
    HPR_Snprintf(tmp, 63, "%d", config_t::instance()->get_sip_port());
    ppl_MD5Update(&md5_ctx, (unsigned char *)tmp, strlen(tmp));

    memset(tmp, 0, 64);
    HPR_Snprintf(tmp, 63, "%u", osip_build_random_number());
    ppl_MD5Update(&md5_ctx, (unsigned char *)tmp, strlen(tmp));

    HASH ha1;
    memset(tmp, 0, 64);
    ppl_MD5Final((unsigned char *)ha1, &md5_ctx);
    ppl_md5_hash_osip_to_hex(ha1, tmp);
    ppl_MD5Update(&md5_ctx, (unsigned char *)tmp, strlen(tmp));
    ppl_MD5Update(&md5_ctx, (unsigned char *)":", 1);
    ppl_MD5Update(&md5_ctx, (unsigned char *)message->cseq->number, strlen(message->cseq->number));
    ppl_MD5Update(&md5_ctx, (unsigned char *)":", 1);
    ppl_MD5Update(&md5_ctx, (unsigned char *)message->call_id->number, strlen(message->call_id->number));
    ppl_MD5Final((unsigned char *)hash_tmp, &md5_ctx);
    ppl_md5_hash_osip_to_hex(hash_tmp, hash_tmp_hex);

    char* nonce = (char*)osip_malloc(HASHHEXLEN + 1 + 2);                       /* +2 for the quotes */
    if (nonce != NULL)
    {
        memset(nonce, 0, HASHHEXLEN + 1 + 2);
        HPR_Snprintf(nonce, HASHHEXLEN + 2, "\"%s\"", hash_tmp_hex);
    }

    ppl_MD5Init(&md5_ctx);
    memset(tmp, 0, 64);
    HPR_Snprintf(tmp, 63, "%i", (HPR_INT32)time(NULL));
    ppl_MD5Update(&md5_ctx, (unsigned char *)tmp, strlen(tmp));
    ppl_MD5Update(&md5_ctx, (unsigned char *)":", 1);
    ppl_MD5Final((unsigned char *)hash_tmp, &md5_ctx);
    ppl_md5_hash_osip_to_hex(hash_tmp, hash_tmp_hex);

    char* opaque = (char*)osip_malloc(HASHHEXLEN + 1 + 2);                     /* +2 is for the quotes */
    if (opaque != NULL)
    {
        memset(opaque, 0, HASHHEXLEN + 1 + 2);
        HPR_Snprintf(opaque, HASHHEXLEN + 2, "\"%s\"", hash_tmp_hex);
    }

    char* new_realm = (char*)osip_malloc(strlen(realm) + 1 + 2);
    if (new_realm != NULL)
    {
        memset(new_realm, 0, strlen(realm) + 1 + 2);
        HPR_Snprintf(new_realm, strlen(realm) + 2, "\"%s\"", realm);
    }

    osip_www_authenticate_t* w_auth = NULL;
    if (osip_www_authenticate_init(&w_auth) == 0)
    {
        osip_www_authenticate_set_auth_type(w_auth, osip_strdup("Digest"));
        osip_www_authenticate_set_nonce(w_auth, nonce);
        osip_www_authenticate_set_opaque(w_auth, opaque);
        osip_www_authenticate_set_realm(w_auth, new_realm);
        osip_www_authenticate_set_algorithm_MD5(w_auth);
        osip_list_add(&(rsp->www_authenticates), w_auth, -1);
        return HPR_OK;
    }

	osip_free(nonce);
	osip_free(opaque);
	osip_free(new_realm);
    return HPR_ERROR;
}

HPR_INT32 service_sip_t::handle_invite( osip_message_t* message, osip_message_t** answer, HPR_ADDR_T& dest_addr )
{
    HPR_INT32 rt = HPR_OK;
    device_t* device = NULL;
    device_t* device_from = NULL;
    std::string dev_ip;
    HPR_ADDR_T dev_addr;
    HPR_INT32 status = UAS_STATUS_200;
	
	SIP_SVR_INFO("handle_invite...");
    addr_book_mgr_t::instance()->lock_address_book_r();
    do
    {
        if ((NULL == message->to->url->username) || (NULL == message->from->url->username))
        {
            SIP_SVR_ERROR("from or to username is null.");
            status = UAS_STATUS_404;
            break;
        }

        device = addr_book_mgr_t::instance()->get_address_book_for_call(message->to->url->username, status, message->call_id->number, message->from->url->username);
        device_from = addr_book_mgr_t::instance()->get_address_book(message->from->url->username);
        SIP_SVR_INFO("device_from:%s --> device_to:%s", message->from->url->username, message->to->url->username);

		if (NULL == device)
        {
            SIP_SVR_ERROR("can not find device or manage device is busy, to device:%s", message->to->url->username);
            break;
        }
        else if (NULL == device_from)
        {
            SIP_SVR_ERROR("can not find device, from device:%s", message->from->url->username);
            device->set_idle_status(); //防止失败后被叫管理机一直都为忙线状态
            status = UAS_STATUS_404;
            break;
        }

		if (HPR_ERROR == addr_book_mgr_t::instance()->redirect_addr(device))
		{
			SIP_SVR_ERROR("redirect device %s fail", device->get_phone_number());
			device->set_idle_status();
            status = UAS_STATUS_404;
            break;
		}
		
		if (HPR_ERROR == addr_book_mgr_t::instance()->redirect_addr(device_from))
		{
			SIP_SVR_ERROR("redirect device_from %s fail", device_from->get_phone_number());
			device->set_idle_status();
            status = UAS_STATUS_404;
            break;
		}

        if (HPR_ERROR == call_manager_t::instance()->init_call(message->call_id->number, device_from, device, status))
        {
            device->set_idle_status();
			SIP_SVR_ERROR("init_call fail!");
            break;
        }
        //添加via字段
        osip_via_t* via = NULL;
        osip_message_get_via(message, 0, &via);
        osip_via_t* via_to_add = NULL;
        osip_via_clone(via, &via_to_add); //克隆原via字段，主要为了branch字段保持一致。
        char* via_ip = osip_via_get_host(via_to_add);
        osip_free(via_ip);
        via_ip = osip_strdup(config_t::instance()->get_local_ip(device->get_net_zone_id()));
        if (0 == strlen(via_ip))
        {
            osip_free(via_ip);
            osip_via_set_host(via_to_add, NULL);
            osip_via_free(via_to_add);
            status = UAS_STATUS_400;
            call_manager_t::instance()->set_call_faild_status(message->call_id->number);
            SIP_SVR_ERROR("can not find local ip, netzone id:%d", device->get_net_zone_id());
            break;
        }
        osip_via_set_host(via_to_add, via_ip);
        char* via_port = osip_via_get_port(via_to_add);
        osip_free(via_port);
        char tmp_via_port[8];
        HPR_Itoa(tmp_via_port, config_t::instance()->get_sip_port(), 10);
        via_port = osip_strdup(tmp_via_port);
        osip_via_set_port(via_to_add, via_port);
        osip_generic_param_t *param = NULL;
        osip_generic_param_get_byname(&(via_to_add->via_params),(char *)"rport",&param);
        osip_free(param->gname);
        osip_free(param->gvalue);
        osip_generic_param_set (param, osip_strdup("rport"), osip_strdup (tmp_via_port));
        osip_list_add(&(message->vias), via_to_add, 0);

        //添加record-route字段
        char route_str[200];
        memset(route_str, 0, 200);
        HPR_Snprintf(route_str, 199,  "<sip:%s:%d;lr>", config_t::instance()->get_local_ip(device->get_net_zone_id()), \
            config_t::instance()->get_sip_port());
        osip_message_set_record_route(message, route_str);

        //Max-Forwards减1
        osip_header_t* max_forwards = NULL;
        osip_message_get_max_forwards (message, 0, &max_forwards);
        HPR_INT32 mf = HPR_Atoi32(max_forwards->hvalue);
        mf--;
        HPR_Itoa(max_forwards->hvalue, mf, 10);

        //设置req-uri
        char* lhost = osip_uri_get_host(message->req_uri); //获取请求的req_uri地址后设置新的消息发送地址
        osip_free(lhost); //释放原来的资源，避免内存泄漏
        device->get_ip(dev_ip);
        lhost = osip_strdup(dev_ip.c_str());
        osip_uri_set_host(message->req_uri, lhost);

        char* lport = osip_uri_get_port(message->req_uri); //获取请求的req_uri端口后设置新的消息发送端口
        osip_free(lport);
        char tmp_port[8];
        HPR_Itoa(tmp_port, device->get_port(), 10);
        lport = osip_strdup(tmp_port);
        osip_uri_set_port(message->req_uri, lport);

        HPR_MakeAddrByString(AF_INET, dev_ip.c_str(), device->get_port(), &dev_addr);
        memcpy(&dest_addr, &dev_addr, sizeof(dest_addr));

        //如果是管理机，则设置To字段，使得后续的交互直接可找到管理机的号码
        osip_uri_t* to_uri = NULL;
		const char* to_uri_username = NULL;
        to_uri = osip_to_get_url(message->to);
        osip_free(to_uri->username);
		to_uri_username = device->is_manager_center() ? device->get_phone_number() : device->get_call_number();
        osip_uri_set_username(to_uri, osip_strdup(to_uri_username));

    } while (0);
    addr_book_mgr_t::instance()->unlock_address_book_r();

    if (status != UAS_STATUS_200)
    {
        SIP_SVR_ERROR("bad request, status:%d.", status);
        if (HPR_OK != build_response(answer, status, message))
        {
            SIP_SVR_ERROR("build_response failed.");
            rt = HPR_ERROR;
        }
    }


    return rt;
}

HPR_INT32 service_sip_t::handle_ack( osip_message_t* message, HPR_ADDR_T& dest_addr )
{
    HPR_INT32 rt = HPR_ERROR;
    device_t* device = NULL;
    std::string dev_ip;

    addr_book_mgr_t::instance()->lock_address_book_r();
    do
    {
    	SIP_SVR_INFO("handle_ack...");
        if (message->to->url->username == NULL || message->from->url->username == NULL)
        {
            SIP_SVR_ERROR("from or to username is null.");
            break;
        }

        device = addr_book_mgr_t::instance()->get_address_book(message->to->url->username);
        if (device != NULL && HPR_TRUE == device->is_online_status())
        {
            char* lhost = osip_uri_get_host(message->req_uri); //获取请求的req_uri地址后设置新的消息发送地址
            osip_free(lhost); //释放原来的资源，避免内存泄漏
            device->get_ip(dev_ip);
            lhost = osip_strdup(dev_ip.c_str());

            osip_uri_set_host(message->req_uri, lhost);

            char* lport = osip_uri_get_port(message->req_uri); //获取请求的req_uri端口后设置新的消息发送端口
            osip_free(lport);
            char tmp_port[8];
            HPR_Itoa(tmp_port, device->get_port(), 10);
            lport = osip_strdup(tmp_port);
            osip_uri_set_port(message->req_uri, lport);
        }
        else if (device != NULL && HPR_FALSE == device->is_online_status() )
        {
            SIP_SVR_ERROR("device not online, indexcode:%s", message->to->url->username);
            break;
        }

        osip_route_t* route = NULL;
        if (-1 != osip_message_get_route(message, 0, &route) )
        {
            //暂时不比较route中ip和端口是否是本机主机地址
            osip_list_remove(&message->routes, 0);
            osip_route_free(route);
        }

        if (device == NULL)
        {
            if (-1 != osip_message_get_route(message, 0, &route) )
            {
                //Max-Forwards减1
                osip_header_t* max_forwards = NULL;
                osip_message_get_max_forwards (message, 0, &max_forwards);
                HPR_INT32 mf = HPR_Atoi32(max_forwards->hvalue);
                mf--;
                HPR_Itoa(max_forwards->hvalue, mf, 10);
                HPR_MakeAddrByString(AF_INET, route->url->host, HPR_Atoi32(route->url->port), &dest_addr);
            }
            else
            {
                SIP_SVR_ERROR("can not find device, and there is no route info.");
                break;
            }
        }
        else
        {
            HPR_MakeAddrByString(AF_INET, dev_ip.c_str(), device->get_port(), &dest_addr);
        }

        rt = HPR_OK;
    } while (0);
    addr_book_mgr_t::instance()->unlock_address_book_r();

    return rt;
}

HPR_INT32 service_sip_t::handle_response( osip_message_t* message, HPR_ADDR_T& dest_addr )
{
    HPR_INT32 rt = HPR_ERROR;
    char* dest_ip = NULL;
    HPR_INT32 dest_port = 0;

    do
    {
		SIP_SVR_INFO("handle_response...");
        if (message->call_id->number == NULL)
        {
            SIP_SVR_ERROR("call-id number is null.");
            break;
        }

        if (MSG_IS_RESPONSE_FOR(message, "INVITE") )
        {
            if (message->status_code == UAS_STATUS_488 || message->status_code == UAS_STATUS_480)
            {
                SIP_SVR_INFO("recv %d rsp, call-id:%s", message->status_code, message->call_id->number);
                call_manager_t::instance()->set_call_timeout_status(message->call_id->number);
            }
            else if (message->status_code == UAS_STATUS_486)
            {
                SIP_SVR_INFO("recv %d rsp, call-id:%s", message->status_code, message->call_id->number);
                call_manager_t::instance()->set_busy_status(message->call_id->number);
            }
            else if (message->status_code == UAS_STATUS_406 || message->status_code == UAS_STATUS_603)
            {
                SIP_SVR_INFO("recv %d rsp, call-id:%s", message->status_code, message->call_id->number);
                call_manager_t::instance()->set_no_pick_up_status(message->call_id->number);
            }
            else if (message->status_code >= UAS_STATUS_300)
            {
                SIP_SVR_INFO("recv %d rsp, call-id:%s", message->status_code, message->call_id->number);
                call_manager_t::instance()->set_call_faild_status(message->call_id->number);
            }
            else if (message->status_code == UAS_STATUS_200)
            {
                call_manager_t::instance()->set_rcd_start_time_now(message->call_id->number);
            }
            else if (message->status_code == UAS_STATUS_100)
            {
                SIP_SVR_INFO("recv %d rsp, call-id:%s", message->status_code, message->call_id->number);
                call_manager_t::instance()->set_trying_time_now(message->call_id->number);
            }
        }

        osip_via_t* via_to_del = (osip_via_t*) osip_list_get(&(message->vias), 0);
        if (0 > osip_list_remove(&(message->vias), 0))
        {
            SIP_SVR_ERROR("there is no vias in the rsp.");
            break;
        } //先移除本机的via字段
        osip_via_free(via_to_del);

        osip_via_t* via = static_cast<osip_via_t*>(osip_list_get(&(message->vias), 0) );
        if (via == NULL)
        {
            SIP_SVR_ERROR("there is no enough via in the rsp.");
            break;
        }

        dest_ip = osip_via_get_host(via);
        char* str_dest_port = osip_via_get_port(via);
        if (dest_ip == NULL || str_dest_port == NULL)
        {
            SIP_SVR_ERROR("dest ip or port is illegel.");
            break;
        }
        dest_port = HPR_Atoi32(str_dest_port);

        HPR_MakeAddrByString(AF_INET, dest_ip, static_cast<HPR_UINT16>(dest_port), &dest_addr);
		SIP_SVR_INFO("dest ip: %s, dest port: %d.\n",dest_ip,dest_port);

        rt = HPR_OK;

        if (MSG_IS_RESPONSE_FOR(message, "INVITE"))
        {
            SIP_SVR_INFO("recv %d rsp, call-id:%s", message->status_code, message->call_id->number);
        }

    } while (0);

    return rt;
}

HPR_INT32 service_sip_t::handle_message( osip_message_t* message, osip_message_t** answer, HPR_ADDR_T& dest_addr )
{
    HPR_INT32 rt = HPR_OK;
    device_t* device = NULL;
    osip_body_t* body = NULL;
    HPR_INT32 status = UAS_STATUS_400;
    HPR_INT32 body_type = -1;
    TiXmlDocument doc;
    std::string dev_ip;
    HPR_ADDR_T dev_addr;

	SIP_SVR_INFO("handle_message...");
    addr_book_mgr_t::instance()->lock_address_book_r();
    do
    {
        if (message->to->url->username == NULL || message->from->url->username == NULL)
        {
            SIP_SVR_ERROR("from or to username is null.");
            break;
        }

        if (-1 == osip_message_get_body(message, 0, &body) ) //获取body
        {
            SIP_SVR_ERROR("osip_message_get_body failed.");
            break;
        }

        doc.Parse(body->body);
        if (doc.Error())
        {
            SIP_SVR_ERROR("body parse err.");
            break;
        }

        TiXmlElement* root = doc.RootElement();
        if (root == NULL)
        {
            SIP_SVR_ERROR("get root element failed.");
            break;
        }

        TiXmlElement* head = root->FirstChildElement(MSG_HEADER);
        if (head == NULL)
        {
            SIP_SVR_ERROR("get head failed.");
            break;
        }

        TiXmlElement* method = head->FirstChildElement(MSG_METHOD);
        if (method == NULL)
        {
            SIP_SVR_ERROR("get method failed.");
            break;
        }

        const char* str_type = method->GetText();
        if (str_type == NULL)
        {
            SIP_SVR_ERROR("method->GetText() failed.");
            break;
        }

        body_type = HPR_Atoi32(str_type);

        if (body_type == MSG_HEART_BEAT)
        {
            device = addr_book_mgr_t::instance()->get_address_book(message->from->url->username);
            if (device == NULL || HPR_FALSE == device->is_online_status())
            {
                SIP_SVR_ERROR("can not find online device from address book, indexcode:%s", message->from->url->username);
                status = UAS_STATUS_404;
                break;
            }

            //device->on_heartbeat();

            status = UAS_STATUS_200;
        }
        else if (body_type == MSG_ALARM)
        {
            //添加via字段
            osip_via_t* via = NULL;
            osip_message_get_via(message, 0, &via);
            osip_via_t* via_to_add = NULL;
            osip_via_clone(via, &via_to_add); //克隆原via字段，主要为了branch字段保持一致。
            char* via_ip = osip_via_get_host(via_to_add);
            osip_free(via_ip);
            via_ip = osip_strdup("127.0.0.1");//写死via地址，在接收客户端中自动修正
            osip_via_set_host(via_to_add, via_ip);
            char* via_port = osip_via_get_port(via_to_add);
            osip_free(via_port);
            char tmp_via_port[8];
            HPR_Itoa(tmp_via_port, config_t::instance()->get_sip_port(), 10);
            via_port = osip_strdup(tmp_via_port);
            osip_via_set_port(via_to_add, via_port);
            osip_generic_param_t *param = NULL;
            osip_generic_param_get_byname(&(via_to_add->via_params),(char *)"rport",&param);
            osip_free(param->gname);
            osip_free(param->gvalue);
            osip_generic_param_set (param, osip_strdup("rport"), osip_strdup (tmp_via_port));
            osip_list_add(&(message->vias), via_to_add, 0);

            //Max-Forwards减1
            osip_header_t* max_forwards = NULL;
            osip_message_get_max_forwards (message, 0, &max_forwards);
            HPR_INT32 mf = HPR_Atoi32(max_forwards->hvalue);
            mf--;
            HPR_Itoa(max_forwards->hvalue, mf, 10);

            status = UAS_STATUS_200;
            if ( strcmp(config_t::instance()->get_mgr_phone_num(), message->from->url->username) == 0 )
            {
                rt = 10; //标识发送至所有管理中心
            }
            else
            {
                rt = message->from->url->username[0] - '0'; //标识发送至所在期的管理中心
            }
        }
        else
        {
            device = addr_book_mgr_t::instance()->get_address_book(message->to->url->username);
            if (device == NULL || HPR_FALSE == device->is_online_status())
            {
                SIP_SVR_ERROR("can not find online device from address book, indexcode:%s", message->to->url->username);
                status = UAS_STATUS_404;
                rt = HPR_ERROR;
                break;
            }

			if (HPR_ERROR == addr_book_mgr_t::instance()->redirect_addr(device))
			{
	            status = UAS_STATUS_404;
				rt = HPR_ERROR;
	            break;
			}

            //添加via字段
            osip_via_t* via = NULL;
            osip_message_get_via(message, 0, &via);
            osip_via_t* via_to_add = NULL;
            osip_via_clone(via, &via_to_add); //克隆原via字段，主要为了branch字段保持一致。
            char* via_ip = osip_via_get_host(via_to_add);
            osip_free(via_ip);
            via_ip = osip_strdup(config_t::instance()->get_local_ip(device->get_net_zone_id()));
            osip_via_set_host(via_to_add, via_ip);
            char* via_port = osip_via_get_port(via_to_add);
            osip_free(via_port);
            char tmp_via_port[8];
            HPR_Itoa(tmp_via_port, config_t::instance()->get_sip_port(), 10);
            via_port = osip_strdup(tmp_via_port);
            osip_via_set_port(via_to_add, via_port);
            osip_generic_param_t *param = NULL;
            osip_generic_param_get_byname(&(via_to_add->via_params),(char *)"rport",&param);
            osip_free(param->gname);
            osip_free(param->gvalue);
            osip_generic_param_set (param, osip_strdup("rport"), osip_strdup (tmp_via_port));
            osip_list_add(&(message->vias), via_to_add, 0);

            //message暂不加该字段，注释掉先
            ////添加record-route字段
            //char* route_str = (char*)osip_malloc(200);
            //_snprintf_s(route_str, 200, _TRUNCATE, "<sip:%s;lr>", config_t::instance()->get_local_ip(device->get_net_zone_id()));
            //osip_message_set_record_route(message, route_str);

            //Max-Forwards减1
            osip_header_t* max_forwards = NULL;
            osip_message_get_max_forwards (message, 0, &max_forwards);
            HPR_INT32 mf = HPR_Atoi32(max_forwards->hvalue);
            mf--;
            HPR_Itoa(max_forwards->hvalue, mf, 10);

            //设置req-uri
            char* lhost = osip_uri_get_host(message->req_uri); //获取请求的req_uri地址后设置新的消息发送地址
            osip_free(lhost); //释放原来的资源，避免内存泄漏
            device->get_ip(dev_ip);
            lhost = osip_strdup(dev_ip.c_str());
            osip_uri_set_host(message->req_uri, lhost);

            char* lport = osip_uri_get_port(message->req_uri); //获取请求的req_uri端口后设置新的消息发送端口
            osip_free(lport);
            char tmp_port[8];
            HPR_Itoa(tmp_port, device->get_port(), 10);
            lport = osip_strdup(tmp_port);
            osip_uri_set_port(message->req_uri, lport);

            HPR_MakeAddrByString(AF_INET, dev_ip.c_str(), device->get_port(), &dev_addr);
            memcpy(&dest_addr, &dev_addr, sizeof(dest_addr));

            status = UAS_STATUS_200;
        }

    } while (0);
    addr_book_mgr_t::instance()->unlock_address_book_r();

    if (body_type == MSG_HEART_BEAT || status != UAS_STATUS_200)
    {
        if (HPR_OK != build_response(answer, status, message))
        {
            SIP_SVR_ERROR("build_response failed.");
            rt = HPR_ERROR;
        }
    }
    return rt;
}

HPR_VOID service_sip_t::free_memory_from_out( HPR_VOIDPTR buf )
{
    _memory_pool.free_memory(buf);
}

HPR_INT32 service_sip_t::handle_bye( osip_message_t* message, osip_message_t** answer, HPR_ADDR_T& dest_addr )
{
    HPR_INT32 rt = HPR_OK;
    device_t* device = NULL;
    std::string dev_ip;
    HPR_INT32 status = UAS_STATUS_200;

    addr_book_mgr_t::instance()->lock_address_book_r();
    do
    {
      	SIP_SVR_INFO("handle_bye...");
        if (message->req_uri->username == NULL)
        {
            SIP_SVR_ERROR("request username is NULL.");
            status = UAS_STATUS_400;
            break;
        }

        if (MSG_IS_CANCEL(message))
        {
            std::string called_phone_num = message->to->url->username;
            if (HPR_ERROR == call_manager_t::instance()->get_called_phone_number(message->call_id->number, called_phone_num) )
            {
                SIP_SVR_ERROR("get_called_phone_number failed, callid:%s", message->call_id->number);
                status = UAS_STATUS_400;
                break;
            }
            device = addr_book_mgr_t::instance()->get_address_book(called_phone_num.c_str());
            SIP_SVR_INFO("recv cancel, call-id:%s", message->call_id->number);
            call_manager_t::instance()->set_cancel_status(message->call_id->number);
        }
        else if (MSG_IS_BYE(message))
        {
            SIP_SVR_INFO("recv bye, call-id:%s", message->call_id->number);
            device = addr_book_mgr_t::instance()->get_address_book(message->req_uri->username);//先通过call_id找到called设备号码，再清除上下文
            call_manager_t::instance()->set_hang_up_status(message->call_id->number);
        }
        else
        {
            SIP_SVR_INFO("recv other bye, call-id:%s", message->call_id->number);
        }


        if (device != NULL && HPR_TRUE == device->is_online_status())
        {
            //设置req-uri
            char* lhost = osip_uri_get_host(message->req_uri); //获取请求的req_uri地址后设置新的消息发送地址
            osip_free(lhost); //释放原来的资源，避免内存泄漏
            device->get_ip(dev_ip);
            lhost = osip_strdup(dev_ip.c_str());
            osip_uri_set_host(message->req_uri, lhost);

            char* lport = osip_uri_get_port(message->req_uri); //获取请求的req_uri端口后设置新的消息发送端口
            osip_free(lport);
            char tmp_port[8];
            HPR_Itoa(tmp_port, device->get_port(), 10);
            lport = osip_strdup(tmp_port);
            osip_uri_set_port(message->req_uri, lport);

        }
        else if (device != NULL && HPR_FALSE == device->is_online_status() )
        {
            SIP_SVR_ERROR("device offline, indexcode:%s", message->to->url->username);
            status = UAS_STATUS_404;
            break;
        }

        osip_route_t* route = NULL;
        if (-1 != osip_message_get_route(message, 0, &route) )
        {
            //暂时不比较route中ip和端口是否是本机主机地址
            //删除本机route
            osip_list_remove(&message->routes, 0);
            osip_route_free(route);
			route = NULL;
        }

        if (device != NULL)
        {
            //添加via字段
            osip_via_t* via = NULL;
            osip_message_get_via(message, 0, &via);
            osip_via_t* via_to_add = NULL;
            osip_via_clone(via, &via_to_add); //克隆原via字段，主要为了branch字段保持一致。
            char* via_ip = osip_via_get_host(via_to_add);
            osip_free(via_ip);
            via_ip = osip_strdup(config_t::instance()->get_local_ip(device->get_net_zone_id()));
            osip_via_set_host(via_to_add, via_ip);
            char* via_port = osip_via_get_port(via_to_add);
            osip_free(via_port);
            char tmp_via_port[8];
            HPR_Itoa(tmp_via_port, config_t::instance()->get_sip_port(), 10);
            via_port = osip_strdup(tmp_via_port);
            osip_via_set_port(via_to_add, via_port);
            osip_generic_param_t *param = NULL;
            osip_generic_param_get_byname(&(via_to_add->via_params),(char *)"rport",&param);
            osip_free(param->gname);
            osip_free(param->gvalue);
            osip_generic_param_set (param, osip_strdup("rport"), osip_strdup (tmp_via_port));
            osip_list_add(&(message->vias), via_to_add, 0);

            HPR_MakeAddrByString(AF_INET, dev_ip.c_str(), device->get_port(), &dest_addr);
        }
        else
        {		
	        if (-1 != osip_message_get_route(message, 0, &route) )
	        {
				if (route != NULL)
				{
				    //添加via字段
                    osip_via_t* via = NULL;
                    osip_message_get_via(message, 0, &via);
                    osip_via_t* via_to_add = NULL;
                    osip_via_clone(via, &via_to_add); //克隆原via字段，主要为了branch字段保持一致。
                    char* via_ip = osip_via_get_host(via_to_add);
                    osip_free(via_ip);
            
					via_ip = osip_strdup(route->url->host); //暂不考虑多重转发时，主机地址的网域不一致导致的问题，端口同此说明
					osip_via_set_host(via_to_add, via_ip);
					char* via_port = osip_via_get_port(via_to_add);
					osip_free(via_port);
					via_port = osip_strdup(route->url->port);
					osip_via_set_port(via_to_add, via_port);
					osip_generic_param_t *param = NULL;
					osip_generic_param_get_byname(&(via_to_add->via_params),(char *)"rport",&param);
					osip_free(param->gname);
					osip_free(param->gvalue);
					osip_generic_param_set (param, osip_strdup("rport"), osip_strdup (route->url->port));
					osip_list_add(&(message->vias), via_to_add, 0);
					HPR_MakeAddrByString(AF_INET, route->url->host, HPR_Atoi32(route->url->port), &dest_addr);
								
					osip_list_remove(&message->routes, 0);
					osip_route_free(route);
				}
				else
				{
					break;
				}
	        }
			else
			{
				break;
			}
        }

        //Max-Forwards减1
        osip_header_t* max_forwards = NULL;
        osip_message_get_max_forwards (message, 0, &max_forwards);
        HPR_INT32 mf = HPR_Atoi32(max_forwards->hvalue);
        mf--;
        HPR_Itoa(max_forwards->hvalue, mf, 10);

    } while (0);
    addr_book_mgr_t::instance()->unlock_address_book_r();

    if (status != UAS_STATUS_200)
    {
        SIP_SVR_ERROR("bad request.");
        if (HPR_OK != build_response(answer, status, message))
        {
            SIP_SVR_ERROR("build_response failed.");
            rt = HPR_ERROR;
        }
    }

    return rt;
}

HPR_VOIDPTR CALLBACK service_sip_t::clear_routine( HPR_VOIDPTR thread_param )
{
    sip_buffer* p = static_cast<sip_buffer*>(thread_param);
    service_sip_t* svc = static_cast<service_sip_t*>(p->_user_data);

    //释放资源
    svc->free_memory_from_out(p->_buf);
    delete p;
    return NULL;
}
