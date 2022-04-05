#include "shttp.h"

SX_BEGIN

shttp::shttp(uint16_t port)
    : m_running(true),
    m_port(port),
    m_root_dir("../root")
{
    read_config();
}

shttp::~shttp()
{
    close();
}


// 重新生成默认配置信息
void shttp::create_config()const
{
    json conf = {
        { "port", PORT },
        { "root", get_root_dir() }
    };

    ofstream ofile("../config/config.json");
    ofile << conf.dump(4);

    out_info(INFO) << "create default config success\n";
}

// 读取配置信息
void shttp::read_config()
{
    ifstream ifile("../config/config.json");
    json conf;
    if (ifile.is_open())
    {
        ifile >> conf;
    }
    
    if (conf.contains("port") && conf.contains("root"))
    {
        m_port = conf.at("port");
        m_root_dir = conf.at("root");
    }
    else
    {
        if (!conf.contains("port"))
        {
            out_info(WARN) << "config miss key port\n";
        }
        if (!conf.contains("root"))
        {
            out_info(WARN) << "config miss key root\n";
        }

        create_config();
    }
}


// 准备工作
bool shttp::do_ready()
{
    if (!m_server_socket.create())
    {
        out_info(ERROR) << "create socket failed\n";
        return false;
    }

    if (!m_server_socket.set_reuseaddr())
    {
        out_info(ERROR) << "set reuseaddr failed\n";
        return false;
    }

    if (!m_server_socket.bind(m_port))
    {
        out_info(ERROR) << "bind socket failed\n";
        return false;
    }

    if (!m_server_socket.listen())
    {
        out_info(ERROR) << "listen socket failed\n";
        return false;
    }

    if (!m_server_socket.set_nonblock())
    {
        out_info(ERROR) << "set nonblock socket failed\n";
        return false;
    }

    if (!m_epoll.create())
    {
        out_info(ERROR) << "create epoll failed\n";
        return false;
    }

    if (!m_epoll.add_event(m_server_socket))
    {
        out_info(ERROR) << "epoll add server event failed\n";
        return false;
    }

    return true;
}

// 处理连接请求
bool shttp::do_accept()
{
    TcpSocket client_socket = m_server_socket.accept();
    if (!client_socket.valid())
    {
        out_info(WARN) << "accept client failed\n";
        return false;
    }

    if (!client_socket.set_nonblock())
    {
        out_info(WARN) << "client set nonblock failed\n";
        client_socket.close();
        return false;
    }

    if (!m_epoll.add_event(client_socket))
    {
        out_info(WARN) << "epoll add client event failed\n";
        client_socket.close();
        return false;
    }

    out_info(INFO) << "client accept socket : " << color::F_BLUE << client_socket << "\n";

    return true;
}

// 处理可读消息
bool shttp::do_read(TcpSocket tcp_sock, size_t pos)
{
    thread_pool.submit([this, tcp_sock, pos]() -> bool
                       { return do_http(tcp_sock, pos); });
    return true;
}

// 处理可写消息
bool shttp::do_write(TcpSocket tcp_sock, size_t pos)
{
    // todo...
    out_info(WARN) << "have write message, socket : " << tcp_sock << "\n";
    return true;
}

// 处理挂起消息
bool shttp::do_rdhup(TcpSocket tcp_sock, size_t pos)
{
    out_info(INFO) << "client close, socket : " << color::F_YELLOW << tcp_sock << "\n";
    tcp_sock.close();
    m_epoll.del_event(pos);
    return true;
}

// 处理错误消息
bool shttp::do_error(TcpSocket tcp_sock, size_t pos)
{
    out_info(INFO) << "client error, socket : " << color::F_YELLOW << tcp_sock << "\n";
    tcp_sock.close();
    m_epoll.del_event(pos);
    return true;
}


// 设置服务器 root 目录
string shttp::set_root_dir(const string& dir)
{
    string old_dir = m_root_dir;
    m_root_dir = dir;
    return old_dir;
}

// 获取服务器 root 目录
string shttp::get_root_dir()const
{
    return m_root_dir;
}


