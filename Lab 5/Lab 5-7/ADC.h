//*****************************************************************************
//
// ADC.h - Prototypes for the ADC module.
//
// Copyright：2020-2021,上海交通大学电子工程系实验教学中心
// 
// Author: 上海交通大学电子工程系实验教学中心
// Version: 1.0.0.20210508 
// Date：2021-05-08
// History：
//
//*****************************************************************************

#ifndef __ADC_H__
#define __ADC_H__

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
#include "driverlib/sysctl.h"	  // 系统控制定义
#include "driverlib/systick.h"    // SysTick Driver 原型
#include "driverlib/interrupt.h"  // NVIC Interrupt Controller Driver 原型
#include "driverlib/adc.h"        // 与ADC有关的定义 

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************

// ADC初始化
extern void ADCInit(void);         

// 获取ADC采样值
extern uint32_t ADC_Sample(void);   

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __ADC_H__
