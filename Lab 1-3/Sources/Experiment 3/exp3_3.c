//*****************************************************************************
//
// 头文件
//
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"        // 基址宏定义
#include "inc/hw_ints.h"
#include "inc/hw_types.h"         // 数据类型宏定义，寄存器访问函数
#include "driverlib/debug.h"      // 调试用
#include "driverlib/gpio.h"       // 通用IO口宏定义和函数原型
#include "driverlib/pin_map.h"    // TM4C系列MCU外围设备管脚宏定义
#include "driverlib/sysctl.h"	  // 系统控制定义
#include "driverlib/systick.h"    // SysTick Driver 原型
#include "driverlib/interrupt.h"  // NVIC Interrupt Controller Driver 原型
#include "driverlib/uart.h"       // 与UART有关的宏定义和函数原型

#include "tm1638.h"               // 与控制TM1638芯片有关的宏定义和函数原型

//*****************************************************************************
//
// 宏定义
//
//*****************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTick频率为50Hz，即循环定时周期20ms

#define V_T1000ms	50                  // 1s软件定时器溢出值，50个20ms

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
//*****************************************************************************
//
// 变量定义
//
//*****************************************************************************

// 8位数码管显示的数字或字母符号
// 注：板上数码位从左到右序号排列为4、5、6、7、0、1、2、3
uint8_t digit[8]={' ',' ',' ',' ',' ',' ',' ',' '};

// 8位小数点 1亮  0灭
// 注：板上数码位小数点从左到右序号排列为4、5、6、7、0、1、2、3
uint8_t pnt = 0xa0;

// 8个LED指示灯状态，0灭，1亮
// 注：板上指示灯从左到右序号排列为7、6、5、4、3、2、1、0
//     对应元件LED8、LED7、LED6、LED5、LED4、LED3、LED2、LED1
uint8_t led[] = {0, 0, 0, 0, 0, 0, 0, 0};


// 系统时钟频率 
uint32_t ui32SysClock;

// 软件定时器计数
uint8_t clock1000ms = 0;

// 软件定时器溢出标志
uint8_t clock1000ms_flag = 0;

// 测试用计数器
uint32_t test_counter = 0;

uint8_t hour = 0;
uint8_t minute = 0;
uint8_t second = 0;
//*****************************************************************************
//
// 主程序
//
//*****************************************************************************
int main(void)
{
    

    DevicesInit();            //  MCU器件初始化
	
    SysCtlDelay(60 * ( ui32SysClock / 3000)); // 延时>60ms,等待TM1638上电完成
    TM1638_Init();	          // 初始化TM1638
	
	
	  digit[4] = 0;
	  digit[5] = 0;
	  digit[6] = 0;
   	digit[7] = 0;
  	digit[0] = 0;
	  digit[1] = 0;
	
    while (1)
			{
				if (clock1000ms_flag == 1)      // 检查1秒定时是否到
		{
			clock1000ms_flag		= 0;
			// 每1秒累加计时值在数码管上以十进制显示
				if (++test_counter >= 86400) test_counter = 0;
				hour=test_counter /3600;
				minute=test_counter % 3600 / 60;
				second=test_counter % 3600 % 60;
				digit[4] = hour / 10; 	    // 计算小时
				digit[5] = hour % 10; 	
				digit[6] = minute / 10; 	  // 计算分钟
				digit[7] = minute % 10;       
				digit[0] = second / 10;     // 计算秒钟
			  digit[1] = second % 10;
			  UARTStringPut(UART0_BASE, (const char *)"\r\n现在是中华人民共和国北京时间");
				UARTCharPut(UART0_BASE,(char)(digit[4]+'0'));
				UARTCharPut(UART0_BASE,(char)(digit[5]+'0'));
				UARTCharPut(UART0_BASE,':');
				UARTCharPut(UART0_BASE,(char)(digit[6]+'0'));
				UARTCharPut(UART0_BASE,(char)(digit[7]+'0'));
				UARTCharPut(UART0_BASE,':');
				UARTCharPut(UART0_BASE,(char)(digit[0]+'0'));
				UARTCharPut(UART0_BASE,(char)(digit[1]+'0'));
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
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);				// 使能端口 K	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)){};		// 等待端口 K准备完毕		
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);				// 使能端口 M	
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
// 函数原型：void UARTStringPut(uint32_t ui32Base,const char *cMessage)
// 函数功能：向UART模块发送字符串
// 函数参数：ui32Base：UART模块
//          cMessage：待发送字符串  
// 函数返回值：无
//
//*****************************************************************************
void UARTStringPut(uint32_t ui32Base,const char *cMessage)
{
	while(*cMessage != '\0')
		UARTCharPut(ui32Base, *(cMessage++));
}

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
    UARTStringPut(UART0_BASE, (const char *)"\r\nHello, 2A!\r\n");	
		
		IntEnable(INT_UART0); // UART0 中断允许
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);// 使能 UART0 RX,RT 中断
}

//*****************************************************************************
// 
// 函数原型：DevicesInit(void)
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
    UARTInit();             // UART初始化 
    IntMasterEnable();			// 总中断允许
}

void SysTick_Handler(void)       // 定时周期为20ms
{
	// 1秒钟软定时器计数
	if (++clock1000ms >= V_T1000ms)
	{
		clock1000ms_flag = 1; // 当1秒到时，溢出标志置1
		clock1000ms = 0;
	}
	
 	
	// 刷新全部数码管和LED指示灯
	TM1638_RefreshDIGIandLED(digit, pnt, led);



}








