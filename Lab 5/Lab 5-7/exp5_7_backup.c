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
#include "define.h"               // 定义NCPDT
#include "inc/hw_memmap.h"        // 基址宏定义
#include "inc/hw_ints.h"
#include "inc/hw_types.h"         // 数据类型宏定义，寄存器访问函数
#include "driverlib/debug.h"      // 调试用
#include "driverlib/gpio.h"       // 通用IO口宏定义
#include "driverlib/pin_map.h"    // TM4C系列MCU外围设备管脚宏定义
#include "driverlib/sysctl.h"	  // 系统控制定义
#include "driverlib/systick.h"    // SysTick Driver 原型
#include "driverlib/interrupt.h"  // NVIC Interrupt Controller Driver 原型
#include "driverlib/uart.h"       // 与UART有关的宏定义和函数原型

#include "tm1638.h"               // 与控制TM1638芯片有关的函数
#include "JLX12864_2.c"
#include "modes.c"
#include "ADC.h"

//*****************************************************************************
//
// 宏定义
//
//*****************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTick频率为50Hz，即循环定时周期20ms

#define V_T40ms	 2              // 40ms软件定时器溢出值，2个20ms
#define V_T100ms	5              // 0.1s软件定时器溢出值，5个20ms
#define V_T500ms	25             // 0.5s软件定时器溢出值，25个20ms
#define V_T2s     100           // 2s软件定时器溢出值，100个20ms
#define V_T5s     250           // 5s软件定时器溢出值
#define V_T10s    500           // 10s软件定时器溢出值
//*****************************************************************************
//
// 函数原型声明
//
//*****************************************************************************
void GPIOInit(void);        // GPIO初始化
void SysTickInit(void);     // 设置SysTick中断 
void DevicesInit(void);     // MCU器件初始化，注：会调用上述函数
void UARTInit(void);        // UART初始化
void UARTStringPut(uint32_t ui32Base,const char *cMessage);// 向UART发送字符串
void UARTStringGet(uint32_t ui32Base, char* s);
void UART6_Handler(void);
void passFreq(uint8_t* f);
//*****************************************************************************
//
// 变量定义
//
//*****************************************************************************
uint8_t sjtu[128*8];
uint8_t bit[8];
// 软件定时器计数
uint8_t clock40ms = 0;
uint8_t clock100ms = 0;
uint8_t clock500ms = 0;
uint8_t clock2s = 0;
uint8_t clock5s = 0;
uint16_t clock10s = 0;

// 软件定时器溢出标志
uint8_t clock100ms_flag = 0;
uint8_t clock500ms_flag = 0;
uint8_t clock2s_flag = 1;
uint8_t clock5s_flag = 0;
uint8_t clock10s_flag = 0;
uint8_t clock40ms_flag = 0;
bool if_count_2s = false;      // 是否开始计数

//LCD屏幕状态机
uint8_t lcd_act = 0;
uint8_t lcd_subact = 0;     //加入子状态
bool if_refresh = true;    // 赋值为true便于初始化时必定切入模式0

// 测试用计数器
uint32_t test_counter = 0;

// 8位数码管显示的数字或字母符号
// 注：板上数码位从左到右序号排列为4、5、6、7、0、1、2、3
uint8_t digit[8]={' ',' ',' ',' ','_',' ','_',' '};

// 8位小数点 1亮  0灭
// 注：板上数码位小数点从左到右序号排列为4、5、6、7、0、1、2、3
uint8_t pnt = 0x01;

// 8个LED指示灯状态，0灭，1亮
// 注：板上指示灯从左到右序号排列为7、6、5、4、3、2、1、0
//     对应元件LED8、LED7、LED6、LED5、LED4、LED3、LED2、LED1
uint8_t led[] = {1, 1, 1, 1, 1, 1, 1, 0};

// 存储接受发送字符串用
char str[50] = {'\0'};

// 当前按键值
uint8_t key_code = 0;

// 系统时钟频率 
uint32_t ui32SysClock;

