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
#include "inc/hw_memmap.h"        // ��ַ�궨��
#include "inc/hw_types.h"         // �������ͺ궨�壬�Ĵ������ʺ���
#include "driverlib/debug.h"      // ������
#include "driverlib/gpio.h"       // ͨ��IO�ں궨��
#include "driverlib/pin_map.h"    // TM4Cϵ��MCU��Χ�豸�ܽź궨��
#include "driverlib/sysctl.h"	  // ϵͳ���ƶ���
#include "driverlib/systick.h"    // SysTick Driver ԭ��
#include "driverlib/interrupt.h"  // NVIC Interrupt Controller Driver ԭ��

#include "tm1638.h"               // �����TM1638оƬ�йصĺ���
#include "JLX12864_2.c"
#include "modes_3.c"

//*****************************************************************************
//
// �궨��
//
//*****************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTickƵ��Ϊ50Hz����ѭ����ʱ����20ms

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
void key5();                // ȷ�������º�ִ�иú���
//*****************************************************************************
//
// ��������
//
//*****************************************************************************
uint8_t sjtu[128*8];
uint8_t bit[8];
// �����ʱ������
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
bool if_count_5s = false;      // �Ƿ�ʼ����
bool if_count_10s = false;

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
uint8_t pnt = 0x04;

// 8��LEDָʾ��״̬��0��1��
// ע������ָʾ�ƴ������������Ϊ7��6��5��4��3��2��1��0
//     ��ӦԪ��LED8��LED7��LED6��LED5��LED4��LED3��LED2��LED1
uint8_t led[] = {1, 1, 1, 1, 1, 1, 1, 0};

// ��ǰ����ֵ
uint8_t key_code = 0;

// ϵͳʱ��Ƶ�� 
uint32_t ui32SysClock;

