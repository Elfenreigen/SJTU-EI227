//*****************************************************************************
//
// ͷ�ļ�
//
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"        // ��ַ�궨��
#include "inc/hw_ints.h"
#include "inc/hw_types.h"         // �������ͺ궨�壬�Ĵ������ʺ���
#include "driverlib/debug.h"      // ������
#include "driverlib/gpio.h"       // ͨ��IO�ں궨��ͺ���ԭ��
#include "driverlib/pin_map.h"    // TM4Cϵ��MCU��Χ�豸�ܽź궨��
#include "driverlib/sysctl.h"	  // ϵͳ���ƶ���
#include "driverlib/systick.h"    // SysTick Driver ԭ��
#include "driverlib/interrupt.h"  // NVIC Interrupt Controller Driver ԭ��
#include "driverlib/uart.h"       // ��UART�йصĺ궨��ͺ���ԭ��

#include "tm1638.h"               // �����TM1638оƬ�йصĺ궨��ͺ���ԭ��

//*****************************************************************************
//
// �궨��
//
//*****************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTickƵ��Ϊ50Hz����ѭ����ʱ����20ms

#define V_T1000ms	50                  // 1s�����ʱ�����ֵ��50��20ms

//*****************************************************************************
//
// ����ԭ������
//
//*****************************************************************************
void GPIOInit(void);        // GPIO��ʼ��
void SysTickInit(void);     // ����SysTick�ж� 
void DevicesInit(void);     // MCU������ʼ����ע���������������
void UARTInit(void);        // UART��ʼ��
void UARTStringPut(uint32_t ui32Base,const char *cMessage);// ��UART�����ַ���
//*****************************************************************************
//
// ��������
//
//*****************************************************************************

// 8λ�������ʾ�����ֻ���ĸ����
// ע����������λ�������������Ϊ4��5��6��7��0��1��2��3
uint8_t digit[8]={' ',' ',' ',' ',' ',' ',' ',' '};

// 8λС���� 1��  0��
// ע����������λС����������������Ϊ4��5��6��7��0��1��2��3
uint8_t pnt = 0xa0;

// 8��LEDָʾ��״̬��0��1��
// ע������ָʾ�ƴ������������Ϊ7��6��5��4��3��2��1��0
//     ��ӦԪ��LED8��LED7��LED6��LED5��LED4��LED3��LED2��LED1
uint8_t led[] = {0, 0, 0, 0, 0, 0, 0, 0};


// ϵͳʱ��Ƶ�� 
uint32_t ui32SysClock;

// �����ʱ������
uint8_t clock1000ms = 0;

// �����ʱ�������־
uint8_t clock1000ms_flag = 0;

// �����ü�����
uint32_t test_counter = 0;

uint8_t hour = 0;
uint8_t minute = 0;
uint8_t second = 0;
//*****************************************************************************
//
// ������
//
//*****************************************************************************
int main(void)
{
    

    DevicesInit();            //  MCU������ʼ��
	
    SysCtlDelay(60 * ( ui32SysClock / 3000)); // ��ʱ>60ms,�ȴ�TM1638�ϵ����
    TM1638_Init();	          // ��ʼ��TM1638
	
	
	  digit[4] = 0;
	  digit[5] = 0;
	  digit[6] = 0;
   	digit[7] = 0;
  	digit[0] = 0;
	  digit[1] = 0;
	
    while (1)
			{
				if (clock1000ms_flag == 1)      // ���1�붨ʱ�Ƿ�
		{
			clock1000ms_flag		= 0;
			// ÿ1���ۼӼ�ʱֵ�����������ʮ������ʾ
				if (++test_counter >= 86400) test_counter = 0;
				hour=test_counter /3600;
				minute=test_counter % 3600 / 60;
				second=test_counter % 3600 % 60;
				digit[4] = hour / 10; 	    // ����Сʱ
				digit[5] = hour % 10; 	
				digit[6] = minute / 10; 	  // �������
				digit[7] = minute % 10;       
				digit[0] = second / 10;     // ��������
			  digit[1] = second % 10;
			  UARTStringPut(UART0_BASE, (const char *)"\r\n�������л����񹲺͹�����ʱ��");
				UARTCharPut(UART0_BASE,(char)(digit[4]+'0'));
				UARTCharPut(UART0_BASE,(char)(digit[5]+'0'));
				UARTCharPut(UART0_BASE,':');
				UARTCharPut(UART0_BASE,(char)(digit[6]+'0'));
				UARTCharPut(UART0_BASE,(char)(digit[7]+'0'));
				UARTCharPut(UART0_BASE,':');
				UARTCharPut(UART0_BASE,(char)(digit[0]+'0'));
				UARTCharPut(UART0_BASE,(char)(digit[1]+'0'));
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
// ����ԭ�ͣ�void UARTStringPut(uint32_t ui32Base,const char *cMessage)
// �������ܣ���UARTģ�鷢���ַ���
// ����������ui32Base��UARTģ��
//          cMessage���������ַ���  
// ��������ֵ����
//
//*****************************************************************************
void UARTStringPut(uint32_t ui32Base,const char *cMessage)
{
	while(*cMessage != '\0')
		UARTCharPut(ui32Base, *(cMessage++));
}

//*****************************************************************************
//
// ����ԭ�ͣ�void UARTInit(void)
// �������ܣ�UART��ʼ����ʹ��UART0������PA0,PA1ΪUART0 RX,TX���ţ�
//          ���ò����ʼ�֡��ʽ��
// ������������
// ��������ֵ����
//
//*****************************************************************************
void UARTInit(void)
{
    // ��������
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);        // ʹ��UART0ģ��   
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);        // ʹ�ܶ˿� A
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));	// �ȴ��˿� A׼����� 

    GPIOPinConfigure(GPIO_PA0_U0RX);				  // ����PA0ΪUART0 RX����
    GPIOPinConfigure(GPIO_PA1_U0TX);    			  // ����PA1ΪUART0 TX����        

	// ���ö˿� A�ĵ�0,1λ��PA0,PA1��ΪUART����
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);   

    // �����ʼ�֡��ʽ����
    UARTConfigSetExpClk(UART0_BASE, 
	                    ui32SysClock,
	                    115200,                  // �����ʣ�115200
	                    (UART_CONFIG_WLEN_8 |    // ����λ��8
	                     UART_CONFIG_STOP_ONE |  // ֹͣλ��1
	                     UART_CONFIG_PAR_NONE)); // У��λ����
	
    // ��ʼ����ɺ���PC�˷���"Hello, 2A!"�ַ���
    UARTStringPut(UART0_BASE, (const char *)"\r\nHello, 2A!\r\n");	
		
		IntEnable(INT_UART0); // UART0 �ж�����
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);// ʹ�� UART0 RX,RT �ж�
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
    SysTickInit();          // ����SysTick�ж�
    UARTInit();             // UART��ʼ�� 
    IntMasterEnable();			// ���ж�����
}

void SysTick_Handler(void)       // ��ʱ����Ϊ20ms
{
	// 1������ʱ������
	if (++clock1000ms >= V_T1000ms)
	{
		clock1000ms_flag = 1; // ��1�뵽ʱ�������־��1
		clock1000ms = 0;
	}
	
 	
	// ˢ��ȫ������ܺ�LEDָʾ��
	TM1638_RefreshDIGIandLED(digit, pnt, led);



}








