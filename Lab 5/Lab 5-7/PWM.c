//*****************************************************************************
//
// PWM.c - API for PWM.
//
// Copyright：2020-2021,上海交通大学电子工程系实验教学中心
// 
// Author:	上海交通大学电子工程系实验教学中心
// Version: 1.0.0.20210508 
// Date：2021-05-08
// History：
//
//*****************************************************************************

#include "PWM.h"
extern uint32_t g_ui32SysClock;    // 系统时钟

//*******************************************************************************************************
// 
// 函数原型：void PWMInit()
// 函数功能：配置引脚PG0使用复用功能M0PWM4
// 函数参数：无
// 函数返回值：无
//
//*******************************************************************************************************
void PWMInit()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);     // PWM0使能   
    
    PWMOutputState(PWM0_BASE, PWM_OUT_4_BIT, true); // 使能(允许)PWM0_4的输出
    //PWMGenEnable(PWM0_BASE, PWM_GEN_2);             //使能PWM0模块的2号发生器(因为4号PWM是2号发生器产生的)
    //PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, g_ui32SysClock / ui32Freq_Hz); // 根据Freq_Hz设置PWM周期
   
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);    // 使能GPIOG
    GPIOPinConfigure(GPIO_PG0_M0PWM4);              // 配置引脚复用
    GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_0);    // 引脚映射
    
    PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);   //配置PWM发生器
    //PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4,(PWMGenPeriodGet(PWM0_BASE, PWM_GEN_2)/ 2)); //设置占空比为50%
}

//*******************************************************************************************************
// 
// 函数原型：void PWMStart(uint32_t ui32Freq_Hz)
// 函数功能：产生频率为ui32Freq_Hz的方波(占空比为50%的PWM)，输出引脚为M0PWM4(PG0)
//          该函数是为了方便用户没有信号发生器时产生测试信号而编写的。
// 函数参数：ui32Freq_Hz 需要产生的方波的频率
// 函数返回值：无
//
//*******************************************************************************************************
void PWMStart(uint32_t ui32Freq_Hz)
{

    PWMGenDisable(PWM0_BASE, PWM_GEN_2);     //使能PWM0模块的2号发生器(因为4号PWM是2号发生器产生的)   
    
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, g_ui32SysClock / ui32Freq_Hz); // 根据Freq_Hz设置PWM周期
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4,(PWMGenPeriodGet(PWM0_BASE, PWM_GEN_2)/ 2)); //设置占空比为50%
    
    PWMGenEnable(PWM0_BASE, PWM_GEN_2);     //使能PWM0模块的2号发生器(因为4号PWM是2号发生器产生的)   
}

//*******************************************************************************************************
// 
// 函数原型：void PWMStop()
// 函数功能：M0PWM4(PG0)停止产生PWM信号
// 函数参数：无
// 函数返回值：无
//
//*******************************************************************************************************
void PWMStop()
{
    PWMGenDisable(PWM0_BASE, PWM_GEN_2);   // M0PWM4(PG0)停止产生PWM信号

}
