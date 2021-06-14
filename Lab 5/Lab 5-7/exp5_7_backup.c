//*****************************************************************************
//
// Copyright: 2020-2021, �Ϻ���ͨ��ѧ���ӹ���ϵʵ���ѧ����
// File name: exp2_0.c
// Description: 
//    1.������λ�󣬵װ����ұ�4λ������Զ���ʾ��ʱ��ֵ�����λ��Ӧ��λ��0.1�룻
//    2.������λ�󣬵װ���8��LED�����������ʽ��������ѭ���任��Լ0.5��任1�Σ�
//    3.��û�а�������ʱ����������ڶ�λ�������ʾ��0����
//      ���˹�����ĳ�����������ʾ�ü��ı�ţ�
//      �˿���λ��ʱ�������ͣ�仯��ֹͣ��ʱ��ֱ���ſ��������Զ�������ʱ��
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
#include "define.h"               // ����NCPDT
#include "inc/hw_memmap.h"        // ��ַ�궨��
#include "inc/hw_ints.h"
#include "inc/hw_types.h"         // �������ͺ궨�壬�Ĵ������ʺ���
#include "driverlib/debug.h"      // ������
#include "driverlib/gpio.h"       // ͨ��IO�ں궨��
#include "driverlib/pin_map.h"    // TM4Cϵ��MCU��Χ�豸�ܽź궨��
#include "driverlib/sysctl.h"	  // ϵͳ���ƶ���
#include "driverlib/systick.h"    // SysTick Driver ԭ��
#include "driverlib/interrupt.h"  // NVIC Interrupt Controller Driver ԭ��
#include "driverlib/uart.h"       // ��UART�йصĺ궨��ͺ���ԭ��

#include "tm1638.h"               // �����TM1638оƬ�йصĺ���
#include "JLX12864_2.c"
#include "modes.c"
#include "ADC.h"

//*****************************************************************************
//
// �궨��
//
//*****************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTickƵ��Ϊ50Hz����ѭ����ʱ����20ms

#define V_T40ms	 2              // 40ms�����ʱ�����ֵ��2��20ms
#define V_T100ms	5              // 0.1s�����ʱ�����ֵ��5��20ms
#define V_T500ms	25             // 0.5s�����ʱ�����ֵ��25��20ms
#define V_T2s     100           // 2s�����ʱ�����ֵ��100��20ms
#define V_T5s     250           // 5s�����ʱ�����ֵ
#define V_T10s    500           // 10s�����ʱ�����ֵ
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
void UARTStringGet(uint32_t ui32Base, char* s);
void UART6_Handler(void);
void passFreq(uint8_t* f);
//*****************************************************************************
//
// ��������
//
//*****************************************************************************
uint8_t sjtu[128*8];
uint8_t bit[8];
// �����ʱ������
uint8_t clock40ms = 0;
uint8_t clock100ms = 0;
uint8_t clock500ms = 0;
uint8_t clock2s = 0;
uint8_t clock5s = 0;
uint16_t clock10s = 0;

// �����ʱ�������־
uint8_t clock100ms_flag = 0;
uint8_t clock500ms_flag = 0;
uint8_t clock2s_flag = 1;
uint8_t clock5s_flag = 0;
uint8_t clock10s_flag = 0;
uint8_t clock40ms_flag = 0;
bool if_count_2s = false;      // �Ƿ�ʼ����

//LCD��Ļ״̬��
uint8_t lcd_act = 0;
uint8_t lcd_subact = 0;     //������״̬
bool if_refresh = true;    // ��ֵΪtrue���ڳ�ʼ��ʱ�ض�����ģʽ0

// �����ü�����
uint32_t test_counter = 0;

// 8λ�������ʾ�����ֻ���ĸ����
// ע����������λ�������������Ϊ4��5��6��7��0��1��2��3
uint8_t digit[8]={' ',' ',' ',' ','_',' ','_',' '};

// 8λС���� 1��  0��
// ע����������λС����������������Ϊ4��5��6��7��0��1��2��3
uint8_t pnt = 0x01;

// 8��LEDָʾ��״̬��0��1��
// ע������ָʾ�ƴ������������Ϊ7��6��5��4��3��2��1��0
//     ��ӦԪ��LED8��LED7��LED6��LED5��LED4��LED3��LED2��LED1
uint8_t led[] = {1, 1, 1, 1, 1, 1, 1, 0};

