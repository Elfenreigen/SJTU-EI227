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
#include "inc/hw_types.h"         // �������ͺ궨�壬�Ĵ������ʺ���
#include "driverlib/debug.h"      // ������
#include "driverlib/gpio.h"       // ͨ��IO�ں궨��ͺ���ԭ��
#include "driverlib/pin_map.h"    // TM4Cϵ��MCU��Χ�豸�ܽź궨��
#include "driverlib/sysctl.h"	  // ϵͳ���ƶ���
#include "driverlib/systick.h"    // SysTick Driver ԭ��
#include "driverlib/interrupt.h"  // NVIC Interrupt Controller Driver ԭ��
#include "driverlib/uart.h"       // ��UART�йصĺ궨��ͺ���ԭ��
#include "inc/hw_ints.h"
#include "tm1638.h"               // �����TM1638оƬ�йصĺ궨��ͺ���ԭ��

//*****************************************************************************
//
// �궨��
//
//*****************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTickƵ��Ϊ50Hz����ѭ����ʱ����20ms

#define V_T100ms	5                  // 0.1s�����ʱ�����ֵ��5��20ms
#define V_T1000ms	50
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
//*****************************************************************************
//
// ��������
//
//*****************************************************************************

// 8λ�������ʾ�����ֻ���ĸ����
// ע����������λ�������������Ϊ4��5��6��7��0��1��2��3
uint8_t digit[8]= {0,0,' ',' ',0,0,0,0};

// 8λС���� 1��  0��
// ע����������λС����������������Ϊ4��5��6��7��0��1��2��3
uint8_t pnt = 0xA0;

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

uint8_t clock1000ms = 0;
uint8_t clock1000ms_flag=0;
// �����ü�����
uint32_t second_counter = 0;
uint32_t minute_counter = 0;
uint32_t hour_counter = 0;
//*****************************************************************************
//
// ������
//
//*****************************************************************************
int main(void)
{
    DevicesInit();            //  MCU������ʼ��
    while(clock1000ms < 3) {};
    TM1638_Init();	          // ��ʼ��TM1638
		while(clock1000ms < 6) {};
			
    UARTCharPut(UART6_BASE,'A');
    UARTCharPut(UART6_BASE,'T');
    UARTCharPut(UART6_BASE,'+');
    UARTCharPut(UART6_BASE,'F');
    UARTCharPut(UART6_BASE,'R');
    UARTCharPut(UART6_BASE,'E');
		UARTCharPut(UART6_BASE,'Q');
    UARTCharPut(UART6_BASE,'=');
    UARTCharPut(UART6_BASE,'8');
    UARTCharPut(UART6_BASE,'8');
    UARTCharPut(UART6_BASE,'6');
			
		while(clock1000ms < 10) {};
			
		UARTCharPut(UART6_BASE,'A');
    UARTCharPut(UART6_BASE,'T');
    UARTCharPut(UART6_BASE,'+');
    UARTCharPut(UART6_BASE,'V');
    UARTCharPut(UART6_BASE,'O');
    UARTCharPut(UART6_BASE,'L');
    UARTCharPut(UART6_BASE,'=');
    UARTCharPut(UART6_BASE,'1');
    UARTCharPut(UART6_BASE,'5');

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
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)) {};		// �ȴ��˿� K׼�����

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);				// ʹ�ܶ˿� M
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)) {};		// �ȴ��˿� M׼�����

    // ���ö˿� K�ĵ�4,5λ��PK4,PK5��Ϊ�������		PK4-STB  PK5-DIO
    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_4|GPIO_PIN_5);
    // ���ö˿� M�ĵ�0λ��PM0��Ϊ�������   PM0-CLK
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_0);
}

void SysTickInit(void)
{
    SysTickPeriodSet(ui32SysClock/SYSTICK_FREQUENCY); // ������������,��ʱ����20ms
    SysTickEnable();  			// SysTickʹ��
    SysTickIntEnable();			// SysTick�ж�����
}


