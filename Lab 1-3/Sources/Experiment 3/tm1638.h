//*****************************************************************************
//
// tm1638.h - Prototypes for the TM1638 driver.
//
// Copyright：2020-2021, 上海交通大学电子工程系实验教学中心
// 
// Author:	上海交通大学电子工程系实验教学中心
// Version: 1.0.0.20201228 
// Date：2020-12-28
// History：
//
//*****************************************************************************

#ifndef __TM1638_H__
#define __TM1638_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"        // 基址宏定义
#include "inc/hw_types.h"         // 数据类型宏定义，寄存器访问函数
#include "driverlib/debug.h"      // 调试用
#include "driverlib/gpio.h"       // 通用IO口宏定义
#include "driverlib/pin_map.h"    // TM4C系列MCU外围设备管脚宏定义
#include "driverlib/sysctl.h"			// 系统控制定义


// 用于连接TM1638的TM4C1294 GPIO引脚定义
#define DIO_PIN_BASE  GPIO_PORTK_BASE
#define DIO_PIN       GPIO_PIN_5          // DIO <--> PK5
#define STB_PIN_BASE  GPIO_PORTK_BASE
#define STB_PIN       GPIO_PIN_4          // STB <--> PK4
#define CLK_PIN_BASE  GPIO_PORTM_BASE       
#define CLK_PIN       GPIO_PIN_0          // CLK <--> PM0
	
//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
	
// 将显示数字或符号转换为共阴数码管的笔画值
extern uint8_t TM1638_DigiSegment(uint8_t digit);

// TM1638串行数据输入
extern void TM1638_Serial_Input(uint8_t data);

// TM1638串行数据输出
extern uint8_t TM1638_Serial_Output(void);

// 读取键盘当前状态
extern uint8_t TM1638_Readkeyboard(void);

// 刷新8位数码管（含小数点）和8组指示灯（每组2只，有4种亮灯模式）
extern void TM1638_RefreshDIGIandLED(uint8_t digit_buf[8],
                                     uint8_t pnt_buf,
                                     uint8_t led_buf[8]);

// TM1638初始化函数
extern void TM1638_Init(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __TM1638_H__
