//*****************************************************************************
//
// ADC.c - API for ADC.
//
// Copyright：2020-2021,上海交通大学电子工程系实验教学中心
// 
// Author:	上海交通大学电子工程系实验教学中心
// Version: 1.0.0.20210508 
// Date：2021-05-08
// History：
//
//*****************************************************************************

//*****************************************************************************
//
// 头文件
//
//*****************************************************************************

#include "ADC.h"

//*****************************************************************************
//
// 宏定义
//
//*****************************************************************************

//*****************************************************************************
//
// 函数原型：void ADCInit(void)
// 函数功能：ADC0初始化
//           选择CH2/PE1作为ADC采样输入端口，选用采样序列产生器3
// 函数参数：无
// 函数返回值：无
//
//*****************************************************************************
void ADCInit(void)
{	   
    // 使能ADC0模块
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // 使能端口E
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // 使用CH2/PE1引脚作为ADC输入
     GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);
    
    // 选用采样序列产生器3，采样起始信号由ADCProcessorTrigger函数触发，优先级为0
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    // 设置采样序列产生器3第0步骤(也是最后1个步骤):
    // ADC_CTL_CH2--选择CH2为采样通道， ADC_CTL_END--采样序列的最后一步
    // ADC_CTL_IE--采样结束后产生中断
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH2 | ADC_CTL_IE |
                             ADC_CTL_END);

    // 使能采样序列产生器3
    ADCSequenceEnable(ADC0_BASE, 3);

    // 在采样前，必须清除中断状态标志
    ADCIntClear(ADC0_BASE, 3);		
}

//*****************************************************************************
//
// 函数原型：uint32_t ADC_Sample(void)
// 函数功能：获取ADC采样值
// 函数参数：无
// 函数返回值：ADC采样值[0-4095]
//
//*****************************************************************************
uint32_t ADC_Sample(void)
{

    // pui32ADC0Value数组用于从ADC FIFO读取的数据
    uint32_t pui32ADC0Value[1];
	
    // 触发ADC采样
    ADCProcessorTrigger(ADC0_BASE, 3);

    // 等待采样转换完成
    while(!ADCIntStatus(ADC0_BASE, 3, false))
    {
    }

    // 清除ADC中断标志
    ADCIntClear(ADC0_BASE, 3);

    // 读取ADC采样值,存储到数组pui32ADC0Value中
    ADCSequenceDataGet(ADC0_BASE, 3, pui32ADC0Value);

    // pui32ADC0Value[0]存放了PE2引脚输入电压的采样值
    return pui32ADC0Value[0];
}

