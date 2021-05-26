//
// Created by Meng Lv on 2021/5/23.
//

#include "JLX12864_2.c"


// 模式0_0
void mode0_0(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 17, "模式", 0);
    display_GB2312_string(3, 49, letter, 0);
    display_GB2312_string(3, 81, integer, 0);
    display_GB2312_string(3, 89, ".", 0);
    display_GB2312_string(3, 97, decimal, 0);
    display_GB2312_string(3, 105, "Hz", 0);
    display_GB2312_string(7, 1, "设置", 0);
}

// 模式0_1
void mode0_1(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 17, "模式", 0);
    display_GB2312_string(3, 49, letter, 0);
    display_GB2312_string(3, 81, integer, 0);
    display_GB2312_string(3, 89, ".", 0);
    display_GB2312_string(3, 97, decimal, 0);
    display_GB2312_string(3, 105, "Hz", 0);
    display_GB2312_string(7, 1, "设置", 1);
}

// 模式1_0
void mode1_0(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 17, "工作模式", 1);
    display_GB2312_string(5, 17, "工作参数", 0);
    display_GB2312_string(7, 97, "返回", 0);
}

// 模式1_1
void mode1_1(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 17, "工作模式", 0);
    display_GB2312_string(5, 17, "工作参数", 1);
    display_GB2312_string(7, 97, "返回", 0);
}

// 模式1_2
void mode1_2(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 17, "工作模式", 0);
    display_GB2312_string(5, 17, "工作参数", 0);
    display_GB2312_string(7, 97, "返回", 1);
}

// 模式2_0
void mode2_0(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 1, "工作模式：", 0);
    display_GB2312_string(3, 81, "模式", 1);
    display_GB2312_string(3, 113, letter, 1);
    display_GB2312_string(7, 1, "确定", 0);
    display_GB2312_string(7, 97, "取消", 0);
}

// 模式2_1
void mode2_1(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 1, "工作模式：", 0);
    display_GB2312_string(3, 81, "模式", 0);
    display_GB2312_string(3, 113, letter, 0);
    display_GB2312_string(7, 1, "确定", 1);
    display_GB2312_string(7, 97, "取消", 0);
}

// 模式2_2
void mode2_2(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 1, "工作模式：", 0);
    display_GB2312_string(3, 81, "模式", 0);
    display_GB2312_string(3, 113, letter, 0);
    display_GB2312_string(7, 1, "确定", 0);
    display_GB2312_string(7, 97, "取消", 1);
}

// 模式3_0
void mode3_0(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 1, "工作参数：", 0);
    display_GB2312_string(3, 81, integer, 1);
    display_GB2312_string(3, 89, ".", 0);
    display_GB2312_string(3, 97, decimal, 0);
    display_GB2312_string(3, 105, "Hz", 0);
    display_GB2312_string(7, 1, "确定", 0);
    display_GB2312_string(7, 97, "取消", 0);
}

// 模式3_1
void mode3_1(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 1, "工作参数：", 0);
    display_GB2312_string(3, 81, integer, 0);
    display_GB2312_string(3, 89, ".", 0);
    display_GB2312_string(3, 97, decimal, 1);
    display_GB2312_string(3, 105, "Hz", 0);
    display_GB2312_string(7, 1, "确定", 0);
    display_GB2312_string(7, 97, "取消", 0);
}

// 模式3_2
void mode3_2(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 1, "工作参数：", 0);
    display_GB2312_string(3, 81, integer, 0);
    display_GB2312_string(3, 89, ".", 0);
    display_GB2312_string(3, 97, decimal, 0);
    display_GB2312_string(3, 105, "Hz", 0);
    display_GB2312_string(7, 1, "确定", 1);
    display_GB2312_string(7, 97, "取消", 0);
}

// 模式3_3
void mode3_3(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 1, "工作参数：", 0);
    display_GB2312_string(3, 81, integer, 0);
    display_GB2312_string(3, 89, ".", 0);
    display_GB2312_string(3, 97, decimal, 0);
    display_GB2312_string(3, 105, "Hz", 0);
    display_GB2312_string(7, 1, "确定", 0);
    display_GB2312_string(7, 97, "取消", 1);
}

void mode4(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 1, "工作参数不合法", 0);
}