// ����������
uint8_t* letter[3]={"A", "B", "C"};
uint8_t* number[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
uint8_t let = 0;
uint8_t integer = 1;
uint8_t decimal = 0;
uint8_t let_temp = 0;   // ������ʱ��������ȡ��ʱ�ָ�ԭ����
uint8_t int_temp = 1;
uint8_t dec_temp = 0;

// ���庯��ָ����������л�ģʽ
void (*func[5][4])(uint8_t*, uint8_t*, uint8_t*) = {0};

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
    func[0][1] = mode0_1;
    func[1][0] = mode1_0;
    func[1][1] = mode1_1;
    func[1][2] = mode1_2;
    func[2][0] = mode2_0;
    func[2][1] = mode2_1;
    func[2][2] = mode2_2;
    func[3][0] = mode3_0;
    func[3][1] = mode3_1;
    func[3][2] = mode3_2;
    func[3][3] = mode3_3;
    func[4][0] = mode4;

    while (1)
    {

        if (clock100ms_flag == 1)      // ���0.1�붨ʱ�Ƿ�
        {
            clock100ms_flag		= 0;
            // ÿ0.1���ۼӼ�ʱֵ�����������ʮ������ʾ���м�����ʱ��ͣ��ʱ
            if (key_code == 0)
            {
                if (++test_counter >= 10000) test_counter = 0;
                digit[0] = test_counter / 1000; 	    // �����λ��
                digit[1] = test_counter / 100 % 10; 	// ����ʮλ��
                digit[2] = test_counter / 10 % 10; 	  // �����λ��
                digit[3] = test_counter % 10;         // ����ٷ�λ��
            }
        }

        if (clock500ms_flag == 1)   // ���0.5�붨ʱ�Ƿ�
        {
            clock500ms_flag = 0;
            // 8��ָʾ��������Ʒ�ʽ��ÿ0.5�����ң�ѭ�����ƶ�һ��
            temp = led[0];
            for (i = 0; i < 7; i++) led[i] = led[i + 1];
            led[7] = temp;
        }


        if (if_refresh || (if_count_5s && clock5s_flag) || (if_count_10s && clock10s_flag))
        {
            if (if_count_5s && clock5s_flag){
                lcd_act = 3;
                lcd_subact = 0;
                if_count_5s = false;
            }
            if (if_count_10s && clock10s_flag){
                lcd_act = 0;
                lcd_subact = 0;
                if_count_10s = false;
            }
            if_refresh = false;
            TEST_H;

            func[lcd_act][lcd_subact](letter[let], number[integer], number[decimal]);

            if (lcd_act == 0 && lcd_subact == 1) {if_count_10s = true; if_count_5s = false; clock10s = 0; clock10s_flag = false;}
            else if (lcd_act == 4) {if_count_5s = true; if_count_10s = false; clock5s = 0; clock5s_flag = false;}
            else if_count_5s = if_count_10s = false;
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

    // ˢ��ȫ������ܺ�LEDָʾ��
    TM1638_RefreshDIGIandLED(digit, pnt, led);

    // ��鵱ǰ�������룬0�����޼�������1-9��ʾ�ж�Ӧ����
    // ������ʾ��һλ�������
    key_code = TM1638_Readkeyboard();

    digit[4] = lcd_act;
    digit[5] = lcd_subact;
    digit[6] = (int)if_count_5s;
    digit[7] = (int)if_count_10s;

    // �������л�
    if (key_code_pre == 0 && key_code != 0 && lcd_act == 0 && lcd_subact == 0){
        lcd_subact = 1;
        if_refresh = true;
        return;
    }

    if (key_code_pre != 8 && key_code == 8){
        if (lcd_act == 2 && lcd_subact == 0){
            let = (let + 2) % 3;
            if_refresh = true;
        } else if (lcd_act == 3 && lcd_subact == 0){
            integer = (integer + 9) % 10;
            if_refresh = true;
        } else if (lcd_act == 3 && lcd_subact == 1){
            decimal = (decimal + 9) % 10;
            if_refresh = true;
        }
    }
    if (key_code_pre != 2 && key_code == 2){
        if (lcd_act == 2 && lcd_subact == 0){
            let = ++let % 3;
            if_refresh = true;
        } else if (lcd_act == 3 && lcd_subact == 0){
            integer = ++integer % 10;
            if_refresh = true;
        } else if (lcd_act == 3 && lcd_subact == 1){
            decimal = ++decimal % 10;
            if_refresh = true;
        }
    }
    if (key_code_pre != 4 && key_code == 4){
        switch (lcd_act) {
            case 1:
                lcd_subact = (lcd_subact + 2) % 3;
                if_refresh = true;
                break;
            case 2:
                lcd_subact = (lcd_subact + 2) % 3;
                if_refresh = true;
                break;
            case 3:
                lcd_subact = (lcd_subact + 3) % 4;
                if_refresh = true;
                break;
        }
    }
    if (key_code_pre != 6 && key_code == 6){
        switch (lcd_act) {
            case 1:
                lcd_subact = (lcd_subact + 1) % 3;
                if_refresh = true;
                break;
            case 2:
                lcd_subact = (lcd_subact + 1) % 3;
                if_refresh = true;
                break;
            case 3:
                lcd_subact = (lcd_subact + 1) % 4;
                if_refresh = true;
                break;
        }
    }
    if (key_code_pre != 5 && key_code == 5) key5();

}

void key5(){
    switch (lcd_act) {
        case 0:
            switch (lcd_subact) {
                case 1:
                    lcd_act = 1;
                    lcd_subact = 0;
                    if_refresh = true;
                    break;
            }
            break;
        case 1:
            switch (lcd_subact) {
                case 0:
                    let_temp = let;
                    lcd_act = 2;
                    lcd_subact = 0;
                    if_refresh = true;
                    break;
                case 1:
                    int_temp = integer;
                    dec_temp = decimal;
                    lcd_act = 3;
                    lcd_subact = 0;
                    if_refresh = true;
                    break;
                case 2:
                    lcd_act = 0;
                    lcd_subact = 0;
                    if_refresh = true;
                    break;
            }
            break;
        case 2:
            switch (lcd_subact) {
                case 1:     // ȷ��
                    lcd_act = 1;
                    lcd_subact = 2;
                    if_refresh = true;
                    break;
                case 2:     // ȡ��
                    lcd_act = 1;
                    lcd_subact = 2;
                    let = let_temp;     //�ָ�����
                    if_refresh = true;
                    break;
            }
            break;
        case 3:
            switch (lcd_subact) {
                case 2:     // ȷ��
                    if (10 * integer + decimal >= 10 && 10 * integer + decimal <= 90) {      // �����Ϸ�
                        lcd_act = 1;
                        lcd_subact = 2;
                    }
                    else {
                        lcd_act = 4;
                        lcd_subact = 0;
                    }
                    if_refresh = true;
                    break;
                case 3:     // ȡ��
                    lcd_act = 1;
                    lcd_subact = 2;
                    integer = int_temp;
                    decimal = dec_temp;
                    if_refresh = true;
                    break;
            }
            break;
        default:
            break;
    }
}
