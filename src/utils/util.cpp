#include <sstream>
#include <cctype>
#include <ctime>
#include "util.h"

SX_BEGIN


// 去除字符串两边多余的空格
void strip(string& str)
{
    // const int size = str.size();
    // int left = 0;
    // while (left < size && ::isspace(str.at(left))) 
    // {
    //     ++left;
    // }
    // int right = size - 1;
    // while (right > left && ::isspace(str.at(right)))
    // {
    //     --right;
    // }
    //
    // if (left == 0)
    // {
    //     for (int i = 0, n = size - right - 1; i < n; ++i)
    //     {
    //         str.pop_back();
    //     }
    // }
    // else
    // {
    //     str = str.substr(left, right - left + 1);
    // }

    std::istringstream iss(str);
    iss >> str;
}

// 将字符串的大写字母转换成小写
void tolower(string& str)
{
    for (auto& c : str)
    {
        if (::isupper(c))
        {
            c = ::tolower(c);
        }
    }
}

// 将字符串的小写字母转换撑大写
void toupper(string& str)
{
    for (auto& c : str)
    {
        if (::islower(c))
        {
            c = ::toupper(c);
        }
    }
}

// 以字符 x 分割字符串，返回分割后的字符串列表
vector<string> split(const string& str, char ch)
{
    vector<string> vec;
    std::istringstream istr(str);
    string line;
    while (std::getline(istr, line, ch))
    {
        vec.push_back(line);
    }

    return vec;
}

// RFC 1123 
string RFC_1123()
{
    char datetime[48]{ 0 };
    const time_t now = time(nullptr);
    const tm* gmt = gmtime(&now);
    strftime(datetime, sizeof(datetime), "%a, %d %b %Y %X GMT", gmt);
    
    return datetime;
}


SX_END