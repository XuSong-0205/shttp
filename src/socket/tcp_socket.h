#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_

#include <string>
#include <iostream>
#include "socket.h"

using std::clog;
using std::iostream;
using std::string;
class shttp;

SX_BEGIN

class TcpSocket : public Socket
{
private:
    using Socket::Socket;
    friend class shttp;

public:
    TcpSocket();
    TcpSocket(const TcpSocket &) = default;
    TcpSocket(TcpSocket &&) = default;
    TcpSocket &operator=(const TcpSocket &) = default;
    TcpSocket &operator=(TcpSocket &&) = default;
    ~TcpSocket() override = default;

public:
    // 创建 tcp socket 
    bool create()override;
    // 绑定 socket 到端口号
    bool bind(uint32_t ip, uint16_t port)const;
    bool bind(const char* ip, uint16_t port)const;
    bool bind(uint16_t port)const;
    // 设置为监听状态
    bool listen(int max_num = 5)const;
    // 监听连接
    TcpSocket accept()const;
    // 连接 socket 
    bool connect(int ip, uint16_t port)const;
    bool connect(const char* ip, uint16_t port)const;

    
    // 发送消息
    int send(const char* buff, size_t len)const;
    int send(const string &msg) const;
    template <size_t len>
    int send(const char (&buff)[len]) const
    {
        return send(buff, len);
    }

    // 接收消息
    int recv(char* buff, size_t len)const;
    template<size_t len>
    int recv(char(&buff)[len])const
    {
        return recv(buff, len);
    }

    // 按照 ET 进行接收数据并写入 iostream 中
    bool recv_et_to_stream(iostream &msg)const;

};

SX_END

#endif  // _TCP_SOCKET_H_