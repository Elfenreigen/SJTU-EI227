//
// Created by Meng Lv on 2021/5/23.
//

#include "JLX12864_2.c"
#include "tm1638.h"

// 模式0
void mode0(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    display_GB2312_string(3, 1, "工作模式：", 0);
    display_GB2312_string(3, 81, "模式", 0);
    display_GB2312_string(3, 113, letter, 0);
    display_GB2312_string(5, 1, "工作参数：", 0);
    display_GB2312_string(5, 81, integer, 0);
    display_GB2312_string(5, 89, ".", 0);
    display_GB2312_string(5, 97, decimal, 0);
    display_GB2312_string(5, 105, "Hz", 0);
}

// 模式1
void mode1(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    display_GB2312_string(3, 1, "工作模式：", 0);
    display_GB2312_string(3, 81, "模式", 1);
    display_GB2312_string(3, 113, letter, 1);
    display_GB2312_string(5, 1, "工作参数：", 0);
    display_GB2312_string(5, 81, integer, 0);
    display_GB2312_string(5, 89, ".", 0);
    display_GB2312_string(5, 97, decimal, 0);
    display_GB2312_string(5, 105, "Hz", 0);
}

// 模式2
void mode2(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    display_GB2312_string(3, 1, "工作模式：", 0);
    display_GB2312_string(3, 81, "模式", 0);
    display_GB2312_string(3, 113, letter, 0);
    display_GB2312_string(5, 1, "工作参数：", 0);
    display_GB2312_string(5, 81, integer, 1);
    display_GB2312_string(5, 89, ".", 0);
    display_GB2312_string(5, 97, decimal, 0);
    display_GB2312_string(5, 105, "Hz", 0);
}

// 模式3
void mode3(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    display_GB2312_string(3, 1, "工作模式：", 0);
    display_GB2312_string(3, 81, "模式", 0);
    display_GB2312_string(3, 113, letter, 0);
    display_GB2312_string(5, 1, "工作参数：", 0);
    display_GB2312_string(5, 81, integer, 0);
    display_GB2312_string(5, 89, ".", 0);
    display_GB2312_string(5, 97, decimal, 1);
    display_GB2312_string(5, 105, "Hz", 0);
}