// �洢���ܷ����ַ�����
char str[50] = {'\0'};

// ��ǰ����ֵ
uint8_t key_code = 0;

// ϵͳʱ��Ƶ�� 
uint32_t ui32SysClock;

// ����������
uint8_t* letter[3]={"A", "B", "C"};
uint8_t* number[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
uint16_t voltage = 0;
uint8_t* v_passin[4] = {0};
uint16_t tempre = 305;
uint8_t* t_passin[3] = {0};
uint8_t freq[4] = {1, 0, 0, 0};
uint8_t freq_temp[4] = {0};    // ���ڱ���Ƶ�ʲ����������Ϸ��򷵻��޸�ǰ����
uint8_t* f_passin[4] = {0};

// ���庯��ָ����������л�ģʽ
void (*func[3][5])(uint8_t* [], uint8_t* [], uint8_t* []) = {0};

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
    uint8_t temp,i;
    uint8_t row;
    uint8_t line;


    DevicesInit();            //  MCU������ʼ��

    while (clock100ms < 3);   // ��ʱ>60ms,�ȴ�TM1638�ϵ����
    TM1638_Init();	          // ��ʼ��TM1638
    initial_lcd();

    clear_screen();

    for (row=0;row<128;++row)
    {
        for(line=0;line<8;++line)
        {
            sjtu[line*128+127-row]=~bmp1[row*8+line];
            bit[0]=sjtu[line*128+127-row]%2;
            bit[1]=sjtu[line*128+127-row]%4/2;
            bit[2]=sjtu[line*128+127-row]%8/4;
            bit[3]=sjtu[line*128+127-row]%16/8;
            bit[4]=sjtu[line*128+127-row]%32/16;
            bit[5]=sjtu[line*128+127-row]%64/32;
            bit[6]=sjtu[line*128+127-row]%128/64;
            bit[7]=sjtu[line*128+127-row]/128;
            sjtu[line*128+127-row]=bit[0]*128+bit[1]*64+bit[2]*32+bit[3]*16+bit[4]*8+bit[5]*4+bit[6]*2+bit[7];
        }
    }

    //������װ�뺯��ָ��������
    func[0][0] = mode0_0;
    func[1][0] = mode1_0;
    func[1][1] = mode1_1;
    func[1][2] = mode1_2;
    func[1][3] = mode1_3;
    func[1][4] = mode1_4;
    func[2][0] = mode2_0;

    t_passin[0] = number[tempre / 100];
    t_passin[1] = number[(tempre / 10) % 10];
    t_passin[2] = number[tempre % 10];

    passFreq(freq);

    while (1)
    {
        // ������ѹ
        if (clock500ms_flag == 1)             // ���40ms�붨ʱ�Ƿ�
        {
            clock500ms_flag = 0;

            ui32ADC0Value = ADC_Sample();   // ����
//            ui32ADC0Value = 2000;   // ����

            digit[4] = ui32ADC0Value / 1000; 	     // ��ʾADC����ֵǧλ��
            digit[5] = ui32ADC0Value / 100 % 10; 	 // ��ʾADC����ֵ��λ��
            digit[6] = ui32ADC0Value / 10 % 10; 	 // ��ʾADC����ֵʮλ��
            digit[7] = ui32ADC0Value % 10;           // ��ʾADC����ֵ��λ��

            ui32ADC0Voltage = ui32ADC0Value * 3300 / 4095;

            digit[0] = (ui32ADC0Voltage / 1000) % 10;
            digit[1] = (ui32ADC0Voltage / 100) % 10; // ��ʾ��ѹֵ��λ��
            digit[2] = (ui32ADC0Voltage / 10) % 10;  // ��ʾ��ѹֵʮ��λ��
            digit[3] = ui32ADC0Voltage % 10;         // ��ʾ��ѹֵ�ٷ�λ��

            v_passin[0] = number[(ui32ADC0Voltage / 1000) % 10]; // ��ʾ��ѹֵ��λ��
            v_passin[1] = number[(ui32ADC0Voltage / 100) % 10];  // ��ʾ��ѹֵʮ��λ��
            v_passin[2] = number[(ui32ADC0Voltage / 10) % 10];   // ��ʾ��ѹֵ�ٷ�λ��
            v_passin[3] = number[(ui32ADC0Voltage) % 10];        // ��ʾ��ѹֵǧ��λ��
            if (lcd_act == 0 && lcd_subact == 0) if_refresh = true;
        }


        if (clock500ms_flag == 1)   // ���0.5�붨ʱ�Ƿ�
        {
            clock500ms_flag = 0;
            // 8��ָʾ��������Ʒ�ʽ��ÿ0.5�����ң�ѭ�����ƶ�һ��
            temp = led[0];
            for (i = 0; i < 7; i++) led[i] = led[i + 1];
            led[7] = temp;
        }

        // ˢ����Ļ����
        if (if_refresh || (if_count_2s && clock2s_flag))
        {
            if (if_count_2s && clock2s_flag){
                lcd_act = 1;
                lcd_subact = 0;
                if_count_2s = false;
            }
            if_refresh = false;

            for (i=0; i<4; ++i) f_passin[i] = number[freq_temp[i]]; // ������ʾƵ��
            TEST_H;

            func[lcd_act][lcd_subact](v_passin, t_passin, f_passin);    // ���ݲ�ͬģʽ��ʾ��ͬ����

            if (lcd_act == 2 && lcd_subact == 0) {if_count_2s = true; clock2s = 0; clock2s_flag = false;}   // ���뾯ʾ������ʼ��ʱ
            else if_count_2s = false;
            TEST_L;
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
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);						// ʹ�ܶ˿� K
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)){};		// �ȴ��˿� K׼�����

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);						// ʹ�ܶ˿� M
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
// ����ԭ�ͣ�void DevicesInit(void)
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
    UARTInit();             // UART��ʼ��
    SysTickInit();          // ����SysTick�ж�
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
    uint8_t key_code_pre = key_code;    // ����ǰһ�ΰ���״̬
    uint8_t i = 0;

    // 0.1������ʱ������
    if (++clock100ms >= V_T100ms)
    {
        clock100ms_flag = 1; // ��0.1�뵽ʱ�������־��1
        clock100ms = 0;
    }

    // 0.5������ʱ������
    if (++clock500ms >= V_T500ms)
    {
        clock500ms_flag = 1; // ��0.5�뵽ʱ�������־��1
        clock500ms = 0;
    }

    // 2������ʱ������
    if (++clock2s >= V_T2s)
    {
        clock2s_flag = 1; // ��2�뵽ʱ�������־��1
        clock2s = 0;
    }

    // 5������ʱ������
    if (++clock5s >= V_T5s){
        clock5s_flag = 1;
        clock5s = 0;
    }

    // 10������ʱ������
    if (++clock10s >= V_T10s){
        clock10s_flag = 1;
        clock10s = 0;
    }

    if (++clock40ms >= V_T40ms)
    {
        clock40ms_flag = 1; // ��40ms��ʱ�������־��1
        clock40ms = 0;
    }


    // ˢ��ȫ������ܺ�LEDָʾ��
    TM1638_RefreshDIGIandLED(digit, pnt, led);

    // ��鵱ǰ�������룬0�����޼�������1-9��ʾ�ж�Ӧ����
    // ������ʾ��һλ�������
    key_code = TM1638_Readkeyboard();


    // �������л�
    if (key_code_pre == 0 && key_code != 0 && lcd_act == 0 && lcd_subact == 0){
        lcd_act = 1;
        lcd_subact = 0;
        for (i =0; i<4; ++i){
            freq_temp[i] = freq[i];
        }   // �����޸�ǰ����
        if_refresh = true;
        return;
    }

    if (key_code_pre != 8 && key_code == 8){
        if (lcd_act == 1 && lcd_subact == 0){
            freq_temp[0] = (freq_temp[0] + 9) % 10;
            if_refresh = true;
        } else if (lcd_act == 1 && lcd_subact == 1){
            freq_temp[1] = (freq_temp[1] + 9) % 10;
            if_refresh = true;
        } else if (lcd_act == 1 && lcd_subact == 2){
            freq_temp[2] = (freq_temp[2] + 9) % 10;
            if_refresh = true;
        } else if (lcd_act == 1 && lcd_subact == 3){
            freq_temp[3] = (freq_temp[3] + 9) % 10;
            if_refresh = true;
        }
    }

    if (key_code_pre != 2 && key_code == 2){
        if (lcd_act == 1 && lcd_subact == 0){
            freq_temp[0] = (freq_temp[0] + 1) % 10;
            if_refresh = true;
        } else if (lcd_act == 1 && lcd_subact == 1){
            freq_temp[1] = (freq_temp[1] + 1) % 10;
            if_refresh = true;
        } else if (lcd_act == 1 && lcd_subact == 2){
            freq_temp[2] = (freq_temp[2] + 1) % 10;
            if_refresh = true;
        } else if (lcd_act == 1 && lcd_subact == 3){
            freq_temp[3] = (freq_temp[3] + 1) % 10;
            if_refresh = true;
        }       // ���滻��switch, �����û���
    }

    if (key_code_pre != 4 && key_code == 4){
        switch (lcd_act) {
            case 1:
                lcd_subact = (lcd_subact + 4) % 5;
                if_refresh = true;
                break;
        }       // ���滻��if, �����û���
    }

    if (key_code_pre != 6 && key_code == 6){
        switch (lcd_act) {
            case 1:
                lcd_subact = (lcd_subact + 1) % 5;
                if_refresh = true;
                break;
        }
    }

    if (key_code_pre != 5 && key_code == 5 && lcd_act == 1 && lcd_subact == 4){
        uint16_t f_temp = 1000 * freq_temp[0] + 100 * freq_temp[1] + 10 * freq_temp[2] + freq_temp[3];
        if (f_temp >= 880 && f_temp <= 1080){   // �����Ϸ�
            for (i =0; i<4; ++i){
                freq[i] = freq_temp[i];     // ���²���
            }
            passFreq(freq);             // ��������FM����ģ��
            lcd_act = 0;
            lcd_subact = 0;
            if_refresh = true;
            clear_screen();
        }
        else {                              // �������Ϸ�
            for (i =0; i<4; ++i){
                freq_temp[i] = freq[i];     // ��ԭ����
            }
            lcd_act = 2;
            lcd_subact = 0;
            if_refresh = true;
        }
    }
}


