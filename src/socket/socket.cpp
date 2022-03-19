#include "socket.h"

SX_BEGIN

Socket::Socket(int sockfd) : m_sockfd(sockfd) { }

Socket::Socket() : Socket(-1) { }


// 可以直接转为 int socket 类型
Socket::operator int()const noexcept
{
    return m_sockfd;
}

// 获取内部 socket 
int Socket::get()const noexcept
{
    return m_sockfd;
}

// 是否是有效的
bool Socket::valid()const noexcept
{
    return m_sockfd != -1;
}

// 关闭 socket 
void Socket::close()noexcept
{
    if (!valid())
    {
        ::close(m_sockfd);
        m_sockfd = -1;
    }
}


// 设置端口可重用
bool Socket::set_reuseaddr()const
{
    int on = 1;
    return setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == 0;
}

// 设置 socket 为非阻塞
bool Socket::set_nonblock()const
{
    if (!valid()) return false;
    
    int opts = ::fcntl(m_sockfd, F_GETFL, 0);
    if (opts == -1) return false;

    opts |= O_NONBLOCK;
    return ::fcntl(m_sockfd, F_SETFL, opts) != -1;
}


SX_END