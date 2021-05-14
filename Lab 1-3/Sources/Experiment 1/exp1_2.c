//**************************************************************************************
//
// Copyright: 2020-2021, 上海交通大学电子工程系实验教学中心
// File name: exp0.c
// Description: LED4(D4-PF0)大约以1000毫秒为周期缓慢闪烁；
//              当按下PUSH1(USR_SW1-PJ0)键，LED4(D4-PF0)大约以100毫秒为周期快速闪烁；
//              松开PUSH1(USR_SW1-PJ0)键，LED4(D4-PF0)恢复以1000毫秒为周期缓慢闪烁。
// Author:	上海交通大学电子工程系实验教学中心
// Version: 1.0.0.20201228 
// Date：2020-12-28
// History：
//       
//**************************************************************************************

//**************************************************************************************
//
// 头文件
//
//**************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"        // 基址宏定义
#include "inc/hw_types.h"         // 数据类型宏定义，寄存器访问函数
#include "driverlib/debug.h"      // 调试用
#include "driverlib/gpio.h"       // 通用IO口宏定义
#include "driverlib/pin_map.h"    // TM4C系列MCU外围设备管脚宏定义
#include "driverlib/sysctl.h"     // 系统控制宏定义

//**************************************************************************************
//
// 宏定义
//
//**************************************************************************************

//**************************************************************************************
//
// 函数原型声明
//
//**************************************************************************************
void  GPIOInit(void);                               // GPIO初始化

uint32_t g_ui32SysClock;
//**************************************************************************************
//
// 主程序
//
//**************************************************************************************
int main(void)
{
    uint8_t ui8KeyValue;

    g_ui32SysClock=SysCtlClockFreqSet(SYSCTL_OSC_INT,16000000);
	
    GPIOInit();             // GPIO初始化     
    while(true)
{
     if(!GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0))
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
     else
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);

     if(!GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1))
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
		 else
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);

}
}

//**************************************************************************************

// 函数原型：void GPIOInit(void)
// 函数功能：GPIO初始化。使能PortF，设置PF0为输出；使能PortJ，设置PJ0为输入
// 函数参数：无
//
//**************************************************************************************
void GPIOInit(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);		   // 使能端口 N
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));	   // 等待端口 N准备完毕
		
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);		   // 使能端口 J	
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)){};  // 等待端口 J准备完毕
	
    // 设置端口 N的第0、1位（PN0、PN1）为输出引脚
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0|GPIO_PIN_1); 
	
    // 设置端口 J的第0、1位（PJ0、PJ1）为输入引脚
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE,GPIO_PIN_0|GPIO_PIN_1);
    
    // 端口 J的第0、1位作为按键输入，类型设置成“推挽上拉”
    GPIOPadConfigSet(GPIO_PORTJ_BASE,GPIO_PIN_0|GPIO_PIN_1,GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);       
}

//**************************************************************************************