#include "CNET_PortPool.h"

unsigned int CNet_PortPool::m_nRefer = 0;

CNet_PortPool::CNet_PortPool(void)
    : m_minport(0)
    , m_maxport(0)
    , m_Size(0)
{
    m_portqueue.clear();
    m_nRefer++;
}

CNet_PortPool::~CNet_PortPool()
{
    m_minport = 0;
    m_maxport = 0;
    m_Size = 0;
    m_portqueue.clear();
    m_nRefer--;
}

CNet_PortPool* CNet_PortPool::create_new(unsigned short min_port, unsigned short max_port)
{
    CNet_PortPool* p = new(std::nothrow) CNet_PortPool();
    if (!p)
    {
        return NULL;
    }

    if (0 != p->crete_port_pairs(min_port, max_port))
    {
        delete p;
        p = NULL;
    }
    return p;
}


int CNet_PortPool::crete_port_pairs(unsigned short min_port, unsigned short max_port)
{
    m_portqueue.clear();

    for (unsigned short i = min_port; i <= max_port; i++)
    {
        try
        {
            m_portqueue.push_back(i);
        }
        catch (...)
        {
            return -1;
        }

        i = (unsigned short)(i + 1);
        m_Size++;
    }

    m_minport = min_port;
    m_maxport = max_port;

    return 0;
}


unsigned short CNet_PortPool::get_port_pair()
{
    unsigned short port = 0;
    std::lock_guard<std::mutex> lock(mutex_);
    if (m_portqueue.size())
    {
        port = m_portqueue.front();
        m_portqueue.pop_front();
    }
    return port;
}

int CNet_PortPool::reuse_port_pair(unsigned short port)
{
    if (!port)
    {
        return -1;
    }
    std::lock_guard<std::mutex> lock(mutex_);

    try
    {
        m_portqueue.push_back(port);
    }
    catch (...)
    {
        return -1;
    }
    return 0;
}


unsigned int CNet_PortPool::get_max_size()
{
    return m_Size;
}
unsigned int CNet_PortPool::get_curr_size()
{
    return m_portqueue.size();
}
