#ifndef NET_MANAGE_PARSER_ODM56BG
#define NET_MANAGE_PARSER_ODM56BG

#include<hpr/HPR_Hpr.h>

typedef struct NET_MANAGE_DEVICE_INFO 
{
    NET_MANAGE_DEVICE_INFO()
    {
        memset(device_index, 0, 64);
        device_status = 3;
    }
    char device_index[64];
    HPR_INT32 device_status;
}NET_MANAGE_DEVICE_INFO_T;

class net_manage_request_t
{
public:
    net_manage_request_t ();
    virtual ~net_manage_request_t ();

    HPR_INT32 parser(const char* msg);
    std::string builder();

    inline HPR_VOID set_op_type(const char* op_type)
    {
        _operator_type = op_type;
    }

    inline std::string get_op_type()
    {
        return _operator_type;
    }

    inline HPR_VOID set_offset(HPR_INT32 offset)
    {
        _offset = offset;
    }

    inline HPR_INT32 get_offset()
    {
        return _offset;
    }

    inline HPR_VOID set_number_to_get(HPR_INT32 number)
    {
        _number_to_get = number;
        _have_num_to_get = HPR_TRUE;
    }

    inline HPR_INT32 get_number_to_get()
    {
        return _number_to_get;
    }

    inline HPR_VOID set_device_list(HPR_INT32 number, NET_MANAGE_DEVICE_INFO_T* dev_list)
    {
        _device_number = number;
        _dev_status_list = dev_list;
        _have_dev_list = HPR_TRUE;
    }

    inline HPR_VOID get_device_list(HPR_INT32& number, NET_MANAGE_DEVICE_INFO_T** dev_list)
    {
        number = _device_number;
        *dev_list = _dev_status_list;
    }
private:
    std::string _operator_type;
    HPR_INT32 _offset;
    HPR_INT32 _number_to_get;
    HPR_BOOL _have_num_to_get;
    HPR_INT32 _device_number;
    HPR_BOOL _have_dev_list;
    NET_MANAGE_DEVICE_INFO_T* _dev_status_list;
} /* net_manage_request_t */;

class net_manage_response_t
{
public:
    net_manage_response_t ();
    virtual ~net_manage_response_t ();

    HPR_INT32 parser(const char* msg);
    std::string builder();

    inline HPR_VOID set_op_type(const char* op_type)
    {
        _operator_type = op_type;
    }

    inline std::string get_op_type()
    {
        return _operator_type;
    }

    inline HPR_VOID set_result(const char* result)
    {
        _result = result;
    }

    inline std::string get_result()
    {
        return _result;
    }

    inline HPR_VOID set_result_msg(const char* message)
    {
        _result_msg = message;
    }

    inline std::string get_result_msg()
    {
        return _result_msg;
    }

    inline HPR_VOID set_err_code(HPR_INT32 err_code)
    {
        _err_code = err_code;
    }

    inline HPR_INT32 get_err_code()
    {
        return _err_code;
    }

    inline HPR_VOID set_offset(HPR_INT32 offset)
    {
        _offset = offset;
    }

    inline HPR_INT32 get_offset()
    {
        return _offset;
    }

    inline HPR_VOID set_total_num(HPR_INT32 total_num)
    {
        _total_number = total_num;
    }

    inline HPR_INT32 get_total_num()
    {
        return _total_number;
    }

    inline HPR_VOID set_number_to_send(HPR_INT32 number)
    {
        _number_to_send = number;
        _have_num_to_send = HPR_TRUE;
    }

    inline HPR_INT32 get_number_to_get()
    {
        return _number_to_send;
    }

    inline HPR_VOID set_device_list(HPR_INT32 number, NET_MANAGE_DEVICE_INFO_T* dev_list)
    {
        _device_number = number;
        _dev_status_list = dev_list;
        _have_dev_list = HPR_TRUE;
    }

    inline HPR_VOID get_device_list(HPR_INT32& number, NET_MANAGE_DEVICE_INFO_T** dev_list)
    {
        number = _device_number;
        *dev_list = _dev_status_list;
    }

private:
    std::string _operator_type;
    std::string _result;
    std::string _result_msg;
    HPR_INT32 _err_code;
    HPR_INT32 _offset;
    HPR_INT32 _total_number;
    HPR_INT32 _number_to_send;
    HPR_BOOL _have_num_to_send;
    HPR_BOOL _have_dev_list;
    HPR_INT32 _device_number;
    NET_MANAGE_DEVICE_INFO_T* _dev_status_list;
} /* net_manage_response_t */;

#endif /* end of include guard: NET_MANAGE_PARSER_ODM56BG */
