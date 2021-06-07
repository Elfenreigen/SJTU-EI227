//*****************************************************************************
//
// Copyright: 2020-2021, 上海交通大学电子工程系实验教学中心
// File name: exp3_0.c
// Description:
// 本程序通过A2000TM4底板按键阵列控制UART端口的数据读写，并进行字符的大小写转换。
// 1.开机或复位后，底板上左边第2个数码管显示0，LED8点亮，系统工作在默认状态（状态0）
// 2.系统启动后，会处于以下4种状态：
//    1)状态0：默认状态或按下编号为4-9按键后进入。此时PC端发送的字符，实验板收到后
//            原样返回。底板上左边第2个数码管显示0，LED8点亮。
//    2)状态1：按下编号为1的按键后进入。此时PC端如果发送的是小写字符'a'-'z',实验板
//            收到后转换为大写'A'-'Z'后返回，其他字符则原样返回。底板上左边第2个
//            数码管显示1，LED7点亮。
//    3)状态2：按下编号为2的按键后进入。此时PC端如果发送的是大写字符'A'-'Z',实验板
//            收到后转换为小写'a'-'z'后返回，其他字符则原样返回。底板上左边第2个
//            数码管显示2，LED6点亮。
//    4)状态3：按下编号为3的按键后进入。此时PC端如果发送的是大写字符'A'-'Z',实验板
//            收到后转换为小写'a'-'z'后返回；如果发送的是小写字符'a'-'z',实验板
//            收到后转换为大写'A'-'Z'后返回；其他字符则原样返回。底板上左边第2个
//            数码管显示3，LED5点亮。
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
#include "driverlib/gpio.h"       // 通用IO口宏定义和函数原型
#include "driverlib/pin_map.h"    // TM4C系列MCU外围设备管脚宏定义
#include "driverlib/sysctl.h"	  // 系统控制定义
#include "driverlib/systick.h"    // SysTick Driver 原型
#include "driverlib/interrupt.h"  // NVIC Interrupt Controller Driver 原型
#include "driverlib/uart.h"       // 与UART有关的宏定义和函数原型
#include "inc/hw_ints.h"
#include "tm1638.h"               // 与控制TM1638芯片有关的宏定义和函数原型

//*****************************************************************************
//
// 宏定义
//
//*****************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTick频率为50Hz，即循环定时周期20ms

#define V_T100ms	5                  // 0.1s软件定时器溢出值，5个20ms
#define V_T1000ms	50
//*****************************************************************************
//
// 函数原型声明
//
//*****************************************************************************
void GPIOInit(void);        // GPIO初始化
void SysTickInit(void);     // 设置SysTick中断
void UARTInit(void);        // UART初始化
void DevicesInit(void);     // MCU器件初始化，注：会调用上述函数
void UARTStringPut(uint32_t ui32Base,const char *cMessage);// 向UART发送字符串
//*****************************************************************************
//
// 变量定义
//
//*****************************************************************************

// 8位数码管显示的数字或字母符号
// 注：板上数码位从左到右序号排列为4、5、6、7、0、1、2、3
uint8_t digit[8]= {0,0,' ',' ',0,0,0,0};

// 8位小数点 1亮  0灭
// 注：板上数码位小数点从左到右序号排列为4、5、6、7、0、1、2、3
uint8_t pnt = 0xA0;

// 8个LED指示灯状态，0灭，1亮
// 注：板上指示灯从左到右序号排列为7、6、5、4、3、2、1、0
//     对应元件LED8、LED7、LED6、LED5、LED4、LED3、LED2、LED1
uint8_t led[] = {0, 0, 0, 0, 0, 0, 0, 1};

// 当前按键值
volatile uint8_t key_code = 0;

// 用于记录前一次按键检测时的键盘状态，0表示无键按下，1有键按下
volatile uint8_t key_state = 0;

// 记录系统运行状态
volatile uint8_t  run_state = 0;

// 系统时钟频率
uint32_t ui32SysClock;

