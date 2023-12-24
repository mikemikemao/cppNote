#include<tinyxml/tinyxml.h>
#include<hpr/HPR_String.h>
#include<hpr/HPR_Utils.h>

#include"net_manage_parser.h"

net_manage_request_t::net_manage_request_t()
: _offset(0)
, _number_to_get(0)
, _have_num_to_get(HPR_FALSE)
, _device_number(0)
, _have_dev_list(HPR_FALSE)
, _dev_status_list(NULL)
{

}

net_manage_request_t::~net_manage_request_t()
{
    _dev_status_list = NULL;
}

HPR_INT32 net_manage_request_t::parser( const char* msg )
{
    TiXmlDocument doc;

    doc.Parse(msg);
    if (doc.Error())
    {
        return -1;
    }

    TiXmlElement* root = doc.RootElement();
    if (root == NULL)
    {
        return -1;
    }

    TiXmlElement* request = root->FirstChildElement("request");
    if (request == NULL)
    {
        return -1;
    }

    TiXmlElement* op = request->FirstChildElement("operator");
    if (op == NULL)
    {
        return -1;
    }

    const char* optype = op->GetText();
    if (optype == NULL)
    {
        return -1;
    }
    _operator_type = optype;

    TiXmlElement* num = request->FirstChildElement("getNum");
    if (num != NULL)
    {
        const char* str_num = num->GetText();
        if (str_num == NULL)
        {
            return -1;
        }
        _number_to_get = HPR_Atoi32(str_num);
        const std::string attr_num = num->Attribute("offset");
        if (attr_num.length() == 0)
        {
            return -1;
        }
        _offset = HPR_Atoi32(attr_num.c_str());

        _have_num_to_get = HPR_TRUE;
    }

    TiXmlElement* devices = request->FirstChildElement("devices");
    if (devices != NULL)
    {
        const std::string dev_num = devices->Attribute("devNum");
        if (dev_num.length() == 0)
        {
            return -1;
        }
        _device_number = HPR_Atoi32(dev_num.c_str());
        if (_device_number <= 0)
        {
            return -1;
        }

        _dev_status_list = new(std::nothrow) NET_MANAGE_DEVICE_INFO_T[_device_number];
        if (_dev_status_list == NULL)
        {
            return -1;
        }

        TiXmlElement* dev = devices->FirstChildElement("device");
        if (dev == NULL)
        {
            delete[] _dev_status_list;
            _dev_status_list = NULL;
            return -1;
        }
        const std::string index_code = dev->Attribute("devIndexCode");
        if (index_code.length() == 0)
        {
            delete[] _dev_status_list;
            _dev_status_list = NULL;
            return -1;
        }
        const std::string str_state = dev->Attribute("devState");
        if (str_state.length() == 0)
        {
            delete[] _dev_status_list;
            _dev_status_list = NULL;
            return -1;
        }
        HPR_Snprintf(_dev_status_list[0].device_index, 64, "%s", index_code.c_str() );
        _dev_status_list[0].device_status = HPR_Atoi32(str_state.c_str());
        HPR_INT32 device_num = 1;
        for (int i=1; i<_device_number; i++)
        {
            dev = dev->NextSiblingElement();
            if (dev == NULL)
            {
                break;
            }

            const std::string index_code1 = dev->Attribute("devIndexCode");
            if (index_code1.length() == 0)
            {
                delete[] _dev_status_list;
                _dev_status_list = NULL;
                return -1;
            }
            const std::string str_state1 = dev->Attribute("devState");
            if (str_state1.length() == 0)
            {
                delete[] _dev_status_list;
                _dev_status_list = NULL;
                return -1;
            }
            HPR_Snprintf(_dev_status_list[i].device_index, 64, "%s", index_code1.c_str() );
            _dev_status_list[i].device_status = HPR_Atoi32(str_state1.c_str());
            device_num++;
        }
        if (device_num != _device_number)
        {
            delete[] _dev_status_list;
            _dev_status_list = NULL;
            return -1;
        }

        delete[] _dev_status_list;
        _dev_status_list = NULL;

        _have_dev_list = HPR_TRUE;
    }

    return 0;
}

