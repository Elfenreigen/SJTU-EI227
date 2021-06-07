//*****************************************************************************
//
// PWM.h - Prototypes for the PWM driver.
//
// Copyright��2020-2021,�Ϻ���ͨ��ѧ���ӹ���ϵʵ���ѧ����
// 
// Author: �Ϻ���ͨ��ѧ���ӹ���ϵʵ���ѧ����
// Version: 1.0.0.20210508 
// Date��2021-05-08
// History��
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
#include "inc/hw_memmap.h"        // ��ַ�궨��
#include "inc/hw_types.h"         // �������ͺ궨�壬�Ĵ������ʺ���
#include "inc/hw_i2c.h"
#include "driverlib/debug.h"      // ������
#include "driverlib/gpio.h"       // ͨ��IO�ں궨��
#include "driverlib/pin_map.h"    // TM4Cϵ��MCU��Χ�豸�ܽź궨��
#include "driverlib/sysctl.h"	  // ϵͳ���ƶ���
#include "driverlib/i2c.h"
#include "driverlib/pwm.h"        // ��Timer�йصĺ���ԭ��

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************

// M0PWM4(PG0)��ʼ��
extern void PWMInit(void);

// ����Ƶ��Ϊui32Freq_Hz�ķ���
extern void PWMStart(uint32_t ui32Freq_Hz);

// M0PWM4(PG0)ֹͣ����PWM�ź�
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
