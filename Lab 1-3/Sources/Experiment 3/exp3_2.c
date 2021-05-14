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
#include "driverlib/sysctl.h"	    // ϵͳ���ƶ���
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

void digitWrite(int pos, int num);              //д����ܣ�ʹλ�������λ�ö�Ӧ
void SetTime(uint32_t ui32Base, char* s);       //����ʱ��
void IncTime(uint32_t ui32Base, const char* addition);       //����ʱ��
void GetTime(uint32_t ui32Base);       //���ʱ��
bool strCmp(const char* s1, const char* s2);    //�ַ����ȽϺ���
bool isTimeLegal(const char *s);                //���ʱ���Ƿ�Ϸ�

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
uint8_t pnt = 0xA0;

// ϵͳʱ��Ƶ�� 
uint32_t ui32SysClock;

// ����
uint8_t clock1000ms;

uint8_t led[] = {0, 0, 0, 0, 0, 0, 0, 0};

// �洢���ܷ����ַ�����
char str[50] = {'\0'};

// ����ʱ��
int board_time = 0;

// ����ȡ��ż�����ż����
bool if_read = false;

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
//           cMessage���������ַ���
// ��������ֵ����
//
//*****************************************************************************
void UARTStringPut(uint32_t ui32Base,const char *cMessage)
{
    while(*cMessage != '\0')
        UARTCharPut(ui32Base, *(cMessage++));
    UARTCharPut(ui32Base, '\r');
    UARTCharPut(ui32Base, '\n');
}

//*****************************************************************************
//
// ����ԭ�ͣ�void UARTStringGet(uint32_t ui32Base,const char *cMessage)
// �������ܣ�UARTģ������ַ���
// ����������ui32Base��UARTģ��
//           str���������ַ���
// ��������ֵ����
//
//*****************************************************************************
void UARTStringGet(uint32_t ui32Base, char* s) {
//    char* temp = s;
    while (UARTCharsAvail(ui32Base)) // �ظ��ӽ���FIF0��ȡ�ַ�
    {
        *s = (char) UARTCharGetNonBlocking(ui32Base); // ����һ���ַ�
        ++s;
        SysCtlDelay(1 * ( ui32SysClock / 3000)); //ÿ��1�ַ���ʱ1ms
    }
    *s = '\0';
//    UARTStringPut(UART0_BASE, (const char *)"Get String:");
//    UARTStringPut(UART0_BASE, temp);
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
    UARTStringPut(UART0_BASE, (const char *)"Hello, 2A!");
		
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

void SysTick_Handler(void)
{
    if (++clock1000ms >= V_T1000ms) {                   //һ���ȥ��
        clock1000ms = 0;
        board_time = (board_time + 1) % 86400;
    }
    digitWrite(0, board_time/3600/10);
    digitWrite(1, board_time/3600%10);
    digitWrite(2, board_time%3600/60/10);
    digitWrite(3, board_time%3600/60%10);
    digitWrite(4, board_time%3600%60/10);
    digitWrite(5, board_time%3600%60%10);


    TM1638_RefreshDIGIandLED(digit, pnt, led);

}

void UART0_Handler(void)
{
  int i;
	uint32_t uart0_int_status;
	char cmd[10], time[10];

	if_read = !if_read;
  if (!if_read) return;

	  uart0_int_status = UARTIntStatus(UART0_BASE, true); // ȡ�ж�״̬
    UARTIntClear(UART0_BASE, uart0_int_status); // ���ж�״̬

    UARTStringGet(UART0_BASE, str);
//    UARTStringPut(UART0_BASE, str);

    //��ȡ�ַ����������
    for (i = 0; i < 6; ++i) {
        cmd[i] = str[i];
    }
    cmd[6] = '\0';
//    UARTStringPut(UART0_BASE, (const char *)"command:");
//    UARTStringPut(UART0_BASE, cmd);

    //��ȡ�ַ�����ʱ�䲿��
    for (i = 6; i < 14; ++i) {
        time[i-6] = str[i];
    }
    time[8] = '\0';
//    UARTStringPut(UART0_BASE, (const char *)"time:");
//    UARTStringPut(UART0_BASE, time);

    if (strCmp(cmd, "AT+GET") != 0){
        UARTStringPut(UART0_BASE, (const char *)"\nGet:");
        GetTime(UART0_BASE);
    }
    else if (isTimeLegal(time) == 0) {
        UARTStringPut(UART0_BASE, (const char *)"\nError Command!");
    }
    else if (strCmp(cmd, "AT+SET") != 0) {
        UARTStringPut(UART0_BASE, (const char *)"\nSet:");
        SetTime(UART0_BASE, time);
    }
    else if (strCmp(cmd, "AT+INC") != 0) {
        UARTStringPut(UART0_BASE, (const char *)"\nInc:");
        IncTime(UART0_BASE, time);
    }
    else UARTStringPut(UART0_BASE, (const char *)"\nError Command!");
		
}

void digitWrite(int pos, int num){
    if (pos < 4) digit[pos+4] = num;
    else digit[pos -4] = num;
}

void SetTime(uint32_t ui32Base, char* s){
    int hour, min, sec;
//    char temp[3];
    UARTStringPut(UART0_BASE, s);
    hour = 10 * (s[0] - '0') + s[1] - '0';
    min  = 10 * (s[3] - '0') + s[4] - '0';
    sec  = 10 * (s[6] - '0') + s[7] - '0';
    board_time = 3600 * hour + 60 * min + sec;
//    temp[0] = (char) hour / 10 + '0';
//    temp[1] = (char) hour % 10 + '0';
//    temp[3] = '\0';
//    UARTStringPut(UART0_BASE, temp);
//    GetTime(ui32Base);
}

void IncTime(uint32_t ui32Base, const char* addition){
    int hour = 10 * (addition[0] - '0') + addition[1] - '0';
    int min  = 10 * (addition[3] - '0') + addition[4] - '0';
    int sec  = 10 * (addition[6] - '0') + addition[7] - '0';
    board_time += 3600 * hour + 60 * min + sec;

    GetTime(ui32Base);
}

void GetTime(uint32_t ui32Base){
    int hour = board_time / 3600 % 24;
    int min  = board_time % 3600 / 60;
    int sec  = board_time % 3600 % 60;
    char s[9];
    s[0] = hour / 10 + '0';
    s[1] = hour % 10 + '0';
    s[2] = ':';
    s[3] = min / 10 + '0';
    s[4] = min % 10 + '0';
    s[5] = ':';
    s[6] = sec / 10 + '0';
    s[7] = sec % 10 + '0';
    s[8] = '\0';
    UARTStringPut(UART0_BASE, s);
}

bool strCmp(const char* s1, const char* s2){
    while (*s1 != '\0' && *s2 != '\0'){
        if (*s1 != *s2){
            return false;
        }
        ++s1;
        ++s2;
    }
    if (*s1 != '\0' || *s2 != '\0') {
        return false;
    }
    return true;
}

bool isTimeLegal(const char *s){
    int hour, min, sec;
    if (s[2] != ':' || s[5] != ':') return false;
    hour = 10 * (s[0] - '0') + s[1] - '0';
    min  = 10 * (s[3] - '0') + s[4] - '0';
    sec  = 10 * (s[6] - '0') + s[7] - '0';
    if (hour >= 0 && hour <= 23 &&
        min  >= 0 && min  <= 59 &&
        sec  >= 0 && sec  <= 59) return true;
    else return false;
}