#ifndef _COLOR_H_
#define _COLOR_H_

#ifndef _COLOR
    #define COLOR_BEGIN namespace color {
    #define COLOR_END   }
    #define _COLOR      color
#endif  // _COLOR

#include <iostream>

COLOR_BEGIN


enum color : int
{
    // 字体颜色
    F_BLACK     = 30,       // 黑色
    F_RED       = 31,       // 红色
    F_GREEN     = 32,       // 绿色
    F_YELLOW    = 33,       // 黄色
    F_BLUE      = 34,       // 蓝色
    F_PURPLE    = 35,       // 紫色
    F_CYAN      = 36,       // 青色
    F_WHITE     = 37,       // 白色

    // 背景颜色
    B_BLACK     = 40,       // 黑色
    B_RED       = 41,       // 红色
    B_GREEN     = 42,       // 绿色
    B_YELLOW    = 43,       // 黄色
    B_BLUE      = 44,       // 蓝色
    B_PURPLE    = 45,       // 紫色
    B_CYAN      = 46,       // 青色
    B_WHITE     = 47        // 白色
};


enum status : int
{
    CLEAR   = 0x00,         // 清除所有
    CLEAR_F = 0x27,         // 清除字体颜色
    CLEAR_B = 0x31          // 清除背景颜色
};


inline std::ostream& set_attribute(std::ostream& os, int value)
{
    return os << "\033[" << value << "m";
}

inline std::ostream& operator<<(std::ostream& os, color col)
{
    return set_attribute(os, col);
}

inline std::ostream& operator<<(std::ostream& os, status stu)
{
    return set_attribute(os, stu);
}


COLOR_END

#endif	// _COLOR_H_