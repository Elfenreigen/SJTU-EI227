//*******************************************************************************************************
//
// Copyright: 2021-2022, �Ϻ���ͨ��ѧ���ӹ���ϵʵ���ѧ����
// File name: Frequency_Measure.c
// Description: ��ʾ��չʾ���ʹ�ò�Ƶ�ʷ�ʵ�ַ���Ƶ�ʵĲ��������ⷽ���ź����ӵ�T0CCP0/PL4���ţ�
// 1.ÿ�ΰ���A2000TM4C�װ尴��1(SW1)��,����ʼ���������źŵ�Ƶ�ʣ�������ɺ��������ʾ�������,��λΪHz;
// 2.�������Ƶ�ʲ��ڡ�10��9999����Χ���ݣ�����ʾErr��
// 3.ע��������ⷽ���źŸߵ�ƽ���˸���3.3V���͵�ƽ���˵���0V������һ��Ҫ��װ干��
//             �ó��������ڼ��<=1HzƵ�ʵķ�����Ƶ�ʼ�����������1Hz
// Author:	�Ϻ���ͨ��ѧ���ӹ���ϵʵ���ѧ����
// Version: 1.0.0.20210513 
// Date��2021-05-13
// History��
//
//*******************************************************************************************************

//*******************************************************************************************************
//
// ͷ�ļ�
//
//*******************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "define.h"
#include "inc/hw_memmap.h"        // ��ַ�궨��
#include "inc/hw_types.h"         // �������ͺ궨�壬�Ĵ������ʺ���
#include "inc/hw_timer.h"         // �붨ʱ���йصĺ궨��
#include "inc/hw_ints.h"          // ���ж��йصĺ궨��
#include "driverlib/debug.h"      // ������
#include "driverlib/gpio.h"       // ͨ��IO�ں궨��ͺ���ԭ��
#include "driverlib/pin_map.h"    // TM4Cϵ��MCU��Χ�豸�ܽź궨��
#include "driverlib/sysctl.h"	  // ϵͳ���ƶ���
#include "driverlib/systick.h"    // SysTick Driver ����ԭ��
#include "driverlib/interrupt.h"  // NVIC�жϿ�����������ԭ��
#include "driverlib/timer.h"      // ��Timer�йصĺ���ԭ��  
#include "driverlib/pwm.h"        // ��Timer�йصĺ���ԭ��
#include "driverlib/uart.h"
#include "driverlib/fpu.h"

#include "tm1638.h"               // �����TM1638оƬ�йصĺ궨��ͺ���ԭ��

//*******************************************************************************************************
//
// �궨��
//
//*******************************************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTickƵ��Ϊ50Hz����ѭ����ʱ����20ms

#define FREQUENCY_MIN     10           // ��ЧƵ����Сֵ������ʵ����Ҫ���� 
#define FREQUENCY_MAX     9999         // ��ЧƵ�����ֵ������ʵ����Ҫ���� 
//*******************************************************************************************************
//
// ����ԭ������
//
//*******************************************************************************************************
void GPIOInit(void);        // GPIO��ʼ��
void SysTickInit(void);     // ����SysTick�ж�
void Timer0Init(void);      // Timer0��ʼ��
void DevicesInit(void);     // MCU������ʼ����ע���������������
void PWMInit(uint32_t ui32Freq_Hz);  // ����Ƶ��Ϊui32Freq_Hz�ķ���

//*******************************************************************************************************
//
// ��������
//
//*******************************************************************************************************

// 8λ�������ʾ�����ֻ���ĸ����
// ע����������λ�������������Ϊ4��5��6��7��0��1��2��3
uint8_t digit[8]={' ',' ',' ','H',' ',' ',' ',' '};

// 8λС���� 1��  0��
// ע����������λС����������������Ϊ4��5��6��7��0��1��2��3
uint8_t pnt = 0x00;

// 8��LEDָʾ��״̬��0��1��
// ע������ָʾ�ƴ������������Ϊ7��6��5��4��3��2��1��0
//     ��ӦԪ��LED8��LED7��LED6��LED5��LED4��LED3��LED2��LED1
uint8_t led[] = {0, 0, 0, 0, 0, 0, 0, 0};

// ��ǰ����ֵ
volatile uint8_t key_code = 0;

// ���ڼ�¼ǰһ�ΰ������ʱ�ļ���״̬��0��ʾ�޼����£�1�м�����
volatile uint8_t key_state = 0;

// ����������Ч��ǣ�0�������²�����1�����м�����
volatile uint8_t  key_flag = 0;

// ϵͳʱ��Ƶ�� 
uint32_t g_ui32SysClock;

// 1s����������־
volatile uint8_t g_ui8INTStatus = 0;

// ������һ��TIMER0���ؼ���ֵ
volatile uint32_t g_ui32TPreCount = 0;

