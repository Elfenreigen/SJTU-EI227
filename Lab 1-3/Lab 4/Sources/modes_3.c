//
// Created by Meng Lv on 2021/5/23.
//

#include "JLX12864_2.c"


// ģʽ0_0
void mode0_0(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 17, "ģʽ", 0);
    display_GB2312_string(3, 49, letter, 0);
    display_GB2312_string(3, 81, integer, 0);
    display_GB2312_string(3, 89, ".", 0);
    display_GB2312_string(3, 97, decimal, 0);
    display_GB2312_string(3, 105, "Hz", 0);
    display_GB2312_string(7, 1, "����", 0);
}

// ģʽ0_1
void mode0_1(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 17, "ģʽ", 0);
    display_GB2312_string(3, 49, letter, 0);
    display_GB2312_string(3, 81, integer, 0);
    display_GB2312_string(3, 89, ".", 0);
    display_GB2312_string(3, 97, decimal, 0);
    display_GB2312_string(3, 105, "Hz", 0);
    display_GB2312_string(7, 1, "����", 1);
}

// ģʽ1_0
void mode1_0(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 17, "����ģʽ", 1);
    display_GB2312_string(5, 17, "��������", 0);
    display_GB2312_string(7, 97, "����", 0);
}

// ģʽ1_1
void mode1_1(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 17, "����ģʽ", 0);
    display_GB2312_string(5, 17, "��������", 1);
    display_GB2312_string(7, 97, "����", 0);
}

// ģʽ1_2
void mode1_2(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 17, "����ģʽ", 0);
    display_GB2312_string(5, 17, "��������", 0);
    display_GB2312_string(7, 97, "����", 1);
}

// ģʽ2_0
void mode2_0(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 1, "����ģʽ��", 0);
    display_GB2312_string(3, 81, "ģʽ", 1);
    display_GB2312_string(3, 113, letter, 1);
    display_GB2312_string(7, 1, "ȷ��", 0);
    display_GB2312_string(7, 97, "ȡ��", 0);
}

// ģʽ2_1
void mode2_1(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 1, "����ģʽ��", 0);
    display_GB2312_string(3, 81, "ģʽ", 0);
    display_GB2312_string(3, 113, letter, 0);
    display_GB2312_string(7, 1, "ȷ��", 1);
    display_GB2312_string(7, 97, "ȡ��", 0);
}

// ģʽ2_2
void mode2_2(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 1, "����ģʽ��", 0);
    display_GB2312_string(3, 81, "ģʽ", 0);
    display_GB2312_string(3, 113, letter, 0);
    display_GB2312_string(7, 1, "ȷ��", 0);
    display_GB2312_string(7, 97, "ȡ��", 1);
}

// ģʽ3_0
void mode3_0(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 1, "����������", 0);
    display_GB2312_string(3, 81, integer, 1);
    display_GB2312_string(3, 89, ".", 0);
    display_GB2312_string(3, 97, decimal, 0);
    display_GB2312_string(3, 105, "Hz", 0);
    display_GB2312_string(7, 1, "ȷ��", 0);
    display_GB2312_string(7, 97, "ȡ��", 0);
}

// ģʽ3_1
void mode3_1(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 1, "����������", 0);
    display_GB2312_string(3, 81, integer, 0);
    display_GB2312_string(3, 89, ".", 0);
    display_GB2312_string(3, 97, decimal, 1);
    display_GB2312_string(3, 105, "Hz", 0);
    display_GB2312_string(7, 1, "ȷ��", 0);
    display_GB2312_string(7, 97, "ȡ��", 0);
}

// ģʽ3_2
void mode3_2(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 1, "����������", 0);
    display_GB2312_string(3, 81, integer, 0);
    display_GB2312_string(3, 89, ".", 0);
    display_GB2312_string(3, 97, decimal, 0);
    display_GB2312_string(3, 105, "Hz", 0);
    display_GB2312_string(7, 1, "ȷ��", 1);
    display_GB2312_string(7, 97, "ȡ��", 0);
}

// ģʽ3_3
void mode3_3(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 1, "����������", 0);
    display_GB2312_string(3, 81, integer, 0);
    display_GB2312_string(3, 89, ".", 0);
    display_GB2312_string(3, 97, decimal, 0);
    display_GB2312_string(3, 105, "Hz", 0);
    display_GB2312_string(7, 1, "ȷ��", 0);
    display_GB2312_string(7, 97, "ȡ��", 1);
}

void mode4(uint8_t* letter, uint8_t* integer, uint8_t* decimal){
    clear_screen();
    display_GB2312_string(3, 1, "�����������Ϸ�", 0);
}