// 面板参数变量
uint8_t* letter[3]={"A", "B", "C"};
uint8_t* number[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
uint16_t voltage = 0;
uint8_t* v_passin[4] = {0};
uint16_t tempre = 305;
uint8_t* t_passin[3] = {0};
uint8_t freq[4] = {1, 0, 0, 0};
uint8_t freq_temp[4] = {0};    // 用于保存频率参数，若不合法则返回修改前参数
uint8_t* f_passin[4] = {0};

// 定义函数指针数组便于切换模式
void (*func[3][5])(uint8_t* [], uint8_t* [], uint8_t* []) = {0};

// AIN2(PE1)  ADC采样值[0-4095]
uint32_t ui32ADC0Value;

// AIN2电压值(单位为0.01V) [0.00-3.30]
uint32_t ui32ADC0Voltage;

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

    clear_screen();

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

    //将函数装入函数指针数组中
    func[0][0] = mode0_0;
    func[1][0] = mode1_0;
    func[1][1] = mode1_1;
    func[1][2] = mode1_2;
    func[1][3] = mode1_3;
    func[1][4] = mode1_4;
    func[2][0] = mode2_0;

    t_passin[0] = number[tempre / 100];
    t_passin[1] = number[(tempre / 10) % 10];
    t_passin[2] = number[tempre % 10];

    passFreq(freq);

    while (1)
    {
        // 采样电压
        if (clock500ms_flag == 1)             // 检查40ms秒定时是否到
        {
            clock500ms_flag = 0;

            ui32ADC0Value = ADC_Sample();   // 采样
//            ui32ADC0Value = 2000;   // 采样

            digit[4] = ui32ADC0Value / 1000; 	     // 显示ADC采样值千位数
            digit[5] = ui32ADC0Value / 100 % 10; 	 // 显示ADC采样值百位数
            digit[6] = ui32ADC0Value / 10 % 10; 	 // 显示ADC采样值十位数
            digit[7] = ui32ADC0Value % 10;           // 显示ADC采样值个位数

            ui32ADC0Voltage = ui32ADC0Value * 3300 / 4095;

            digit[0] = (ui32ADC0Voltage / 1000) % 10;
            digit[1] = (ui32ADC0Voltage / 100) % 10; // 显示电压值个位数
            digit[2] = (ui32ADC0Voltage / 10) % 10;  // 显示电压值十分位数
            digit[3] = ui32ADC0Voltage % 10;         // 显示电压值百分位数

            v_passin[0] = number[(ui32ADC0Voltage / 1000) % 10]; // 显示电压值个位数
            v_passin[1] = number[(ui32ADC0Voltage / 100) % 10];  // 显示电压值十分位数
            v_passin[2] = number[(ui32ADC0Voltage / 10) % 10];   // 显示电压值百分位数
            v_passin[3] = number[(ui32ADC0Voltage) % 10];        // 显示电压值千分位数
            if (lcd_act == 0 && lcd_subact == 0) if_refresh = true;
        }


        if (clock500ms_flag == 1)   // 检查0.5秒定时是否到
        {
            clock500ms_flag = 0;
            // 8个指示灯以走马灯方式，每0.5秒向右（循环）移动一格
            temp = led[0];
            for (i = 0; i < 7; i++) led[i] = led[i + 1];
            led[7] = temp;
        }

        // 刷新屏幕内容
        if (if_refresh || (if_count_2s && clock2s_flag))
        {
            if (if_count_2s && clock2s_flag){
                lcd_act = 1;
                lcd_subact = 0;
                if_count_2s = false;
            }
            if_refresh = false;

            for (i=0; i<4; ++i) f_passin[i] = number[freq_temp[i]]; // 传递显示频率
            TEST_H;

            func[lcd_act][lcd_subact](v_passin, t_passin, f_passin);    // 根据不同模式显示不同界面

            if (lcd_act == 2 && lcd_subact == 0) {if_count_2s = true; clock2s = 0; clock2s_flag = false;}   // 进入警示界面则开始计时
            else if_count_2s = false;
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
    ADCInit();              // ADC初始化
    UARTInit();             // UART初始化
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
    uint8_t i = 0;

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

    // 5秒钟软定时器计数
    if (++clock5s >= V_T5s){
        clock5s_flag = 1;
        clock5s = 0;
    }

    // 10秒钟软定时器计数
    if (++clock10s >= V_T10s){
        clock10s_flag = 1;
        clock10s = 0;
    }

    if (++clock40ms >= V_T40ms)
    {
        clock40ms_flag = 1; // 当40ms到时，溢出标志置1
        clock40ms = 0;
    }


    // 刷新全部数码管和LED指示灯
    TM1638_RefreshDIGIandLED(digit, pnt, led);

    // 检查当前键盘输入，0代表无键操作，1-9表示有对应按键
    // 键号显示在一位数码管上
    key_code = TM1638_Readkeyboard();


    // 面板参数切换
    if (key_code_pre == 0 && key_code != 0 && lcd_act == 0 && lcd_subact == 0){
        lcd_act = 1;
        lcd_subact = 0;
        for (i =0; i<4; ++i){
            freq_temp[i] = freq[i];
        }   // 保存修改前参数
        if_refresh = true;
        return;
    }

    if (key_code_pre != 8 && key_code == 8){
        if (lcd_act == 1 && lcd_subact == 0){
            freq_temp[0] = (freq_temp[0] + 9) % 10;
            if_refresh = true;
        } else if (lcd_act == 1 && lcd_subact == 1){
            freq_temp[1] = (freq_temp[1] + 9) % 10;
            if_refresh = true;
        } else if (lcd_act == 1 && lcd_subact == 2){
            freq_temp[2] = (freq_temp[2] + 9) % 10;
            if_refresh = true;
        } else if (lcd_act == 1 && lcd_subact == 3){
            freq_temp[3] = (freq_temp[3] + 9) % 10;
            if_refresh = true;
        }
    }

    if (key_code_pre != 2 && key_code == 2){
        if (lcd_act == 1 && lcd_subact == 0){
            freq_temp[0] = (freq_temp[0] + 1) % 10;
            if_refresh = true;
        } else if (lcd_act == 1 && lcd_subact == 1){
            freq_temp[1] = (freq_temp[1] + 1) % 10;
            if_refresh = true;
        } else if (lcd_act == 1 && lcd_subact == 2){
            freq_temp[2] = (freq_temp[2] + 1) % 10;
            if_refresh = true;
        } else if (lcd_act == 1 && lcd_subact == 3){
            freq_temp[3] = (freq_temp[3] + 1) % 10;
            if_refresh = true;
        }       // 可替换成switch, 但懒得换了
    }

    if (key_code_pre != 4 && key_code == 4){
        switch (lcd_act) {
            case 1:
                lcd_subact = (lcd_subact + 4) % 5;
                if_refresh = true;
                break;
        }       // 可替换成if, 但懒得换了
    }

    if (key_code_pre != 6 && key_code == 6){
        switch (lcd_act) {
            case 1:
                lcd_subact = (lcd_subact + 1) % 5;
                if_refresh = true;
                break;
        }
    }

    if (key_code_pre != 5 && key_code == 5 && lcd_act == 1 && lcd_subact == 4){
        uint16_t f_temp = 1000 * freq_temp[0] + 100 * freq_temp[1] + 10 * freq_temp[2] + freq_temp[3];
        if (f_temp >= 880 && f_temp <= 1080){   // 参数合法
            for (i =0; i<4; ++i){
                freq[i] = freq_temp[i];     // 更新参数
            }
            passFreq(freq);             // 参数传至FM接收模块
            lcd_act = 0;
            lcd_subact = 0;
            if_refresh = true;
            clear_screen();
        }
        else {                              // 参数不合法
            for (i =0; i<4; ++i){
                freq_temp[i] = freq[i];     // 还原参数
            }
            lcd_act = 2;
            lcd_subact = 0;
            if_refresh = true;
        }
    }
}


// UART部分

//*****************************************************************************
//
// 函数原型：void UARTInit(void)
// 函数功能：UART初始化。使能UART0，设置PA0,PA1为UART0 RX,TX引脚；
//          设置波特率及帧格式。
// 函数参数：无
// 函数返回值：无
//
//*****************************************************************************
void UARTInit(void)
{
    // 引脚配置
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);        // 使能UART6模块
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);        // 使能端口 P
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOP));	// 等待端口 P准备完毕

    GPIOPinConfigure(GPIO_PP0_U6RX);				  // 设置PP0为UART6 RX引脚
    GPIOPinConfigure(GPIO_PP1_U6TX);    			  // 设置PP1为UART6 TX引脚

    // 设置端口 A的第0,1位（PA0,PA1）为UART引脚
    GPIOPinTypeUART(GPIO_PORTP_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // 波特率及帧格式设置
    UARTConfigSetExpClk(UART6_BASE,
                        ui32SysClock,
                        9600,                  // 波特率：9600
                        (UART_CONFIG_WLEN_8 |    // 数据位：8
                         UART_CONFIG_STOP_ONE |  // 停止位：1
                         UART_CONFIG_PAR_NONE)); // 校验位：无

    IntEnable(INT_UART6); // UART6 中断允许
    UARTIntEnable(UART6_BASE, UART_INT_RX | UART_INT_RT);// 使能 UART6 RX,RT 中断

    // 引脚配置
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);        // 使能UART0模块
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);        // 使能端口 A
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));	// 等待端口 A准备完毕

    GPIOPinConfigure(GPIO_PA0_U0RX);				  // 设置PA0为UART0 RX引脚
    GPIOPinConfigure(GPIO_PA1_U0TX);    			  // 设置PA1为UART0 TX引脚

    // 设置端口 A的第0,1位（PA0,PA1）为UART引脚
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // 波特率及帧格式设置
    UARTConfigSetExpClk(UART0_BASE,
                        ui32SysClock,
                        115200,                  // 波特率：115200
                        (UART_CONFIG_WLEN_8 |    // 数据位：8
                         UART_CONFIG_STOP_ONE |  // 停止位：1
                         UART_CONFIG_PAR_NONE)); // 校验位：无

    // 初始化完成后向PC端发送"Hello, 2A!"字符串
    UARTStringPut(UART0_BASE, (const char *)"Hello, 2A!");

    IntEnable(INT_UART0); // UART0 中断允许
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);// 使能 UART0 RX,RT 中断
}