// ���汾��TIMER0���ؼ���ֵ
volatile uint32_t g_ui32TCurCount = 0;

//*******************************************************************************************************
//
// ������
//
//*******************************************************************************************************
int main(void)
{
    uint32_t ui32Freq;  // ��¼�����ķ���Ƶ��
       
    DevicesInit();            //  MCU������ʼ��
	
    SysCtlDelay(60 * ( g_ui32SysClock / 3000));   // ��ʱ>60ms,�ȴ�TM1638�ϵ����
    TM1638_Init();	          // ��ʼ��TM1638
	    
    //PWMInit(1234);   //  ����3998Hz���������û�û���źŷ�����ʱ���ڲ��������ź�,���������޸�Ƶ��
        
    while (1)
    {	
        if(key_flag == 1)    // �м�����
        {
            key_flag = 0;   
            
            if(key_code == 1)  // SW1���£�����Timer0A
            {

                TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT); // Timer1A ��ʱ�ж�ʹ��
                IntEnable(INT_TIMER1A);                 // ���� TIMER1A �ж�Դ
                TimerEnable(TIMER1_BASE, TIMER_A);      // TIMER1 ��ʼ��ʱ
                
                TimerEnable(TIMER0_BASE, TIMER_A);       // TIMER0 ��ʼ����   
            }                   
        }
                
        if(g_ui8INTStatus == 1)   // 1s��ʱ��������ʼ����Ƶ��
        {
            g_ui8INTStatus = 0;
            
            ui32Freq = g_ui32TCurCount >= g_ui32TPreCount ?
                      (g_ui32TCurCount - g_ui32TPreCount):(g_ui32TCurCount - g_ui32TPreCount + 0xFFFFFFFF);
            
            if((ui32Freq >= FREQUENCY_MIN) && (ui32Freq <= FREQUENCY_MAX))
            {
                digit[4] = ui32Freq / 1000 % 10;    // ����ǧλ��
			    digit[5] = ui32Freq / 100 % 10;     // �����λ��                			    
                digit[6] = ui32Freq / 10 % 10; 	    // ����ʮλ��
			    digit[7] = ui32Freq % 10;           // �����λ��
           
            }
            else  // ����Ƶ�ʳ���ָ����Χ
            {
                digit[4] = 'E'; 	    
			    digit[5] = 'R'; 	
			    digit[6] = 'R'; 	  
			    digit[7] = ' ';            
            }
        }
    }      
}

//*******************************************************************************************************
//
// ����ԭ�ͣ�void GPIOInit(void)
// �������ܣ�GPIO��ʼ����ʹ��PortK������PK4,PK5Ϊ�����ʹ��PortM������PM0Ϊ�����
//          ��PK4����TM1638��STB��PK5����TM1638��DIO��PM0����TM1638��CLK��
// ������������
// ��������ֵ����
//
//*******************************************************************************************************
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

//*******************************************************************************************************
//
// ����ԭ�ͣ�void Timer0Init(void)
// �������ܣ�����Timer0Ϊ������أ������أ�����ģʽ��T0CCP0(PL4)Ϊ��������
// ������������
// ��������ֵ����
//
//*******************************************************************************************************
void Timer0Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);  // ʹ��TIMER0
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);   // ʹ��GPIOL
    
    GPIOPinConfigure(GPIO_PL4_T0CCP0);             // �������Ÿ���   
    GPIOPinTypeTimer(GPIO_PORTL_BASE, GPIO_PIN_4); // ����ӳ��
    GPIOPadConfigSet(GPIO_PORTL_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); // ������������
    
    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT_UP); // �볤��ʱ����������
    //TimerPrescaleSet(TIMER0_BASE, TIMER_A, 255);   // Ԥ��Ƶ256
    TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE); // ��ʼ������Ϊ��׽������        
}

//*******************************************************************************************************
//
// ����ԭ�ͣ�void Timer1Init(void)
// �������ܣ�����Timer1Ϊһ���Զ�ʱ������ʱ����Ϊ1s
// ������������
// ��������ֵ����
//
//*******************************************************************************************************
void Timer1Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);  // TIMER1 ʹ��
       
    TimerConfigure(TIMER1_BASE, TIMER_CFG_ONE_SHOT); // ����Ϊ 32 λ 1�� ��ʱ��
    TimerLoadSet(TIMER1_BASE, TIMER_A, g_ui32SysClock);  // TIMER1Aװ�ؼ���ֵ1s        
}

//*******************************************************************************************************
// 
// ����ԭ�ͣ�SysTickInit(void)
// �������ܣ�����SysTick�ж�
// ������������
// ��������ֵ����
//
//*******************************************************************************************************
void SysTickInit(void)
{
	SysTickPeriodSet(g_ui32SysClock/SYSTICK_FREQUENCY); // ������������,��ʱ����20ms
	SysTickEnable();  			// SysTickʹ��
	SysTickIntEnable();			// SysTick�ж�����
}

