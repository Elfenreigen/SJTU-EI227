//*****************************************************************************
//
// Copyright: 2020-2021, �Ϻ���ͨ��ѧ���ӹ���ϵʵ���ѧ����
// File name: ADC_Demo.c
// Description: ��ʾ��չʾ�������AIN2/PE1�˿�ʵ�ֵ������뵥��ADC����,����Ƶ��25Hz
//    1.����ĸ��������ʾADC����ֵ[0-4095]��
//    2.�Ҳ������������ʾ��ѹֵ[0.00-3.30V]��
//    3.ע�⣺�����ѹֵ��Χ����Ϊ[0-3.3V]��������ջ��˿ڡ�
// Author:	�Ϻ���ͨ��ѧ���ӹ���ϵʵ���ѧ����
// Version: 1.0.0.20210513 
// Date��2021-05-13
// History��
//
//*****************************************************************************

//*****************************************************************************
//
// ͷ�ļ�
//
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"        // ��ַ�궨��
#include "inc/hw_types.h"         // �������ͺ궨�壬�Ĵ������ʺ���
#include "driverlib/debug.h"      // ������
#include "driverlib/gpio.h"       // ͨ��IO�ں궨��
#include "driverlib/pin_map.h"    // TM4Cϵ��MCU��Χ�豸�ܽź궨��
#include "driverlib/sysctl.h"	  // ϵͳ���ƶ���
#include "driverlib/systick.h"    // SysTick Driver ԭ��
#include "driverlib/interrupt.h"  // NVIC Interrupt Controller Driver ԭ��
#include "driverlib/adc.h"        // ��ADC�йصĶ��� 

#include "tm1638.h"               // �����TM1638оƬ�йصĺ���
#include "ADC.h"
//*****************************************************************************
//
// �궨��
//
//*****************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTickƵ��Ϊ50Hz����ѭ����ʱ����20ms

#define V_T40ms	 2              // 40ms�����ʱ�����ֵ��2��20ms

#define V_T100ms 5              // 0.1s�����ʱ�����ֵ��5��20ms
//*****************************************************************************
//
// ����ԭ������
//
//*****************************************************************************
void GPIOInit(void);        // GPIO��ʼ��
void SysTickInit(void);     // ����SysTick�ж� 
void DevicesInit(void);     // MCU������ʼ����ע���������������
//*****************************************************************************
//
// ��������
//
//*****************************************************************************

// �����ʱ������
uint8_t clock40ms = 0;
uint8_t clock100ms = 0;

// �����ʱ�������־
uint8_t clock40ms_flag = 0;
uint8_t	clock100ms_flag = 0; 

// 8λ�������ʾ�����ֻ���ĸ����
// ע����������λ�������������Ϊ4��5��6��7��0��1��2��3
uint8_t digit[8]={' ',' ',' ',' ',' ',' ',' ',' '};

// 8λС���� 1��  0��
// ע����������λС����������������Ϊ4��5��6��7��0��1��2��3
uint8_t pnt = 0x2;

// 8��LEDָʾ��״̬��0��1��
// ע������ָʾ�ƴ������������Ϊ7��6��5��4��3��2��1��0
//     ��ӦԪ��LED8��LED7��LED6��LED5��LED4��LED3��LED2��LED1
uint8_t led[] = {1, 0, 0, 0, 0, 0, 0, 0};

// ϵͳʱ��Ƶ�� 
uint32_t ui32SysClock;

// AIN2(PE1)  ADC����ֵ[0-4095]
uint32_t ui32ADC0Value;     

// AIN2��ѹֵ(��λΪ0.01V) [0.00-3.30]
uint32_t ui32ADC0Voltage; 