/*lint -save -e429*/
std::string net_manage_request_t::builder()
{
    TiXmlDocument doc;
    std::string out_string;

    TiXmlDeclaration declaration("1.0", "UTF-8", "yes");
    doc.InsertEndChild(declaration);

    TiXmlElement root("nmsBatchMsg");
    TiXmlElement request("request");

    TiXmlElement op("operator");
    TiXmlText* operator_text = new(std::nothrow)TiXmlText(_operator_type);
    if (operator_text == NULL)
    {
        return NULL;
    }
    op.LinkEndChild(operator_text);

    request.InsertEndChild(op);

    if (_have_num_to_get == HPR_TRUE)
    {
        TiXmlElement num_to_get("getNum");

        char str_num[8];
        memset(str_num, 0, 8);
        HPR_Itoa(str_num, _number_to_get, 10);
        TiXmlText* num = new(std::nothrow)TiXmlText(str_num);
        if (num == NULL)
        {
            return NULL;
        }
        num_to_get.LinkEndChild(num);

        num_to_get.SetAttribute("offset", _offset);

        request.InsertEndChild(num_to_get);
    }

    if (_have_dev_list == HPR_TRUE)
    {
        if (_dev_status_list == NULL)
        {
            return NULL;
        }

        TiXmlElement dev_list("devices");
        dev_list.SetAttribute("devNum", _device_number);

        for (int i=_offset; i<(_offset+_device_number); i++)
        {
            TiXmlElement device("device");
            device.SetAttribute("devIndexCode", _dev_status_list[i].device_index);
            device.SetAttribute("devState", _dev_status_list[i].device_status);
            dev_list.InsertEndChild(device);
        }

        request.InsertEndChild(dev_list);
    }

    root.InsertEndChild(request);
    doc.InsertEndChild(root);

    out_string << doc;

    return out_string;
}
/*lint -restore*/


net_manage_response_t::net_manage_response_t()
: _err_code(0)
, _offset(0)
, _total_number(0)
, _number_to_send(0)
, _have_num_to_send(HPR_FALSE)
, _have_dev_list(HPR_FALSE)
, _dev_status_list(NULL)
, _device_number(0)
{

}

net_manage_response_t::~net_manage_response_t()
{
    _dev_status_list = NULL;
}