//*******************************************************************************************************
// 
// ����ԭ�ͣ�DevicesInit(void)
// �������ܣ�MCU������ʼ��������ϵͳʱ�����á�GPIO��ʼ����SysTick�ж�����
// ������������
// ��������ֵ����
//
//*******************************************************************************************************
void DevicesInit(void)
{
	// ʹ���ⲿ25MHz��ʱ��Դ������PLL��Ȼ���ƵΪ16MHz
	g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN | 
	                                   SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480), 
	                                   16000000);

    //UART_Init();//���ڳ�ʼ��
	GPIOInit();             // GPIO��ʼ��
    Timer0Init();           // Timer0��ʼ��
    Timer1Init();
    SysTickInit();          // ����SysTick�ж�
    IntMasterEnable();	    // ���ж�����
        
    FPULazyStackingEnable();
    FPUEnable();//ʹ��FPU
    
    IntPrioritySet(INT_TIMER1A,0x00);		// ����INT_TIMER0A������ȼ�   
    IntPrioritySet(INT_TIMER0A,0x01);		// ����INT_TIMER0A������ȼ�
    IntPrioritySet(FAULT_SYSTICK,0xe0);	    // ����SYSTICK���ȼ�����INT_TIMER0A�����ȼ�    
}

//*******************************************************************************************************
// 
// ����ԭ�ͣ�void SysTick_Handler(void)
// �������ܣ�SysTick�жϷ�����򣬼�ⰴ��������ˢ���������ʾ
// ������������
// ��������ֵ����
//
//*******************************************************************************************************
void SysTick_Handler(void)    // ��ʱ����Ϊ20ms
{

	// ˢ��ȫ������ܺ�LEDָʾ��
	TM1638_RefreshDIGIandLED(digit, pnt, led);

	// ��鵱ǰ�������룬0�����޼�������1-9��ʾ�ж�Ӧ����
	// ������ʾ��һλ�������
	key_code = TM1638_Readkeyboard();

	// ����������SysTick�жϷ�������е�״̬ת�ƴ������
	
	// key_state���ڼ�¼ǰһ�ΰ������ʱ�ļ���״̬��0��ʾ�޼����£�1�м�����
	switch (key_state)
	{
        case 0:   // ǰһ�ΰ������ʱ�޼�����
            if (key_code > 0)   // ���ΰ�������м�����
            {
                key_state = 1;
                key_flag = 1;			
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

//*******************************************************************************************************
// 
// ����ԭ�ͣ�void TIMER1A_Handler(void)
// �������ܣ�Timer1A�жϷ�����򣬼�¼���񷽲�������ʱ��ʱ����TIMER0���ļ���ֵ
// ������������
// ��������ֵ����
//
//*******************************************************************************************************
void TIMER1A_Handler(void)
{
    
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);     // ����жϱ�־
    g_ui32TPreCount = g_ui32TCurCount;   // ������һ��TIMER0���ؼ���ֵ               
    
    g_ui32TCurCount = TimerValueGet(TIMER0_BASE, TIMER_A);  // ��ȡTIMER0���ؼ���ֵ

    TimerDisable(TIMER0_BASE, TIMER_A);  // ֹͣTIMER0���ؼ���

    g_ui8INTStatus = 1;                                    // 1s�������
}

//*******************************************************************************************************
// 
// ����ԭ�ͣ�void PWMInit(uint32_t ui32Freq_Hz)
// �������ܣ�����Ƶ��Ϊui32Freq_Hz�ķ���(ռ�ձ�Ϊ50%��PWM)���������ΪM0PWM4(PG0)
//          �ú�����Ϊ�˷����û�û���źŷ�����ʱ���������źŶ���д�ġ�
// ����������ui32Freq_Hz ��Ҫ�����ķ�����Ƶ��
// ��������ֵ����
//
//*******************************************************************************************************
void PWMInit(uint32_t ui32Freq_Hz)
{
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);             // ����Ƶ
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);     // PWM0ʹ��
    PWMOutputState(PWM0_BASE, PWM_OUT_4_BIT, true); // ʹ��(����)PWM0_4�����
    PWMGenEnable(PWM0_BASE, PWM_GEN_2);             //ʹ��PWM0ģ���2�ŷ�����(��Ϊ4��PWM��2�ŷ�����������)
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, g_ui32SysClock / ui32Freq_Hz); // ����Freq_Hz����PWM����

   
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);    // ʹ��GPIOG
    GPIOPinConfigure(GPIO_PG0_M0PWM4);              // �������Ÿ���
    GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_0);    // ����ӳ��
    
    PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);   //����PWM������
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4,(PWMGenPeriodGet(PWM0_BASE, PWM_GEN_2)/ 2)); //����ռ�ձ�Ϊ50%
}
