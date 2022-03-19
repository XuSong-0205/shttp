#ifndef _EPOLL_H_
#define _EPOLL_H_

#include <sys/epoll.h>
#include <array>
#include <utility>
#include "socket.h"
using std::array;
using std::pair;

SX_BEGIN


class Epoll
{
private:
    int m_epollfd;
    array<epoll_event, EVENT_NUM> m_events;

    using array_type = decltype(m_events);
    using array_const_iterator = array_type::const_iterator;
    using pair_array_const_iterator = pair<array_const_iterator, array_const_iterator>;

public:
    Epoll();

    
    // 创建一个 epoll 句柄
    bool create();
    // 将 event 注册到 epoll 中，默认注册 可读|可写|挂起|错误|ET
    bool add_event(const Socket& sock, uint32_t events = DEFAULT_EVENTS)const;
    // 等待 epool 事件到来，默认阻塞，返回有事件的 m_events 的一组迭代器 [beg, end)
    pair_array_const_iterator wait(int wait_time = -1);
    // 是否有效
    bool valid()const noexcept;
    // 关闭 epoll
    void close();
    // 从 events 数组中删除一个 socket 
    bool del_event(size_t pos);

};


SX_END

#endif  // _EPOLL_H_