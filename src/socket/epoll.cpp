#include "epoll.h"


SX_BEGIN

Epoll::Epoll() : m_epollfd(-1), m_events_num(0), m_events{ 0 }
{
    create();
}


bool Epoll::create()
{
    if (!valid())
    {
        m_epollfd = ::epoll_create(m_events.size() + 1);
    }

    return valid();
}

bool Epoll::add_event(const Socket& sock, uint32_t events)
{
    // 查找一个空的 events (待优化，哈希表，链表？)
    bool is_find = false;
    for (size_t i = 0; i < m_events.size(); ++i)
    {
        const auto index = (i + m_events_num) % m_events.size();
        if (m_events.at(m_events_num).data.fd != -1)
        {
            m_events_num = index;
            is_find = true;
            break;
        }
    }
    if (!is_find)
    {
        return false;
    }

    // 添加
    m_events.at(m_events_num).data.fd = sock;
    m_events.at(m_events_num).events = events;
    return epoll_ctl(m_epollfd, EPOLL_CTL_ADD, sock, &m_events.at(m_events_num++)) == 0;
}

Epoll::pair_array_const_iterator Epoll::wait(int wait_time)
{
    int num = epoll_wait(m_epollfd, m_events.data(), m_events.size(), wait_time);
    return { m_events.cbegin(), m_events.cbegin() + num };
}

bool Epoll::valid()const noexcept
{
    return m_epollfd != -1;
}

void Epoll::close()
{
    if (valid())
    {
        ::close(m_epollfd);
        m_epollfd = -1;
    }
}

bool Epoll::del_event(size_t pos)
{
    epoll_event event;
    event.data.fd = m_events.at(m_events_num).data.fd;
    event.events = m_events.at(m_events_num).events;
    // 修改当前 events 
    m_events.at(m_events_num).data.fd = -1;
    event.events = m_events.at(m_events_num).events = 0;
    m_events_num = pos;
    // 从 epoll 中删除
    return epoll_ctl(m_epollfd, EPOLL_CTL_DEL, event.data.fd, &event);
}



SX_END