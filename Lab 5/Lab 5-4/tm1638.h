//*****************************************************************************
//
// tm1638.h - Prototypes for the TM1638 driver.
//
// Copyright��2020-2021, �Ϻ���ͨ��ѧ���ӹ���ϵʵ���ѧ����
// 
// Author:	�Ϻ���ͨ��ѧ���ӹ���ϵʵ���ѧ����
// Version: 1.0.0.20201228 
// Date��2020-12-28
// History��
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
#include "inc/hw_memmap.h"        // ��ַ�궨��
#include "inc/hw_types.h"         // �������ͺ궨�壬�Ĵ������ʺ���
#include "driverlib/debug.h"      // ������
#include "driverlib/gpio.h"       // ͨ��IO�ں궨��
#include "driverlib/pin_map.h"    // TM4Cϵ��MCU��Χ�豸�ܽź궨��
#include "driverlib/sysctl.h"			// ϵͳ���ƶ���


// ��������TM1638��TM4C1294 GPIO���Ŷ���
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
	
// ����ʾ���ֻ����ת��Ϊ��������ܵıʻ�ֵ
extern uint8_t TM1638_DigiSegment(uint8_t digit);

// TM1638������������
extern void TM1638_Serial_Input(uint8_t data);

// TM1638�����������
extern uint8_t TM1638_Serial_Output(void);

// ��ȡ���̵�ǰ״̬
extern uint8_t TM1638_Readkeyboard(void);

// ˢ��8λ����ܣ���С���㣩��8��ָʾ�ƣ�ÿ��2ֻ����4������ģʽ��
extern void TM1638_RefreshDIGIandLED(uint8_t digit_buf[8],
                                     uint8_t pnt_buf,
                                     uint8_t led_buf[8]);

// TM1638��ʼ������
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