void UART6_Handler(void)
{
    uint32_t uart6_int_status;

    uart6_int_status = UARTIntStatus(UART6_BASE, true); // 取中断状态
    UARTIntClear(UART6_BASE, uart6_int_status); // 清中断状态

    UARTStringGet(UART6_BASE, str);
    UARTStringPut(UART0_BASE, str);
}

//*****************************************************************************
//
// 函数原型：void UARTStringPut(uint32_t ui32Base,const char *cMessage)
// 函数功能：向UART模块发送字符串
// 函数参数：ui32Base：UART模块
//          cMessage：待发送字符串
// 函数返回值：无
//
//*****************************************************************************
void UARTStringPut(uint32_t ui32Base,const char *cMessage)
{
    while(*cMessage != '\0') {
        UARTCharPut(ui32Base, *(cMessage++));
        SysCtlDelay(1 * ( ui32SysClock / 3000)); //每读1字符延时1ms
    }
//    UARTCharPut(ui32Base, '\r');
//    UARTCharPut(ui32Base, '\n');
}

//*****************************************************************************
//
// 函数原型：void UARTStringGet(uint32_t ui32Base,const char *cMessage)
// 函数功能：UART模块接收字符串
// 函数参数：ui32Base：UART模块
//          str：待接收字符串
// 函数返回值：无
//
//*****************************************************************************
void UARTStringGet(uint32_t ui32Base, char* s) {
//    char* temp = s;
    while (UARTCharsAvail(ui32Base)) // 重复从接收FIF0读取字符
    {
        *s = (char) UARTCharGetNonBlocking(ui32Base); // 读入一个字符
        ++s;
        SysCtlDelay(1 * ( ui32SysClock / 3000)); //每读1字符延时1ms
    }
    *s = '\0';
//    UARTStringPut(UART0_BASE, (const char *)"Get String:");
//    UARTStringPut(UART0_BASE, temp);
}

void passFreq(uint8_t* f){
    UARTStringPut(UART6_BASE, "AT+FREQ=");
    if (f[0] == 0){
        UARTCharPut(UART6_BASE,f[1]+'0');
        UARTCharPut(UART6_BASE,f[2]+'0');
        UARTCharPut(UART6_BASE,f[3]+'0');
    }
    else {
        UARTCharPut(UART6_BASE,f[0]+'0');
        UARTCharPut(UART6_BASE,f[1]+'0');
        UARTCharPut(UART6_BASE,f[2]+'0');
        UARTCharPut(UART6_BASE,f[3]+'0');
    }
}