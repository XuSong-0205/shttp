#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "../utils/util.h"


SX_BEGIN


// socket 基类
class Socket
{
protected:
    int m_sockfd;

protected:
    explicit Socket(int sockfd);
    
public:
    Socket();
    Socket(const Socket &) = default;
    Socket(Socket &&) = default;
    Socket &operator=(const Socket &) = default;
    Socket &operator=(Socket &&) = default;
    virtual ~Socket() = default;

public:
    // 创建一个 socket 的接口
    virtual bool create() = 0;
    // 可以直接转为 int socket 类型
    operator int()const noexcept;
    // 获取内部 socket 
    int get()const noexcept;
    // 返回是否是有效的
    bool valid()const noexcept;
    // 关闭 socket 
    void close()noexcept;

    // 设置端口可重用
    bool set_reuseaddr()const;
    // 设置 socket 为非阻塞
    bool set_nonblock()const;
};


SX_END
    
#endif  // _SOCKET_H_