HPR_INT32 net_manage_response_t::parser( const char* msg )
{
    TiXmlDocument doc;

    doc.Parse(msg);
    if (doc.Error())
    {
        return -1;
    }

    TiXmlElement* root = doc.RootElement();
    if (root == NULL)
    {
        return -1;
    }

    TiXmlElement* response = root->FirstChildElement("response");
    if (response == NULL)
    {
        return -1;
    }

    TiXmlElement* op = response->FirstChildElement("operator");
    if (op == NULL)
    {
        return -1;
    }

    const char* optype = op->GetText();
    if (optype == NULL)
    {
        return -1;
    }
    _operator_type = optype;
    _result = op->Attribute("result");
    _result_msg = op->Attribute("errMsg");
    op->Attribute("errCode", &_err_code);

    TiXmlElement* num = response->FirstChildElement("getNum");
    if (num != NULL)
    {
        const char* str_num = num->GetText();
        if (str_num == NULL)
        {
            return -1;
        }
        _number_to_send = HPR_Atoi32(str_num);
        const std::string attr_num = num->Attribute("offset");
        if (attr_num.length() == 0)
        {
            return -1;
        }
        _offset = HPR_Atoi32(attr_num.c_str());
        op->Attribute("total", &_total_number);

        _have_num_to_send = HPR_TRUE;
    }

    TiXmlElement* devices = response->FirstChildElement("devices");
    if (devices != NULL)
    {
        const std::string dev_num = devices->Attribute("devNum");
        if (dev_num.length() == 0)
        {
            return -1;
        }
        _device_number = HPR_Atoi32(dev_num.c_str());
        if (_device_number <= 0)
        {
            return -1;
        }

        _dev_status_list = new(std::nothrow) NET_MANAGE_DEVICE_INFO_T[_device_number];
        if (_dev_status_list == NULL)
        {
            return -1;
        }
        TiXmlElement* dev = devices->FirstChildElement("device");
        if (dev == NULL)
        {
            delete[] _dev_status_list;
            _dev_status_list = NULL;
            return -1;
        }
        const std::string index_code = dev->Attribute("devIndexCode");
        if (index_code.length() == 0)
        {
            delete[] _dev_status_list;
            _dev_status_list = NULL;
            return -1;
        }
        const std::string str_state = dev->Attribute("devState");
        if (str_state.length() == 0)
        {
            delete[] _dev_status_list;
            _dev_status_list = NULL;
            return -1;
        }
        HPR_Snprintf(_dev_status_list[0].device_index, 64, "%s", index_code.c_str() );
        _dev_status_list[0].device_status = HPR_Atoi32(str_state.c_str());
        HPR_INT32 device_num = 1;
        for (int i=1; i<_device_number; i++)
        {
            dev = dev->NextSiblingElement();
            if (dev == NULL)
            {
                break;
            }

            const std::string index_code1 = dev->Attribute("devIndexCode");
            if (index_code1.length() == 0)
            {
                delete[] _dev_status_list;
                _dev_status_list = NULL;
                return -1;
            }
            const std::string str_state1 = dev->Attribute("devState");
            if (str_state1.length() == 0)
            {
                delete[] _dev_status_list;
                _dev_status_list = NULL;
                return -1;
            }
            HPR_Snprintf(_dev_status_list[i].device_index, 64, "%s", index_code1.c_str() );
            _dev_status_list[i].device_status = HPR_Atoi32(str_state1.c_str());
            device_num++;
        }
        if (device_num != _device_number)
        {
            delete[] _dev_status_list;
            _dev_status_list = NULL;
            return -1;
        }

        delete[] _dev_status_list;
        _dev_status_list = NULL;

        _have_dev_list = HPR_TRUE;
    }

    return 0;
}

/*lint -save -e429*/
std::string net_manage_response_t::builder()
{
    TiXmlDocument doc;
    std::string out_string;

    TiXmlDeclaration declaration("1.0", "UTF-8", "yes");
    doc.InsertEndChild(declaration);

    TiXmlElement root("nmsBatchMsg");
    TiXmlElement response("response");

    TiXmlElement op("operator");
    TiXmlText* operator_text = new(std::nothrow)TiXmlText(_operator_type);
    if (operator_text == NULL)
    {
        return NULL;
    }
    op.LinkEndChild(operator_text);
    op.SetAttribute("result", _result);
    op.SetAttribute("errMsg", _result_msg);
    op.SetAttribute("errCode", _err_code);

    response.InsertEndChild(op);

    if (_have_num_to_send == HPR_TRUE)
    {
        TiXmlElement num_to_get("getNum");

        char str_num[8];
        memset(str_num, 0, 8);
        HPR_Itoa(str_num, _number_to_send, 10);
        TiXmlText* num = new(std::nothrow)TiXmlText(str_num);
        if (num == NULL)
        {
            return NULL;
        }
        num_to_get.LinkEndChild(num);

        num_to_get.SetAttribute("offset", _offset);
        num_to_get.SetAttribute("total", _total_number);

        response.InsertEndChild(num_to_get);
    }

    if (_have_dev_list == HPR_TRUE)
    {
        if (_dev_status_list == NULL)
        {
            return NULL;
        }

        TiXmlElement dev_list("devices");
        dev_list.SetAttribute("devNum", _device_number);

        for (int i=_offset; i<(_offset+_device_number); i++)
        {
            TiXmlElement device("device");
            device.SetAttribute("devIndexCode", _dev_status_list[i].device_index);
            device.SetAttribute("devState", _dev_status_list[i].device_status);
            dev_list.InsertEndChild(device);
        }

        response.InsertEndChild(dev_list);
    }

    root.InsertEndChild(response);
    doc.InsertEndChild(root);

    out_string << doc;

    return out_string;
}
/*lint -restore*/