uint8_t clock1000ms = 0;
uint8_t clock1000ms_flag=0;
// 测试用计数器
uint32_t second_counter = 0;
uint32_t minute_counter = 0;
uint32_t hour_counter = 0;
//*****************************************************************************
//
// 主程序
//
//*****************************************************************************
int main(void)
{
    DevicesInit();            //  MCU器件初始化
    while(clock1000ms < 3) {};
    TM1638_Init();	          // 初始化TM1638
		while(clock1000ms < 6) {};
			
    UARTCharPut(UART6_BASE,'A');
    UARTCharPut(UART6_BASE,'T');
    UARTCharPut(UART6_BASE,'+');
    UARTCharPut(UART6_BASE,'F');
    UARTCharPut(UART6_BASE,'R');
    UARTCharPut(UART6_BASE,'E');
		UARTCharPut(UART6_BASE,'Q');
    UARTCharPut(UART6_BASE,'=');
    UARTCharPut(UART6_BASE,'8');
    UARTCharPut(UART6_BASE,'8');
    UARTCharPut(UART6_BASE,'6');
			
		while(clock1000ms < 10) {};
			
		UARTCharPut(UART6_BASE,'A');
    UARTCharPut(UART6_BASE,'T');
    UARTCharPut(UART6_BASE,'+');
    UARTCharPut(UART6_BASE,'V');
    UARTCharPut(UART6_BASE,'O');
    UARTCharPut(UART6_BASE,'L');
    UARTCharPut(UART6_BASE,'=');
    UARTCharPut(UART6_BASE,'1');
    UARTCharPut(UART6_BASE,'5');

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
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);				// 使能端口 K
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)) {};		// 等待端口 K准备完毕

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);				// 使能端口 M
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)) {};		// 等待端口 M准备完毕

    // 设置端口 K的第4,5位（PK4,PK5）为输出引脚		PK4-STB  PK5-DIO
    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_4|GPIO_PIN_5);
    // 设置端口 M的第0位（PM0）为输出引脚   PM0-CLK
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_0);
}

void SysTickInit(void)
{
    SysTickPeriodSet(ui32SysClock/SYSTICK_FREQUENCY); // 设置心跳节拍,定时周期20ms
    SysTickEnable();  			// SysTick使能
    SysTickIntEnable();			// SysTick中断允许
}


void UARTInit(void)
{
// SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);        // ??UART0??
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);        // ???? A
//    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));	// ???? A????

//    GPIOPinConfigure(GPIO_PA0_U0RX);				  // ??PA0?UART0 RX??
//    GPIOPinConfigure(GPIO_PA1_U0TX);    			  // ??PA1?UART0 TX??

//	// ???? A??0,1?(PA0,PA1)?UART??
//    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

//    // ?????????
//    UARTConfigSetExpClk(UART0_BASE,
//	                    ui32SysClock,
//	                    115200,                  // ???:115200
//	                    (UART_CONFIG_WLEN_8 |    // ???:8
//	                     UART_CONFIG_STOP_ONE |  // ???:1
//	                     UART_CONFIG_PAR_NONE)); // ???:?



    // 引脚配置
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);        // 使能UART6模块
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);        // 使能端口 P
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOP));	// 等待端口 P准备完毕

    GPIOPinConfigure(GPIO_PP0_U6RX);				  // 设置PD4为UART2 RX引脚
    GPIOPinConfigure(GPIO_PP1_U6TX);    			  // 设置PD5为UART2 TX引脚

    // 设置端口 P的第4,5位（PP0,PP1）为UART引脚
    GPIOPinTypeUART(GPIO_PORTP_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // 波特率及帧格式设置
    UARTConfigSetExpClk(UART6_BASE,
                        ui32SysClock,
                        9600,                  // 波特率：115200
                        (UART_CONFIG_WLEN_8 |    // 数据位：8
                         UART_CONFIG_STOP_ONE |  // 停止位：1
                         UART_CONFIG_PAR_NONE)); // 校验位：无
    UARTFIFOLevelSet(UART6_BASE,UART_FIFO_TX1_8,UART_FIFO_RX7_8);

    IntEnable(INT_UART6); // UART0 中断允许
    UARTIntEnable(UART6_BASE, UART_INT_RX | UART_INT_RT);// 使能 UART6 RX,RT 中断

    // 初始化完成后向PC端发送"Hello, 2A!"字符串
    //UARTStringPut(UART0_BASE, (const char *)"\r\nHello, 2A!\r\n");
}


void DevicesInit(void)
{
    // 使用外部25MHz主时钟源，经过PLL，然后分频为20MHz
    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |
                                       SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480),
                                      20000000);

    GPIOInit();             // GPIO初始化
    UARTInit();             // UART初始化
    SysTickInit();          // 设置SysTick中断
    IntMasterEnable();			// 总中断允许
}




void SysTick_Handler(void)       // 定时周期为20ms
{

    if (++clock1000ms >=V_T1000ms)
    {

        clock1000ms_flag = 1; // 当0.1秒到时，溢出标志置1
        clock1000ms = 0;
    }

    TM1638_RefreshDIGIandLED(digit, pnt, led);

}

void UARTStringPut(uint32_t ui32Base,const char *cMessage)
{
    while(*cMessage != '\0') // 重复从接收 FIFO 读取字符
        UARTCharPut(ui32Base, *(cMessage++));
}


void UART6_Handler(void)
{
    int32_t uart6_int_status;
    uint8_t uart_receive_char;
    uint8_t i=0;
    uart6_int_status = UARTIntStatus(UART6_BASE, true); // 取中断状态
    UARTIntClear(UART6_BASE, uart6_int_status); // 清中断标志
    while(UARTCharsAvail(UART6_BASE)) // 重复从接收 FIFO 读取字符
    {
        uart_receive_char=UARTCharGetNonBlocking(UART6_BASE);
    }

}
