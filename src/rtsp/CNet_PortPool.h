#ifndef __NET_PORT_POOL_H__
#define __NET_PORT_POOL_H__
#include <deque>
#include <mutex>
class CNet_PortPool
{
public:
	CNet_PortPool();
	~CNet_PortPool();
    static CNet_PortPool* create_new(unsigned short min, unsigned short max);
    unsigned short get_port_pair();
    int reuse_port_pair(unsigned short port);
    unsigned int get_max_size();
    unsigned int get_curr_size();
private:
    int crete_port_pairs(unsigned short min_port = 30000, unsigned short max_port = 35000);
private:
    unsigned short m_minport;
    unsigned short m_maxport;
    std::deque<unsigned short> m_portqueue; //!< port queue
    unsigned int m_Size;
    static unsigned int m_nRefer;
    std::mutex mutex_;
};

#endif
