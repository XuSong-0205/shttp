#ifndef _SHTTP_H_
#define _SHTTP_H_

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>

#include "../utils/util.h"
#include "../utils/color.hpp"
#include "../socket/tcp_socket.h"
#include "../socket/epoll.h"
#include "../thread_pool/thread_pool.hpp"
#include "../json/nlohmann/json.hpp"

using std::cerr;
using std::clog;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::string;
using std::stringstream;
using std::ostringstream;
using std::unordered_map;
using std::unordered_set;
using nlohmann::json;

SX_BEGIN


class shttp
{
private:
    // 是否运行
    bool m_running;
    // 端口号
    uint16_t m_port;
    // 根目录
    string m_root_dir;
    // 服务器 TcpSocket 
    TcpSocket m_server_socket;
    // Epoll 对象
    Epoll m_epoll;
    // 线程池
    ThreadPool thread_pool;

public:
    shttp(uint16_t port = PORT);
    shttp(const shttp&) = delete;
    shttp(shttp&&) = delete;
    shttp& operator=(const shttp&) = delete;
    shttp& operator=(shttp&&) = delete;
    ~shttp();

public:
    // 设置服务器 root 目录
    string set_root_dir(const string& dir);
    // 获取服务器 root 目录
    string get_root_dir()const;

    // 运行服务器
    void run();
    // 关闭服务器
    void close()noexcept;

private:
    // 重新生成默认配置信息
    void create_config()const;
    // 读取配置信息
    void read_config();

private:
    // 准备工作
    bool do_ready();
    // 处理连接请求
    bool do_accept();
    // 处理可读消息
    bool do_read(TcpSocket tcp_sock, size_t pos);
    // 处理可写消息
    bool do_write(TcpSocket tcp_sock, size_t pos);
    // 处理挂起消息
    bool do_rdhup(TcpSocket tcp_sock, size_t pos);
    // 处理错误消息
    bool do_error(TcpSocket tcp_sock, size_t pos);

private:
    // 处理 http 请求
    bool do_http(TcpSocket tcp_sock, size_t pos);


    // 解析 http 请求
    bool parse_http_requests(stringstream& sstr, json& requests)const;
    // 解析 请求行
    bool parse_http_requests_line(stringstream& sstr, json& requests)const;
    // 解析 请求头
    bool parse_http_requests_headers(stringstream& sstr, json& requests)const;
    // 解析 请求体
    bool parse_http_requests_body(stringstream& sstr, json& requests)const;


    // 处理 http 请求
    bool process_http_requests(TcpSocket tcp_sock, json& requests)const;
    // 解析 url 
    bool parse_url(const string& url, json& url_json)const;
    // 解析 文件格式
    bool parse_content_type(const string& path, json& url_json)const;
    // 处理 url 路径参数
    bool process_http_path(json& url_json, string& text)const;
    // 处理 url 查询参数
    bool process_http_query(const json& url_json, json& response)const;


    // 从 stringstream 中读取一行
    bool read_line(stringstream& sstr, string& line)const;
    // 发送响应体
    bool send_response(TcpSocket tcp_sock, const json& response)const;


    // 判断是否是 http 方法
    bool is_http_method(const string& method)const;
    // 是否是 keep-alive 
    bool is_keep_alive(const json& requests)const;
    // 根据 content-type 字段判断是否是否二进制模式读取
    bool is_binary_read(const string& content_type)const;
    // 返回文件格式对应的 content-type 类型
    string get_content_type(const string& format)const;


    // 输出信息
    static std::ostream &out_info(OUT_LEVEL level)noexcept;
};

SX_END


#endif  //_SHTTP_H_