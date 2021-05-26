//*****************************************************************************
//
// Copyright: 2020-2021, 上海交通大学电子工程系实验教学中心
// File name: exp2_0.c
// Description: 
//    1.开机或复位后，底板上右边4位数码管自动显示计时数值，最低位对应单位是0.1秒；
//    2.开机或复位后，底板上8个LED灯以跑马灯形式由左向右循环变换，约0.5秒变换1次；
//    3.当没有按键按下时，从左边数第二位数码管显示“0”；
//      当人工按下某键，数码管显示该键的编号；
//      此刻四位计时数码管暂停变化，停止计时，直到放开按键后自动继续计时。
// Author:	上海交通大学电子工程系实验教学中心
// Version: 1.0.0.20201228 
// Date：2020-12-28
// History：
//
//*****************************************************************************

//*****************************************************************************
//
// 头文件
//
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"        // 基址宏定义
#include "inc/hw_types.h"         // 数据类型宏定义，寄存器访问函数
#include "driverlib/debug.h"      // 调试用
#include "driverlib/gpio.h"       // 通用IO口宏定义
#include "driverlib/pin_map.h"    // TM4C系列MCU外围设备管脚宏定义
#include "driverlib/sysctl.h"	  // 系统控制定义
#include "driverlib/systick.h"    // SysTick Driver 原型
#include "driverlib/interrupt.h"  // NVIC Interrupt Controller Driver 原型

#include "tm1638.h"               // 与控制TM1638芯片有关的函数
#include "JLX12864_2.c"
#include "modes_2.c"

//*****************************************************************************
//
// 宏定义
//
//*****************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTick频率为50Hz，即循环定时周期20ms

#define V_T100ms	5              // 0.1s软件定时器溢出值，5个20ms
#define V_T500ms	25             // 0.5s软件定时器溢出值，25个20ms
#define V_T2s     100            //2s软件定时器溢出值，100个20ms
#define V_T10s    500           //10s软件定时器溢出值
//*****************************************************************************
//
// 函数原型声明
//
//*****************************************************************************
void GPIOInit(void);        // GPIO初始化
void SysTickInit(void);     // 设置SysTick中断 
void DevicesInit(void);     // MCU器件初始化，注：会调用上述函数
//*****************************************************************************
//
// 变量定义
//
//*****************************************************************************
uint8_t sjtu[128*8];
uint8_t bit[8];
// 软件定时器计数
uint8_t clock100ms = 0;
uint8_t clock500ms = 0;
uint8_t clock2s = 0;
uint16_t clock10s = 0;

// 软件定时器溢出标志
uint8_t clock100ms_flag = 0;
uint8_t clock500ms_flag = 0;
uint8_t clock2s_flag = 1;
uint8_t clock10s_flag = 0;

//LCD屏幕状态机
uint8_t lcd_act = 0;
bool if_refresh = true;    // 赋值为true便于初始化时必定切入模式0

// 测试用计数器
uint32_t test_counter = 0;

// 8位数码管显示的数字或字母符号
// 注：板上数码位从左到右序号排列为4、5、6、7、0、1、2、3
uint8_t digit[8]={' ',' ',' ',' ','_',' ','_',' '};

// 8位小数点 1亮  0灭
// 注：板上数码位小数点从左到右序号排列为4、5、6、7、0、1、2、3
uint8_t pnt = 0x04;

// 8个LED指示灯状态，0灭，1亮
// 注：板上指示灯从左到右序号排列为7、6、5、4、3、2、1、0
//     对应元件LED8、LED7、LED6、LED5、LED4、LED3、LED2、LED1
uint8_t led[] = {1, 1, 1, 1, 1, 1, 1, 0};

// 当前按键值
uint8_t key_code = 0;

// 系统时钟频率 
uint32_t ui32SysClock;

