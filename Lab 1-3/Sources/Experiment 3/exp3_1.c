//*****************************************************************************
//
// Copyright: 2020-2021, �Ϻ���ͨ��ѧ���ӹ���ϵʵ���ѧ����
// File name: exp3_0.c
// Description: 
// ������ͨ��A2000TM4�װ尴�����п���UART�˿ڵ����ݶ�д���������ַ��Ĵ�Сдת����
// 1.������λ�󣬵װ�����ߵ�2���������ʾ0��LED8������ϵͳ������Ĭ��״̬��״̬0��
// 2.ϵͳ�����󣬻ᴦ������4��״̬��
//    1)״̬0��Ĭ��״̬���±��Ϊ4-9��������롣��ʱPC�˷��͵��ַ���ʵ����յ���
//            ԭ�����ء��װ�����ߵ�2���������ʾ0��LED8������
//    2)״̬1�����±��Ϊ1�İ�������롣��ʱPC��������͵���Сд�ַ�'a'-'z',ʵ���
//            �յ���ת��Ϊ��д'A'-'Z'�󷵻أ������ַ���ԭ�����ء��װ�����ߵ�2��
//            �������ʾ1��LED7������
//    3)״̬2�����±��Ϊ2�İ�������롣��ʱPC��������͵��Ǵ�д�ַ�'A'-'Z',ʵ���
//            �յ���ת��ΪСд'a'-'z'�󷵻أ������ַ���ԭ�����ء��װ�����ߵ�2��
//            �������ʾ2��LED6������
//    4)״̬3�����±��Ϊ3�İ�������롣��ʱPC��������͵��Ǵ�д�ַ�'A'-'Z',ʵ���
//            �յ���ת��ΪСд'a'-'z'�󷵻أ�������͵���Сд�ַ�'a'-'z',ʵ���
//            �յ���ת��Ϊ��д'A'-'Z'�󷵻أ������ַ���ԭ�����ء��װ�����ߵ�2��
//            �������ʾ3��LED5������
// Author:	�Ϻ���ͨ��ѧ���ӹ���ϵʵ���ѧ����
// Version: 1.0.0.20201228 
// Date��2020-12-28
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

//#define V_T100ms	5                  // 0.1s�����ʱ�����ֵ��5��20ms
//*****************************************************************************
//
// ����ԭ������
//
//*****************************************************************************
void GPIOInit(void);        // GPIO��ʼ��
void SysTickInit(void);     // ����SysTick�ж�
void UARTInit(void);        // UART��ʼ��
void DevicesInit(void);     // MCU������ʼ����ע���������������
void UARTStringPut(uint32_t ui32Base,const char *cMessage);// ��UART�����ַ���
void UART0_Handler(void);
//*****************************************************************************
//
// ��������
//
//*****************************************************************************

// 8λ�������ʾ�����ֻ���ĸ����
// ע����������λ�������������Ϊ4��5��6��7��0��1��2��3
uint8_t digit[8]={' ',' ',' ',' ','_',0,'_',' '};

// 8λС���� 1��  0��
// ע����������λС����������������Ϊ4��5��6��7��0��1��2��3
uint8_t pnt = 0x00;

// 8��LEDָʾ��״̬��0��1��
// ע������ָʾ�ƴ������������Ϊ7��6��5��4��3��2��1��0
//     ��ӦԪ��LED8��LED7��LED6��LED5��LED4��LED3��LED2��LED1
uint8_t led[] = {0, 0, 0, 0, 0, 0, 0, 1};

// ��ǰ����ֵ
volatile uint8_t key_code = 0;

// ���ڼ�¼ǰһ�ΰ������ʱ�ļ���״̬��0��ʾ�޼����£�1�м�����
volatile uint8_t key_state = 0;

// ��¼ϵͳ����״̬
volatile uint8_t  run_state = 0;

// ϵͳʱ��Ƶ�� 
uint32_t ui32SysClock;

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
	
    while (1){}
	
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
	uint8_t i;
		
	// ˢ��ȫ������ܺ�LEDָʾ��
	TM1638_RefreshDIGIandLED(digit, pnt, led);

	// ��鵱ǰ�������룬0�����޼�������1-9��ʾ�ж�Ӧ����
	// ������ʾ��һλ�������
	key_code = TM1638_Readkeyboard();

	// ����������SysTick�жϷ�������е�״̬ת�ƴ������
	
	// key_state���ڼ�¼ǰһ�ΰ������ʱ�ļ���״̬��0��ʾ�޼����£�1�м�����
	switch (key_state)
	{
        case 0:  // ǰһ�ΰ������ʱ�޼�����
            if (key_code > 0)   // ���ΰ�������м�����
            {
                key_state = 1;
			
                if(key_code >= 1 && key_code <= 3)   // ���1-3��������
                {	
                    run_state = key_code;              // �޸�ϵͳ����״̬
                    for(i = 0; i < 8; ++i) led[i] = 0; // �޸�LED����ʾ
                    led[7 - run_state] = 1;         
                    digit[5] = run_state;           // �޸���ʾ���� 
                }
                else      // ���4-9��������
                {
                    run_state = 0;          // �޸�ϵͳ����״̬
                    for(i = 0; i < 8; ++i) led[i] = 0;  // �޸�LED����ʾ
                    led[7] = 1;
                    digit[5] = 0;           // �޸���ʾ����
                }
            }
		    break;
        case 1:    // ǰһ�ΰ������ʱ�м�����
            if (key_code == 0)  // ���ΰ������ʱ�޼�����
            {
                key_state = 0;
            }
            break;
        default:
            key_state = 0;
            break;
    }	
	
}




void UART0_Handler(void)
{
	int32_t uart0_int_status;
	uint8_t uart_receive_char;
	
	uart0_int_status = UARTIntStatus(UART0_BASE, true); // ȡ�ж�״̬
	UARTIntClear(UART0_BASE, uart0_int_status); // ���ж�״̬

	while(UARTCharsAvail(UART0_BASE)) // �ظ��ӽ���FIF0��ȡ�ַ�
	{
		uart_receive_char = UARTCharGetNonBlocking(UART0_BASE); // ����һ���ַ�
		switch (run_state)
		{
			case 1: // Сдת��д
				if(uart_receive_char >= 'a' && uart_receive_char <= 'z')
				{
					uart_receive_char = uart_receive_char - 'a' + 'A';
				}
				break;
			case 2: // ��дתСд
				if(uart_receive_char >= 'A' && uart_receive_char <= 'Z')
				{
					uart_receive_char = uart_receive_char - 'A' + 'a';
				}
				break;
			case 3: // ��Сд����	
				if(uart_receive_char >= 'a' && uart_receive_char <= 'z')
				{
					uart_receive_char = uart_receive_char - 'a' + 'A';
				}
				else
				{
					if(uart_receive_char >= 'A' && uart_receive_char <= 'Z')
					{
						uart_receive_char = uart_receive_char - 'A' + 'a';
					}
				}
				break;
				default:
				break;
		}
		// ����ת���õ��ַ�
		UARTCharPutNonBlocking(UART0_BASE, uart_receive_char);
		if(uart_receive_char == '\r') // �������'\r'����һ���س�
			{
				UARTCharPutNonBlocking(UART0_BASE, '\n');
			}
		}
}

