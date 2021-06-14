//*****************************************************************************
//
// Copyright: 2020-2021, 上海交通大学电子工程系实验教学中心
// File name: ADC_Demo.c
// Description: 该示例展示如何利用AIN2/PE1端口实现单端输入单次ADC采样,采样频率25Hz
//    1.左侧四个数码管显示ADC采样值[0-4095]；
//    2.右侧三个数码管显示电压值[0.00-3.30V]；
//    3.注意：输入电压值范围必须为[0-3.3V]，否则会烧坏端口。
// Author:	上海交通大学电子工程系实验教学中心
// Version: 1.0.0.20210513 
// Date：2021-05-13
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
#include "driverlib/adc.h"        // 与ADC有关的定义 

#include "tm1638.h"               // 与控制TM1638芯片有关的函数
#include "ADC.h"
//*****************************************************************************
//
// 宏定义
//
//*****************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTick频率为50Hz，即循环定时周期20ms

#define V_T40ms	 2              // 40ms软件定时器溢出值，2个20ms

#define V_T100ms 5              // 0.1s软件定时器溢出值，5个20ms
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

// 软件定时器计数
uint8_t clock40ms = 0;
uint8_t clock100ms = 0;

// 软件定时器溢出标志
uint8_t clock40ms_flag = 0;
uint8_t	clock100ms_flag = 0; 

// 8位数码管显示的数字或字母符号
// 注：板上数码位从左到右序号排列为4、5、6、7、0、1、2、3
uint8_t digit[8]={' ',' ',' ',' ',' ',' ',' ',' '};

// 8位小数点 1亮  0灭
// 注：板上数码位小数点从左到右序号排列为4、5、6、7、0、1、2、3
uint8_t pnt = 0x2;

// 8个LED指示灯状态，0灭，1亮
// 注：板上指示灯从左到右序号排列为7、6、5、4、3、2、1、0
//     对应元件LED8、LED7、LED6、LED5、LED4、LED3、LED2、LED1
uint8_t led[] = {1, 0, 0, 0, 0, 0, 0, 0};

// 系统时钟频率 
uint32_t ui32SysClock;

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

	DevicesInit();            //  MCU器件初始化
	
	SysCtlDelay(60 * ( ui32SysClock / 3000));   // 延时>60ms,等待TM1638上电完成
	TM1638_Init();	          // 初始化TM1638
	
	while (1)
	{				
        
        if (clock40ms_flag == 1)             // 检查40ms秒定时是否到
        {
            clock40ms_flag = 0;
            
            ui32ADC0Value = ADC_Sample();   // 采样
			
           	digit[4] = ui32ADC0Value / 1000; 	     // 显示ADC采样值千位数
			digit[5] = ui32ADC0Value / 100 % 10; 	 // 显示ADC采样值百位数
			digit[6] = ui32ADC0Value / 10 % 10; 	 // 显示ADC采样值十位数
			digit[7] = ui32ADC0Value % 10;           // 显示ADC采样值个位数
            
			ui32ADC0Voltage = ui32ADC0Value * 330 / 4095;
            
            digit[1] = (ui32ADC0Voltage / 100) % 10; // 显示电压值个位数
            digit[2] = (ui32ADC0Voltage / 10) % 10;  // 显示电压值十分位数
            digit[3] = ui32ADC0Voltage % 10;         // 显示电压值百分位数             
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
  ADCInit();              // ADC初始化
  SysTickInit();          // 设置SysTick中断
  IntMasterEnable();	  // 总中断允许
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
 
	// 40ms秒钟软定时器计数
	if (++clock40ms >= V_T40ms)
	{
		clock40ms_flag = 1; // 当40ms到时，溢出标志置1
		clock40ms = 0;
	}

    
	// 0.1秒钟软定时器计数
	if (++clock100ms >= V_T100ms)
	{
		clock100ms_flag = 1; // 当0.1秒到时，溢出标志置1
		clock100ms = 0;
	}
	
	// 刷新全部数码管和LED
	TM1638_RefreshDIGIandLED(digit, pnt, led);

}
