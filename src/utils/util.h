#ifndef _UTIL_H_
#define _UTIL_H_

#ifndef _SX
#define _SX

#define SX_BEGIN    namespace sx {
#define SX_END      }

#endif  // _SX

#include <sys/epoll.h>
#include <string>
#include <vector>

using std::string;
using std::vector;


SX_BEGIN


// http 版本
constexpr const char* HTTP_VERSION = "HTTP/1.0";

// 服务器名字
constexpr const char* SERVER_NAME = "shttp";

// 服务器版本
constexpr const char* SERVER_VERSION = "v0.1.0";

// 本地 ip 地址
constexpr const char* SERVER_IP = "127.0.0.1";



// 端口号
constexpr uint16_t PORT = 10621;

// buff 数组长度
constexpr int BUFF_SIZE = 2048;

// event 数量
constexpr int EVENT_NUM = 1024;

// epoll 默认添加的事件，可读|可写|挂起|错误|ET     EPOLLRDHUP EPOLLOUT
constexpr uint32_t DEFAULT_EVENTS = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLET;


////////////////////////////////////////////////////////////////////////////

// shttp 输出信息等级
enum OUT_LEVEL : int
{
    ERROR   = 0,    // 错误
    WARN    = 1,    // 警告
    INFO    = 2,    // 信息
    DEBUG   = 3,    // 调试
    TRACE   = 4     // 详细信息
};

// http 状态码
enum HTTP_STATUS_CODE : int
{
    HTTP_100_CONTINUE = 100,
    HTTP_101_SWITCHING_PROTOCOLS = 101,
    HTTP_102_PROCESSING = 102,
    HTTP_103_EARLY_HINTS = 103,
    HTTP_200_OK = 200,
    HTTP_201_CREATED = 201,
    HTTP_202_ACCEPTED = 202,
    HTTP_203_NON_AUTHORITATIVE_INFORMATION = 203,
    HTTP_204_NO_CONTENT = 204,
    HTTP_205_RESET_CONTENT = 205,
    HTTP_206_PARTIAL_CONTENT = 206,
    HTTP_207_MULTI_STATUS = 207,
    HTTP_208_ALREADY_REPORTED = 208,
    HTTP_226_IM_USED = 226,
    HTTP_300_MULTIPLE_CHOICES = 300,
    HTTP_301_MOVED_PERMANENTLY = 301,
    HTTP_302_FOUND = 302,
    HTTP_303_SEE_OTHER = 303,
    HTTP_304_NOT_MODIFIED = 304,
    HTTP_305_USE_PROXY = 305,
    HTTP_306_RESERVED = 306,
    HTTP_307_TEMPORARY_REDIRECT = 307,
    HTTP_308_PERMANENT_REDIRECT = 308,
    HTTP_400_BAD_REQUEST = 400,
    HTTP_401_UNAUTHORIZED = 401,
    HTTP_402_PAYMENT_REQUIRED = 402,
    HTTP_403_FORBIDDEN = 403,
    HTTP_404_NOT_FOUND = 404,
    HTTP_405_METHOD_NOT_ALLOWED = 405,
    HTTP_406_NOT_ACCEPTABLE = 406,
    HTTP_407_PROXY_AUTHENTICATION_REQUIRED = 407,
    HTTP_408_REQUEST_TIMEOUT = 408,
    HTTP_409_CONFLICT = 409,
    HTTP_410_GONE = 410,
    HTTP_411_LENGTH_REQUIRED = 411,
    HTTP_412_PRECONDITION_FAILED = 412,
    HTTP_413_REQUEST_ENTITY_TOO_LARGE = 413,
    HTTP_414_REQUEST_URI_TOO_LONG = 414,
    HTTP_415_UNSUPPORTED_MEDIA_TYPE = 415,
    HTTP_416_REQUESTED_RANGE_NOT_SATISFIABLE = 416,
    HTTP_417_EXPECTATION_FAILED = 417,
    HTTP_418_IM_A_TEAPOT = 418,
    HTTP_421_MISDIRECTED_REQUEST = 421,
    HTTP_422_UNPROCESSABLE_ENTITY = 422,
    HTTP_423_LOCKED = 423,
    HTTP_424_FAILED_DEPENDENCY = 424,
    HTTP_425_TOO_EARLY = 425,
    HTTP_426_UPGRADE_REQUIRED = 426,
    HTTP_428_PRECONDITION_REQUIRED = 428,
    HTTP_429_TOO_MANY_REQUESTS = 429,
    HTTP_431_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
    HTTP_451_UNAVAILABLE_FOR_LEGAL_REASONS = 451,
    HTTP_500_INTERNAL_SERVER_ERROR = 500,
    HTTP_501_NOT_IMPLEMENTED = 501,
    HTTP_502_BAD_GATEWAY = 502,
    HTTP_503_SERVICE_UNAVAILABLE = 503,
    HTTP_504_GATEWAY_TIMEOUT = 504,
    HTTP_505_HTTP_VERSION_NOT_SUPPORTED = 505,
    HTTP_506_VARIANT_ALSO_NEGOTIATES = 506,
    HTTP_507_INSUFFICIENT_STORAGE = 507,
    HTTP_508_LOOP_DETECTED = 508,
    HTTP_510_NOT_EXTENDED = 510,
    HTTP_511_NETWORK_AUTHENTICATION_REQUIRED = 511
};

////////////////////////////////////////////////////////////////////////////


// 去除字符串两边多余的空格
void strip(string& str);

// 将字符串的大写字母转换成小写
void tolower(string& str);

// 将字符串的小写字母转换撑大写
void toupper(string& str);

// 以字符 x 分割字符串，返回分割后的字符串列表，分隔符默认为空格 ' '
vector<string> split(const string& str, char ch = ' ');

// RFC 1123 
string RFC_1123();



SX_END

#endif  // _UTIL_H_