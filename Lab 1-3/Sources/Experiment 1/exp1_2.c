//**************************************************************************************
//
// Copyright: 2020-2021, �Ϻ���ͨ��ѧ���ӹ���ϵʵ���ѧ����
// File name: exp0.c
// Description: LED4(D4-PF0)��Լ��1000����Ϊ���ڻ�����˸��
//              ������PUSH1(USR_SW1-PJ0)����LED4(D4-PF0)��Լ��100����Ϊ���ڿ�����˸��
//              �ɿ�PUSH1(USR_SW1-PJ0)����LED4(D4-PF0)�ָ���1000����Ϊ���ڻ�����˸��
// Author:	�Ϻ���ͨ��ѧ���ӹ���ϵʵ���ѧ����
// Version: 1.0.0.20201228 
// Date��2020-12-28
// History��
//       
//**************************************************************************************

//**************************************************************************************
//
// ͷ�ļ�
//
//**************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"        // ��ַ�궨��
#include "inc/hw_types.h"         // �������ͺ궨�壬�Ĵ������ʺ���
#include "driverlib/debug.h"      // ������
#include "driverlib/gpio.h"       // ͨ��IO�ں궨��
#include "driverlib/pin_map.h"    // TM4Cϵ��MCU��Χ�豸�ܽź궨��
#include "driverlib/sysctl.h"     // ϵͳ���ƺ궨��

//**************************************************************************************
//
// �궨��
//
//**************************************************************************************

//**************************************************************************************
//
// ����ԭ������
//
//**************************************************************************************
void  GPIOInit(void);                               // GPIO��ʼ��

uint32_t g_ui32SysClock;
//**************************************************************************************
//
// ������
//
//**************************************************************************************
int main(void)
{
    uint8_t ui8KeyValue;

    g_ui32SysClock=SysCtlClockFreqSet(SYSCTL_OSC_INT,16000000);
	
    GPIOInit();             // GPIO��ʼ��     
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

// ����ԭ�ͣ�void GPIOInit(void)
// �������ܣ�GPIO��ʼ����ʹ��PortF������PF0Ϊ�����ʹ��PortJ������PJ0Ϊ����
// ������������
//
//**************************************************************************************
void GPIOInit(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);		   // ʹ�ܶ˿� N
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));	   // �ȴ��˿� N׼�����
		
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);		   // ʹ�ܶ˿� J	
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)){};  // �ȴ��˿� J׼�����
	
    // ���ö˿� N�ĵ�0��1λ��PN0��PN1��Ϊ�������
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0|GPIO_PIN_1); 
	
    // ���ö˿� J�ĵ�0��1λ��PJ0��PJ1��Ϊ��������
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE,GPIO_PIN_0|GPIO_PIN_1);
    
    // �˿� J�ĵ�0��1λ��Ϊ�������룬�������óɡ�����������
    GPIOPadConfigSet(GPIO_PORTJ_BASE,GPIO_PIN_0|GPIO_PIN_1,GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);       
}

//**************************************************************************************