// UART����

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
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);        // ʹ��UART6ģ��
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);        // ʹ�ܶ˿� P
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOP));	// �ȴ��˿� P׼�����

    GPIOPinConfigure(GPIO_PP0_U6RX);				  // ����PP0ΪUART6 RX����
    GPIOPinConfigure(GPIO_PP1_U6TX);    			  // ����PP1ΪUART6 TX����

    // ���ö˿� A�ĵ�0,1λ��PA0,PA1��ΪUART����
    GPIOPinTypeUART(GPIO_PORTP_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // �����ʼ�֡��ʽ����
    UARTConfigSetExpClk(UART6_BASE,
                        ui32SysClock,
                        9600,                  // �����ʣ�9600
                        (UART_CONFIG_WLEN_8 |    // ����λ��8
                         UART_CONFIG_STOP_ONE |  // ֹͣλ��1
                         UART_CONFIG_PAR_NONE)); // У��λ����

    IntEnable(INT_UART6); // UART6 �ж�����
    UARTIntEnable(UART6_BASE, UART_INT_RX | UART_INT_RT);// ʹ�� UART6 RX,RT �ж�

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

void UART6_Handler(void)
{
    uint32_t uart6_int_status;

    uart6_int_status = UARTIntStatus(UART6_BASE, true); // ȡ�ж�״̬
    UARTIntClear(UART6_BASE, uart6_int_status); // ���ж�״̬

    UARTStringGet(UART6_BASE, str);
    UARTStringPut(UART0_BASE, str);
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
    while(*cMessage != '\0') {
        UARTCharPut(ui32Base, *(cMessage++));
        SysCtlDelay(1 * ( ui32SysClock / 3000)); //ÿ��1�ַ���ʱ1ms
    }
//    UARTCharPut(ui32Base, '\r');
//    UARTCharPut(ui32Base, '\n');
}

//*****************************************************************************
//
// ����ԭ�ͣ�void UARTStringGet(uint32_t ui32Base,const char *cMessage)
// �������ܣ�UARTģ������ַ���
// ����������ui32Base��UARTģ��
//          str���������ַ���
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

void passFreq(uint8_t* f){
    UARTStringPut(UART6_BASE, "AT+FREQ=");
    if (f[0] == 0){
        UARTCharPut(UART6_BASE,f[1]+'0');
        UARTCharPut(UART6_BASE,f[2]+'0');
        UARTCharPut(UART6_BASE,f[3]+'0');
    }
    else {
        UARTCharPut(UART6_BASE,f[0]+'0');
        UARTCharPut(UART6_BASE,f[1]+'0');
        UARTCharPut(UART6_BASE,f[2]+'0');
        UARTCharPut(UART6_BASE,f[3]+'0');
    }
}