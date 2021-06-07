//*****************************************************************************
//
// PWM.h - Prototypes for the PWM driver.
//
// Copyright：2020-2021,上海交通大学电子工程系实验教学中心
// 
// Author: 上海交通大学电子工程系实验教学中心
// Version: 1.0.0.20210508 
// Date：2021-05-08
// History：
//
//*****************************************************************************

#ifndef __PWM_H__
#define __PWM_H__

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
#include "inc/hw_i2c.h"
#include "driverlib/debug.h"      // 调试用
#include "driverlib/gpio.h"       // 通用IO口宏定义
#include "driverlib/pin_map.h"    // TM4C系列MCU外围设备管脚宏定义
#include "driverlib/sysctl.h"	  // 系统控制定义
#include "driverlib/i2c.h"
#include "driverlib/pwm.h"        // 与Timer有关的函数原型

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************

// M0PWM4(PG0)初始化
extern void PWMInit(void);

// 产生频率为ui32Freq_Hz的方波
extern void PWMStart(uint32_t ui32Freq_Hz);

// M0PWM4(PG0)停止产生PWM信号
extern void PWMStop(void);
//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __PWM_H__
