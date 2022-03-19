#include "tcp_socket.h"

SX_BEGIN

TcpSocket::TcpSocket()
{
    create();
}


bool TcpSocket::create()
{
    if (!valid())
    {
        m_sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    }

    return valid();
}

bool TcpSocket::bind(uint32_t ip, uint16_t port)const
{
    sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(port);
    addr_in.sin_addr.s_addr = htonl(ip);
    return ::bind(get(), reinterpret_cast<sockaddr*>(&addr_in), sizeof(sockaddr)) == 0;
}

bool TcpSocket::bind(const char* ip, uint16_t port)const
{
    return bind(inet_addr(ip), port);
}

bool TcpSocket::bind(uint16_t port)const
{
    return bind(htonl(INADDR_ANY), port);
}

bool TcpSocket::listen(int max_num)const
{
    return ::listen(m_sockfd, max_num) == 0;
}

TcpSocket TcpSocket::accept()const
{
    sockaddr addr;
    socklen_t size = sizeof(sockaddr);
    int sockfd = ::accept(m_sockfd, &addr, &size);
    return TcpSocket(sockfd);
}

bool TcpSocket::connect(int ip, uint16_t port)const
{
    sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(port);
    addr_in.sin_addr.s_addr = htonl(ip);
    return ::connect(m_sockfd, reinterpret_cast<sockaddr*>(&addr_in), sizeof(sockaddr)) == 0;
}


int TcpSocket::send(const char* buff, size_t len)const
{
    return ::send(m_sockfd, buff, len, 0);
}

int TcpSocket::send(const string &msg) const
{
    return send(msg.c_str(), msg.size());
}

int TcpSocket::recv(char* buff, size_t len)const
{
    return ::recv(m_sockfd, buff, len, 0);
}

bool TcpSocket::recv_et_to_stream(iostream &msg)const
{
    char buff[BUFF_SIZE]{ 0 };
    while (true)
    {
        const int len = recv(buff);
        if (len > 0)
        {
            // 写入
            msg << buff;
            // 读取不足 BUFF_SIZE 说明已经读取完
            if (len < BUFF_SIZE)
            {
                return true;
            }
        }
        else if (len == 0)
        {
            // 主动关闭
            clog << "recv failed because client close, socket : " << m_sockfd << "\n";
            break;
        }
        else if (len == -1)
        {
            // EAGAIN/EWOULDBLOCK 提示你的应用程序现在没有数据可读请稍后再试
            // EINTR 指操作被中断唤醒，需要重新读
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            {
                clog << "recv failed bacause EAGAIN or EWOULDBLOCK\n";
                break;
            }
            else if (errno == EINTR)
            {
                clog << "recv warning because EINTR\n";
                continue;
            }
            // 异常断开情况
            else
            {
                clog << "recv failed unknown error\n";
                break;
            }
        }
    }

    return false;
}

SX_END