void UARTInit(void)
{
// SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);        // ??UART0??
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);        // ???? A
//    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));	// ???? A????

//    GPIOPinConfigure(GPIO_PA0_U0RX);				  // ??PA0?UART0 RX??
//    GPIOPinConfigure(GPIO_PA1_U0TX);    			  // ??PA1?UART0 TX??

//	// ???? A??0,1?(PA0,PA1)?UART??
//    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

//    // ?????????
//    UARTConfigSetExpClk(UART0_BASE,
//	                    ui32SysClock,
//	                    115200,                  // ???:115200
//	                    (UART_CONFIG_WLEN_8 |    // ???:8
//	                     UART_CONFIG_STOP_ONE |  // ???:1
//	                     UART_CONFIG_PAR_NONE)); // ???:?



    // ��������
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);        // ʹ��UART6ģ��
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);        // ʹ�ܶ˿� P
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOP));	// �ȴ��˿� P׼�����

    GPIOPinConfigure(GPIO_PP0_U6RX);				  // ����PD4ΪUART2 RX����
    GPIOPinConfigure(GPIO_PP1_U6TX);    			  // ����PD5ΪUART2 TX����

    // ���ö˿� P�ĵ�4,5λ��PP0,PP1��ΪUART����
    GPIOPinTypeUART(GPIO_PORTP_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // �����ʼ�֡��ʽ����
    UARTConfigSetExpClk(UART6_BASE,
                        ui32SysClock,
                        9600,                  // �����ʣ�115200
                        (UART_CONFIG_WLEN_8 |    // ����λ��8
                         UART_CONFIG_STOP_ONE |  // ֹͣλ��1
                         UART_CONFIG_PAR_NONE)); // У��λ����
    UARTFIFOLevelSet(UART6_BASE,UART_FIFO_TX1_8,UART_FIFO_RX7_8);

    IntEnable(INT_UART6); // UART0 �ж�����
    UARTIntEnable(UART6_BASE, UART_INT_RX | UART_INT_RT);// ʹ�� UART6 RX,RT �ж�

    // ��ʼ����ɺ���PC�˷���"Hello, 2A!"�ַ���
    //UARTStringPut(UART0_BASE, (const char *)"\r\nHello, 2A!\r\n");
}


void DevicesInit(void)
{
    // ʹ���ⲿ25MHz��ʱ��Դ������PLL��Ȼ���ƵΪ20MHz
    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |
                                       SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480),
                                      20000000);

    GPIOInit();             // GPIO��ʼ��
    UARTInit();             // UART��ʼ��
    SysTickInit();          // ����SysTick�ж�
    IntMasterEnable();			// ���ж�����
}




void SysTick_Handler(void)       // ��ʱ����Ϊ20ms
{

    if (++clock1000ms >=V_T1000ms)
    {

        clock1000ms_flag = 1; // ��0.1�뵽ʱ�������־��1
        clock1000ms = 0;
    }

    TM1638_RefreshDIGIandLED(digit, pnt, led);

}

void UARTStringPut(uint32_t ui32Base,const char *cMessage)
{
    while(*cMessage != '\0') // �ظ��ӽ��� FIFO ��ȡ�ַ�
        UARTCharPut(ui32Base, *(cMessage++));
}


void UART6_Handler(void)
{
    int32_t uart6_int_status;
    uint8_t uart_receive_char;
    uint8_t i=0;
    uart6_int_status = UARTIntStatus(UART6_BASE, true); // ȡ�ж�״̬
    UARTIntClear(UART6_BASE, uart6_int_status); // ���жϱ�־
    while(UARTCharsAvail(UART6_BASE)) // �ظ��ӽ��� FIFO ��ȡ�ַ�
    {
        uart_receive_char=UARTCharGetNonBlocking(UART6_BASE);
    }

}
