//
// Created by Meng Lv on 2021/5/23.
//

#include "JLX12864_2.c"


// 模式0_0
void mode0_0(uint8_t* v_passin[], uint8_t* t_passin[], uint8_t* f_passin[]){
//    clear_screen();
    display_GB2312_string(1, 1, "电压:", 0);
    display_GB2312_string(1, 41, v_passin[0], 0);
    display_GB2312_string(1, 49, ".", 0);
    display_GB2312_string(1, 57, v_passin[1], 0);
    display_GB2312_string(1, 65, v_passin[2], 0);
    display_GB2312_string(1, 73, v_passin[3], 0);
    display_GB2312_string(1, 81, "V", 0);
    display_GB2312_string(3, 1, "温度:", 0);
    display_GB2312_string(3, 81, t_passin[0], 0);
    display_GB2312_string(3, 89, t_passin[1], 0);
    display_GB2312_string(3, 97, ".", 0);
    display_GB2312_string(3, 105, t_passin[2], 0);
    display_GB2312_string(3, 113, "°", 0);
    display_GB2312_string(3, 121, "C", 0);
}


// 模式1_0
void mode1_0(uint8_t* v_passin[], uint8_t* t_passin[], uint8_t* f_passin[]){
    clear_screen();
    display_GB2312_string(1, 1, "载频:", 0);
    display_GB2312_string(1, 41, f_passin[0], 1);
    display_GB2312_string(1, 49, f_passin[1], 0);
    display_GB2312_string(1, 57, f_passin[2], 0);
    display_GB2312_string(1, 65, ".", 0);
    display_GB2312_string(1, 73, f_passin[3], 0);
    display_GB2312_string(1, 81, "M", 0);
    display_GB2312_string(1, 89, "H", 0);
    display_GB2312_string(1, 97, "z", 0);
    display_GB2312_string(7, 97, "确定", 0);
}

// 模式1_1
void mode1_1(uint8_t* v_passin[], uint8_t* t_passin[], uint8_t* f_passin[]){
    clear_screen();
    display_GB2312_string(1, 1, "载频:", 0);
    display_GB2312_string(1, 41, f_passin[0], 0);
    display_GB2312_string(1, 49, f_passin[1], 1);
    display_GB2312_string(1, 57, f_passin[2], 0);
    display_GB2312_string(1, 65, ".", 0);
    display_GB2312_string(1, 73, f_passin[3], 0);
    display_GB2312_string(1, 81, "M", 0);
    display_GB2312_string(1, 89, "H", 0);
    display_GB2312_string(1, 97, "z", 0);
    display_GB2312_string(7, 97, "确定", 0);
}

// 模式1_2
void mode1_2(uint8_t* v_passin[], uint8_t* t_passin[], uint8_t* f_passin[]){
    clear_screen();
    display_GB2312_string(1, 1, "载频:", 0);
    display_GB2312_string(1, 41, f_passin[0], 0);
    display_GB2312_string(1, 49, f_passin[1], 0);
    display_GB2312_string(1, 57, f_passin[2], 1);
    display_GB2312_string(1, 65, ".", 0);
    display_GB2312_string(1, 73, f_passin[3], 0);
    display_GB2312_string(1, 81, "M", 0);
    display_GB2312_string(1, 89, "H", 0);
    display_GB2312_string(1, 97, "z", 0);
    display_GB2312_string(7, 97, "确定", 0);
}

// 模式1_3
void mode1_3(uint8_t* v_passin[], uint8_t* t_passin[], uint8_t* f_passin[]){
    clear_screen();
    display_GB2312_string(1, 1, "载频:", 0);
    display_GB2312_string(1, 41, f_passin[0], 0);
    display_GB2312_string(1, 49, f_passin[1], 0);
    display_GB2312_string(1, 57, f_passin[2], 0);
    display_GB2312_string(1, 65, ".", 0);
    display_GB2312_string(1, 73, f_passin[3], 1);
    display_GB2312_string(1, 81, "M", 0);
    display_GB2312_string(1, 89, "H", 0);
    display_GB2312_string(1, 97, "z", 0);
    display_GB2312_string(7, 97, "确定", 0);
}

// 模式1_4
void mode1_4(uint8_t* v_passin[], uint8_t* t_passin[], uint8_t* f_passin[]){
    clear_screen();
    display_GB2312_string(1, 1, "载频:", 0);
    display_GB2312_string(1, 41, f_passin[0], 0);
    display_GB2312_string(1, 49, f_passin[1], 0);
    display_GB2312_string(1, 57, f_passin[2], 0);
    display_GB2312_string(1, 65, ".", 0);
    display_GB2312_string(1, 73, f_passin[3], 0);
    display_GB2312_string(1, 81, "M", 0);
    display_GB2312_string(1, 89, "H", 0);
    display_GB2312_string(1, 97, "z", 0);
    display_GB2312_string(7, 97, "确定", 1);
}


// 模式2_0
void mode2_0(uint8_t* v_passin[], uint8_t* t_passin[], uint8_t* f_passin[]){
    clear_screen();
    display_GB2312_string(1, 1, "载频超范围！", 0);
}