// 运行服务器
void shttp::run()
{
    if (!do_ready())
    {
        out_info(ERROR) << "shttp start failed\n";
        close();
        return;
    }

    out_info(INFO) << "shttp start success [" << color::F_BLUE << SERVER_IP << ":" << m_port << color::CLEAR << "] ...\n";
    out_info(INFO) << "shttp server is waiting for the client to connect ...\n";


    // 进入循环监听
    while (m_running)
    {
        // 等待在 epoll 中注册的事件到来
        const auto pair_iter = m_epoll.wait();
        // 遍历所有的事件
        for (auto it = pair_iter.first; it != pair_iter.second; ++it)
        {
            const int curr_socket = it->data.fd;
            const auto curr_events = it->events;
            const auto pos = it - pair_iter.first;

            if (curr_socket < 0)
            {
                continue;
            }

            // 如果是服务器 socket，则说明是客户端的连接请求
            if (curr_socket == m_server_socket)
            {
                // 处理连接请求
                do_accept();
                continue;
            }

            // 可读消息
            if (curr_events & EPOLLIN)
            {
                // 处理可读消息
                do_read(TcpSocket(curr_socket), pos);
            }

            // 可写消息
            if (curr_events & EPOLLOUT)
            {
                // 处理可写消息
                do_write(TcpSocket(curr_socket), pos);
            }

            // 挂起消息
            if (curr_events & EPOLLRDHUP)
            {
                // 处理挂起消息
                do_rdhup(TcpSocket(curr_socket), pos);
                continue;
            }

            // 错误消息
            if (curr_events & EPOLLERR)
            {
                // 处理错误消息
                do_error(TcpSocket(curr_socket), pos);
                continue;
            }
        }
    }

    // 运行完毕，关闭服务器
    close();
}

// 关闭服务器
void shttp::close()noexcept
{
    m_running = false;
    m_server_socket.close();
    m_epoll.close();
}



// 处理 http 请求
bool shttp::do_http(TcpSocket tcp_sock, size_t pos)
{
    // 接收客户端发送来的消息
    stringstream msg;
    if (!tcp_sock.recv_et_to_stream(msg))
    {
        // 读取失败，关闭 socket，从 epoll 池中删除，退出 http 解析
        tcp_sock.close();
        m_epoll.del_event(pos);
        return false;
    }

    json requests;
    // 解析 http 请求
    if (!parse_http_requests(msg, requests))
    {
        out_info(WARN) << "parse http requests failed\n";
        return false;
    }

    // 处理 http 请求
    if (!process_http_requests(tcp_sock, requests))
    {
        out_info(WARN) << "process http requests failed\n";
        return false;
    }

    // 如果不是 keep-alive 则关闭连接
    const bool keep_alive = is_keep_alive(requests);
    if (!keep_alive)
    {
        tcp_sock.close();
        m_epoll.del_event(pos);
    }

    out_info(INFO) << requests["method"] << " "
                   << requests["url"] << " "
                   << requests["version"] << " "
                   << (requests["status_code"] == HTTP_200_OK ? color::F_GREEN : color::F_RED)
                   << requests["status_code"] << " "
                   << requests["status_message"] << "\n";
    return keep_alive;
}

// 解析 http 请求
bool shttp::parse_http_requests(stringstream& sstr, json& requests)const
{
    return  parse_http_requests_line(sstr, requests) && 
            parse_http_requests_headers(sstr, requests) && 
            parse_http_requests_body(sstr, requests);
}

// 解析 请求行
bool shttp::parse_http_requests_line(stringstream& sstr, json& requests)const
{
    string line;
    // 读取一行
    if (!read_line(sstr, line))
    {
        return false;
    }

    // 以空格分割字符串
    stringstream requests_line(line);
    string method;
    string url;
    string version;
    if (requests_line >> method >> url >> version)
    {
        if (!is_http_method(method))
        {
            out_info(WARN) << "parse http method failed\n";
            requests["status_code"] = HTTP_400_BAD_REQUEST;
            return false;
        }

        requests["method"] = method;
        requests["url"] = url;
        requests["version"] = version;
    }
    else
    {
        out_info(WARN) << "prase http requests line failed\n";
        requests["status_code"] = HTTP_400_BAD_REQUEST;
        return false;
    }

    return true;
}

// 解析 请求头
bool shttp::parse_http_requests_headers(stringstream& sstr, json& requests)const
{
    string line;
    string key;
    string value;
    // 循环解析所有的请求头
    while (read_line(sstr, line))
    {
        // 遇到空行，请求头解析完毕
        if (line.empty())
        {
            break;
        }

        size_t pos = line.find(':');
        if (pos == string::npos)
        {
            out_info(WARN) << "parse http requests headers failed, line : " << line << "\n";
            requests["status_code"] = HTTP_400_BAD_REQUEST;
            return false;
        }

        // 解析出 key: value
        key = line.substr(0, pos++);
        value = line.substr(pos, line.size() - pos);

        tolower(key);
        strip(value);

        requests["headers"][key] = value;
    }

    return true;
}