//*****************************************************************************
//
// ������
//
//*****************************************************************************
 int main(void)
{

	DevicesInit();            //  MCU������ʼ��
	
	SysCtlDelay(60 * ( ui32SysClock / 3000));   // ��ʱ>60ms,�ȴ�TM1638�ϵ����
	TM1638_Init();	          // ��ʼ��TM1638
	
	while (1)
	{				
        
        if (clock40ms_flag == 1)             // ���40ms�붨ʱ�Ƿ�
        {
            clock40ms_flag = 0;
            
            ui32ADC0Value = ADC_Sample();   // ����
			
           	digit[4] = ui32ADC0Value / 1000; 	     // ��ʾADC����ֵǧλ��
			digit[5] = ui32ADC0Value / 100 % 10; 	 // ��ʾADC����ֵ��λ��
			digit[6] = ui32ADC0Value / 10 % 10; 	 // ��ʾADC����ֵʮλ��
			digit[7] = ui32ADC0Value % 10;           // ��ʾADC����ֵ��λ��
            
			ui32ADC0Voltage = ui32ADC0Value * 330 / 4095;
            
            digit[1] = (ui32ADC0Voltage / 100) % 10; // ��ʾ��ѹֵ��λ��
            digit[2] = (ui32ADC0Voltage / 10) % 10;  // ��ʾ��ѹֵʮ��λ��
            digit[3] = ui32ADC0Voltage % 10;         // ��ʾ��ѹֵ�ٷ�λ��             
        }
	}
	
}

//*****************************************************************************
//
// ����ԭ�ͣ�void GPIOInit(void)
// �������ܣ�GPIO��ʼ����ʹ��PortK������PK4,PK5Ϊ�����ʹ��PortM������PM0Ϊ�����
//          ��PK4����TM1638��STB��PK5����TM1638��DIO��PM0����TM1638��CLK��
// ������������
// ��������ֵ����
//
//*****************************************************************************
void GPIOInit(void)
{
	//����TM1638оƬ�ܽ�
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);				// ʹ�ܶ˿� K	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)){};		// �ȴ��˿� K׼�����		
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);				// ʹ�ܶ˿� M	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)){};		// �ȴ��˿� M׼�����		
	
   // ���ö˿� K�ĵ�4,5λ��PK4,PK5��Ϊ�������		PK4-STB  PK5-DIO
	GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_4|GPIO_PIN_5);
	// ���ö˿� M�ĵ�0λ��PM0��Ϊ�������   PM0-CLK
	GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_0);	
}

//*****************************************************************************
// 
// ����ԭ�ͣ�SysTickInit(void)
// �������ܣ�����SysTick�ж�
// ������������
// ��������ֵ����
//
//*****************************************************************************
void SysTickInit(void)
{
    SysTickPeriodSet(ui32SysClock/SYSTICK_FREQUENCY); // ������������,��ʱ����20ms
    SysTickEnable();  			// SysTickʹ��
    SysTickIntEnable();			// SysTick�ж�����
}

//*****************************************************************************
// 
// ����ԭ�ͣ�DevicesInit(void)
// �������ܣ�CU������ʼ��������ϵͳʱ�����á�GPIO��ʼ����SysTick�ж�����
// ������������
// ��������ֵ����
//
//*****************************************************************************
void DevicesInit(void)
{
	// ʹ���ⲿ25MHz��ʱ��Դ������PLL��Ȼ���ƵΪ20MHz
	ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN | 
	                                   SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480), 
	                                   20000000);

  GPIOInit();             // GPIO��ʼ��
  ADCInit();              // ADC��ʼ��
  SysTickInit();          // ����SysTick�ж�
  IntMasterEnable();	  // ���ж�����
}

//*****************************************************************************
// 
// ����ԭ�ͣ�void SysTick_Handler(void)
// �������ܣ�SysTick�жϷ������
// ������������
// ��������ֵ����
//
//*****************************************************************************
void SysTick_Handler(void)       // ��ʱ����Ϊ20ms
{
 
	// 40ms������ʱ������
	if (++clock40ms >= V_T40ms)
	{
		clock40ms_flag = 1; // ��40ms��ʱ�������־��1
		clock40ms = 0;
	}

    
	// 0.1������ʱ������
	if (++clock100ms >= V_T100ms)
	{
		clock100ms_flag = 1; // ��0.1�뵽ʱ�������־��1
		clock100ms = 0;
	}
	
	// ˢ��ȫ������ܺ�LED
	TM1638_RefreshDIGIandLED(digit, pnt, led);

}