// 面板参数变量
uint8_t* letter[3]={"A", "B", "C"};
uint8_t* number[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
uint8_t let = 0;
uint8_t integer = 0;
uint8_t decimal = 0;

//*****************************************************************************
//
// 主程序
//
//*****************************************************************************
int main(void)
{
    uint8_t temp,i;
    uint8_t row;
    uint8_t line;


    DevicesInit();            //  MCU器件初始化

    while (clock100ms < 3);   // 延时>60ms,等待TM1638上电完成
    TM1638_Init();	          // 初始化TM1638
    initial_lcd();

    for (row=0;row<128;++row)
    {
        for(line=0;line<8;++line)
        {
            sjtu[line*128+127-row]=~bmp1[row*8+line];
            bit[0]=sjtu[line*128+127-row]%2;
            bit[1]=sjtu[line*128+127-row]%4/2;
            bit[2]=sjtu[line*128+127-row]%8/4;
            bit[3]=sjtu[line*128+127-row]%16/8;
            bit[4]=sjtu[line*128+127-row]%32/16;
            bit[5]=sjtu[line*128+127-row]%64/32;
            bit[6]=sjtu[line*128+127-row]%128/64;
            bit[7]=sjtu[line*128+127-row]/128;
            sjtu[line*128+127-row]=bit[0]*128+bit[1]*64+bit[2]*32+bit[3]*16+bit[4]*8+bit[5]*4+bit[6]*2+bit[7];
        }
    }

    clear_screen();

    while (1)
    {

        if (clock100ms_flag == 1)      // 检查0.1秒定时是否到
        {
            clock100ms_flag		= 0;
            // 每0.1秒累加计时值在数码管上以十进制显示，有键按下时暂停计时
            if (key_code == 0)
            {
                if (++test_counter >= 10000) test_counter = 0;
                digit[0] = test_counter / 1000; 	    // 计算百位数
                digit[1] = test_counter / 100 % 10; 	// 计算十位数
                digit[2] = test_counter / 10 % 10; 	  // 计算个位数
                digit[3] = test_counter % 10;         // 计算百分位数
            }
        }

        if (clock500ms_flag == 1)   // 检查0.5秒定时是否到
        {
            clock500ms_flag = 0;
            // 8个指示灯以走马灯方式，每0.5秒向右（循环）移动一格
            temp = led[0];
            for (i = 0; i < 7; i++) led[i] = led[i + 1];
            led[7] = temp;
        }


        if (if_refresh || clock10s_flag == 1)     // 屏幕状态变化或满10s时切换模式
        {
            if_refresh = false;
            if (clock10s_flag == 1) {lcd_act = 0; clock10s_flag = 0;}   //满10s转回模式0
            clock10s = 0;   // 重新计时
            TEST_H;
            switch(lcd_act)
            {
                case 0:
                    mode0(letter[let], number[integer],number[decimal]);
                    break;
                case 1:
                    mode1(letter[let], number[integer],number[decimal]);
                    break;
                case 2:
                    mode2(letter[let], number[integer],number[decimal]);
                    break;
                case 3:
                    mode3(letter[let], number[integer],number[decimal]);
                    break;
                default: break;
            }
            TEST_L;
        }

    }

}

//*****************************************************************************
//
// 函数原型：void GPIOInit(void)
// 函数功能：GPIO初始化。使能PortK，设置PK4,PK5为输出；使能PortM，设置PM0为输出。
//          （PK4连接TM1638的STB，PK5连接TM1638的DIO，PM0连接TM1638的CLK）
// 函数参数：无
// 函数返回值：无
//
//*****************************************************************************
void GPIOInit(void)
{
    //配置TM1638芯片管脚
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);						// 使能端口 K
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)){};		// 等待端口 K准备完毕

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);						// 使能端口 M
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)){};		// 等待端口 M准备完毕

    // 设置端口 K的第4,5位（PK4,PK5）为输出引脚		PK4-STB  PK5-DIO
    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_4|GPIO_PIN_5);
    // 设置端口 M的第0位（PM0）为输出引脚   PM0-CLK
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_0);
}