// 解析 请求体
bool shttp::parse_http_requests_body(stringstream& sstr, json& requests)const
{
    string body;
    sstr >> body;

    if (!body.empty())
    {
        // 根据 content-type: application/json 解析请求体
        string body_format = requests["content-type"];
        tolower(body_format);
        if (body_format == "application/json")
        {
            requests["body"] = json::parse(body);
        }
        else
        {
            requests["body"] = body;
        }
    }

    return true;
}



// 处理 http 请求
bool shttp::process_http_requests(TcpSocket tcp_sock, json& requests)const
{
    // 解析 url，是否有请求参数
    json url_json;
    parse_url(requests["url"], url_json);

    if (!requests.contains("status_code"))
    {
        requests["status_code"] = HTTP_200_OK;
        requests["status_message"] = "OK";
    }

    // 响应体
    json response;
    // 解析 path 格式
    parse_content_type(url_json["path"], url_json);
    // 没有查询参数
    if (!url_json.contains("query"))
    {
        string body;
        if (!process_http_path(url_json, body))
        {
            requests["status_code"] = HTTP_400_BAD_REQUEST;
        }

        response["body"] = std::move(body);
    }
    else
    {
        // 有查询参数
        if (!process_http_query(url_json, response))
        {
            requests["status_code"] = HTTP_501_NOT_IMPLEMENTED;
            requests["status_message"] = string("not implemented");
            response["body"] = "shttp server process query failed";
        }
    }

    // 设置状态消息
    if (requests["status_code"] == HTTP_400_BAD_REQUEST)
    {
        requests["status_message"] = "bad requests";
    }


    // out_info(TRACE) << "url_json : \n" << std::setw(4) << url_json << "\n";


    // 响应行
    response["version"] = string(HTTP_VERSION);
    response["status_code"] = std::to_string(requests["status_code"].get<int>());
    response["status_message"] = requests["status_message"];
    
    // 响应头
    response["headers"]["server"] = string(SERVER_NAME) + " " + SERVER_VERSION;
    response["headers"]["date"] = RFC_1123();
    response["headers"]["content-type"] = url_json["content-type"];
    response["headers"]["content-length"] = std::to_string(response["body"].get<string>().size());
    if (requests["headers"].contains("connection"))
    {
        response["headers"]["connection"] = requests["headers"]["connection"];
    }

    // 发送响应体
    return send_response(tcp_sock, response);
}

// 解析 url 
bool shttp::parse_url(const string& url, json& url_json)const
{
    const size_t root = url.find('/');
    const size_t query = url.find('?', root);
    if (query == string::npos)
    {
        string path = url.substr(root, url.size() - root);
        // /a/b/    '/' 结尾默认添加 index.html
        if (path.back() == '/')
        {
            path += "index.html";
        }
        url_json["path"] = path;

        return true;
    }

    url_json["path"] = url.substr(root, query - root);
    // 解析查询参数
    // /get_image?q=12&a=1&b=3
    vector<string> vec = split(url.substr(query), '&');
    for (const auto& kv : vec)
    {
        const size_t eq_pos = kv.find('=');
        if (eq_pos == string::npos)
        {
            return false;
        }

        string key = kv.substr(0, eq_pos);
        string value = kv.substr(eq_pos + 1);
        url_json["query"][key] = value;
    }
    
    return true;
}

// 解析 文件格式
bool shttp::parse_content_type(const string& path, json& url_json)const
{
    // 获取 content-type
    const size_t point = path.rfind('.');
    string format = point == string::npos ? path : path.substr(point + 1);
    url_json["content-type"] = get_content_type(format);
    return true;
}

// 处理 url 路径参数
bool shttp::process_http_path(json& url_json, string& text)const
{
    // 当前 url 路径, path
    string path = url_json["path"];
    // 文件内容
    string file_text;

    // 当前完整路径
    path = get_root_dir() + path;
    ifstream ifile;
    // 是否使用二进制读取
    if (is_binary_read(url_json["content-type"]))
    {
        // 二进制读
        ifile.open(path, std::ios::binary | std::ios::in);
    }
    else
    {
        // 默认读
        ifile.open(path);
    }

    if (ifile.is_open())
    {
        ostringstream ostr;
        ostr << ifile.rdbuf();
        file_text = ostr.str();
        ifile.close();
    }
    else
    {
        // 默认为 404 not found
        text = "404 not found";

        // 如果是 html 页面 404 了，则返回 404.html
        if (url_json["content-type"] == get_content_type("html"))
        {
            // 文件未打开，404 了QAQ~
            // 返回 404 页面
            path = get_root_dir() + "/404.html";
            ifile.open(path);
            if (ifile.is_open())
            {
                url_json["content-type"] = get_content_type("html");
                ostringstream ostr;
                ostr << ifile.rdbuf();
                text = ostr.str();
                ifile.close();
            }
        }

        return false;
    }

    text = file_text;
    return true;
}

