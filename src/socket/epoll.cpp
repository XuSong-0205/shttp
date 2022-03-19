#include "epoll.h"


SX_BEGIN

Epoll::Epoll() : m_epollfd(-1), m_events{ 0 }
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

bool Epoll::add_event(const Socket& sock, uint32_t events)const
{
    epoll_event event;
    event.data.fd = sock;
    event.events = events;
    return epoll_ctl(m_epollfd, EPOLL_CTL_ADD, sock, &event) == 0;
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
    if (pos < m_events.size())
    {
        m_events.at(pos).data.fd = -1;
        return true;
    }

    return false;
}


SX_END