//*****************************************************************************
// 
// 函数原型：SysTickInit(void)
// 函数功能：设置SysTick中断
// 函数参数：无
// 函数返回值：无
//
//*****************************************************************************
void SysTickInit(void)
{
    SysTickPeriodSet(ui32SysClock/SYSTICK_FREQUENCY); // 设置心跳节拍,定时周期20ms
    SysTickEnable();  			// SysTick使能
    SysTickIntEnable();			// SysTick中断允许
}

//*****************************************************************************
// 
// 函数原型：void DevicesInit(void)
// 函数功能：CU器件初始化，包括系统时钟设置、GPIO初始化和SysTick中断设置
// 函数参数：无
// 函数返回值：无
//
//*****************************************************************************
void DevicesInit(void)
{
    // 使用外部25MHz主时钟源，经过PLL，然后分频为20MHz
    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |
                                       SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480),
                                      20000000);

    GPIOInit();             // GPIO初始化
    SysTickInit();          // 设置SysTick中断
    IntMasterEnable();			// 总中断允许
}

//*****************************************************************************
// 
// 函数原型：void SysTick_Handler(void)
// 函数功能：SysTick中断服务程序
// 函数参数：无
// 函数返回值：无
//
//*****************************************************************************
void SysTick_Handler(void)       // 定时周期为20ms
{
    uint8_t key_code_pre = key_code;    // 保存前一次按键状态

    // 0.1秒钟软定时器计数
    if (++clock100ms >= V_T100ms)
    {
        clock100ms_flag = 1; // 当0.1秒到时，溢出标志置1
        clock100ms = 0;
    }

    // 0.5秒钟软定时器计数
    if (++clock500ms >= V_T500ms)
    {
        clock500ms_flag = 1; // 当0.5秒到时，溢出标志置1
        clock500ms = 0;
    }

    // 2秒钟软定时器计数
    if (++clock2s >= V_T2s)
    {
        clock2s_flag = 1; // 当2秒到时，溢出标志置1
        clock2s = 0;
    }

    // 10秒钟软定时器计数
    if (++clock10s >= V_T10s){
        clock10s_flag = 1;
        clock10s = 0;
    }

    // 刷新全部数码管和LED指示灯
    TM1638_RefreshDIGIandLED(digit, pnt, led);

    // 检查当前键盘输入，0代表无键操作，1-9表示有对应按键
    // 键号显示在一位数码管上
    key_code = TM1638_Readkeyboard();

    digit[5] = key_code;

    // 键值判断与模式转换:
    // 0:无选中
    // 1:选中模式栏
    // 2:选中整数位
    // 3:选中小数位

    // 模式0时任意按键切换至1
    if (lcd_act == 0 && key_code != 0) {
        ++lcd_act;
        if_refresh = true;
        return;
    }
    // 减模式, 1-3循环
    if (key_code_pre != 4 && key_code == 4) {
        lcd_act = (--lcd_act + 2) % 3 + 1;
        if_refresh = true;
        return;
    }
    // 加模式, 1-3循环
    if (key_code_pre != 6 && key_code == 6) {
        lcd_act = (--lcd_act + 1) % 3 + 1;
        if_refresh = true;
        return;
    }

    // 面板参数切换

    if (key_code_pre != 8 && key_code == 8){
        switch (lcd_act) {
            case 1: let = (let + 2) % 3; break;
            case 2: integer = (integer + 9) % 10; break;
            case 3: decimal = (decimal + 9) % 10; break;
        }
        if_refresh = true;
    }
    if (key_code_pre != 2 && key_code == 2){
        switch (lcd_act) {
            case 1: let = ++let % 3; break;
            case 2: integer = ++integer % 10; break;
            case 3: decimal = ++ decimal % 10; break;
        }
        if_refresh = true;
    }

}