// 处理 url 查询参数
bool shttp::process_http_query(const json& url_json, json& response)const
{
    // todo ...
    return false;
}



// 从 stringstream 中读取一行
bool shttp::read_line(stringstream &sstr, string &line)const
{
    // 读取一行
    std::getline(sstr, line, '\n');
    // 格式判断
    if (line.empty() || line.back() != '\r')
    {
        out_info(WARN) << "read http a line failed, line : " << line <<  "\n";
        line.clear();
        return false;
    }

    // 去除 \r
    line.pop_back();
    return true;
}

// 发送响应体
bool shttp::send_response(TcpSocket tcp_sock, const json& response)const
{
    string msg;
    // 响应行
    msg += response["version"].get<string>() + " ";
    msg += response["status_code"].get<string>() + " ";
    msg += response["status_message"].get<string>() + "\r\n";
    // 响应头
    for (const auto& obj : response["headers"].items())
    {
        msg += obj.key();
        msg += ": ";
        msg += obj.value();
        msg += "\r\n";
    }
    // 空行
    msg += "\r\n";
    // 响应体
    msg += response["body"];

    // 发送响应体
    return tcp_sock.send(msg);
}



// 判断是否是 http 方法
bool shttp::is_http_method(const string& method)const
{
    static const unordered_set<string> http_method{"GET", "POST", "HEAD", "PUT", "DELETE", "PATCH"};
    return http_method.count(method) == 1;
}

// 是否是 keep-alive 
bool shttp::is_keep_alive(const json& requests)const
{
    string value;
    if (requests["headers"].contains("connection"))
    {
        value = requests["headers"]["connection"];
        tolower(value);
    }
    
    return value == "keep-alive";
}

// 根据 content-type 字段判断是否是否二进制模式读取
bool shttp::is_binary_read(const string& content_type)const
{
    // 图片使用 二进制打开
    return content_type.find("image") != string::npos;
}

// 返回文件格式对应的 content-type 类型
string shttp::get_content_type(const string& format)const
{
    static const unordered_map<string, string> format_content_type = {
        { "htm",    "text/html" },
        { "html",   "text/html" },
        { "css",    "text/css" },
        { "js",     "application/x-javascript" },
        { "json",   "application/json" },
        { "jpg",    "image/jpeg" },
        { "jpeg",   "image/jpeg" },
        { "png",    "image/png" },
        { "gif",    "image/gif" },
        { "ico",    "image/x-icon" },
        { "mp3",    "audio/mp3" },
        { "mp4",    "video/mpeg4" },
        { "pdf",    "application/pdf" },
        { "ppt",    "application/x-ppt" },
        { "wav",    "audio/wav" },
        { "xls",    "application/x-xls" },
        { "xml",    "xml" }
    };

    // 默认为 纯文本格式
    string conten_type = "text/plain";
    return format_content_type.find(format) != format_content_type.end() ? format_content_type.at(format) : conten_type;
}



// 输出信息
std::ostream& shttp::out_info(OUT_LEVEL level)noexcept
{
    switch (level)
    {
        case OUT_LEVEL::ERROR:
            cout << color::F_RED << "ERROR:\t";
            break;
        case OUT_LEVEL::WARN:
            cout << color::F_YELLOW << "WARN: \t";
            break;
        case OUT_LEVEL::INFO:
            cout << color::F_GREEN << "INFO" << color::CLEAR << ":\t";
            break;
        case OUT_LEVEL::DEBUG:
            cout << color::F_BLUE << "DEBUG" << color::CLEAR << ":\t";
            break;
        case OUT_LEVEL::TRACE:
            cout << color::F_WHITE << "TRACE" << color::CLEAR << ":\t";
            break;

        default:
            cout << color::F_PURPLE << "LEVEL ERROR" << color::CLEAR << ":\t";
            break;
        }

    return cout;
}

SX_END