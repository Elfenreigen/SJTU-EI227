//*****************************************************************************
//
// tm1638.c - API for TM1638.
//
// Copyright��2020-2021, �Ϻ���ͨ��ѧ���ӹ���ϵʵ���ѧ����
//
// Author:	�Ϻ���ͨ��ѧ���ӹ���ϵʵ���ѧ����
// Version: 1.0.0.20201228
// Date��2020-12-28
// History��
//
//*****************************************************************************

#include "tm1638.h"

//*****************************************************************************
//
// ����ԭ�ͣ�void SysTick_Handler(void)
// �������ܣ�����ʾ���ֻ����ת��Ϊ��������ܵıʻ�ֵ
// ����������digit ��Ҫ��ʾ���ֻ����
// ��������ֵ�� ��Ҫ��ʾ���ֻ���ŵĹ�������ܵıʻ�ֵ
//
//*****************************************************************************
uint8_t TM1638_DigiSegment(uint8_t digit)
{
    uint8_t segment = 0;
    switch (digit)
    {
    case 0:
        segment = 0x3F;
        break;
    case 1:
        segment = 0x06;
        break;
    case 2:
        segment = 0x5B;
        break;
    case 3:
        segment = 0x4F;
        break;
    case 4:
        segment = 0x66;
        break;
    case 5:
        segment = 0x6D;
        break;
    case 6:
        segment = 0x7D;
        break;
    case 7:
        segment = 0x07;
        break;
    case 8:
        segment = 0x7F;
        break;
    case 9:
        segment = 0x6F;
        break;
    case 10:
        segment = 0x77;
        break;
    case 11:
        segment = 0x7C;
        break;
    case 12:
        segment = 0x39;
        break;
    case 13:
        segment = 0x5E;
        break;
    case 14:
        segment = 0x79;
        break;
    case 15:
        segment = 0x71;
        break;
    case '_':
        segment = 0x08;
        break;
    case '-':
        segment = 0x40;
        break;
    case ' ':
        segment = 0x00;
        break;
    case 'G':
        segment = 0x3D;
        break;
    case 'A':
        segment = 0x77;
        break;
    case 'I':
        segment = 0x06;
        break;
    case 'N':
        segment = 0x37;
        break;
    case 'F':
        segment = 0x71;
        break;
    case 'U':
        segment = 0x3E;
        break;
    case 'L':
        segment = 0x38;
        break;
    case 'R':
        segment = 0x50;
        break;
    case 'E':
        segment = 0x79;
        break;
    case 'D':
        segment = 0x5E;
        break;
    case 'Y':
        segment = 0x6E;
        break;
    case 'H':
        segment = 0x76;
        break;
    case 'o':
        segment = 0x5C;
        break;
    case 'b':
        segment = 0x7C;
        break;
    case 'C':
        segment = 0x39;
        break;
    default:
        segment = 0x00;
        break;
    }

    return segment;
}

//*****************************************************************************
//
// ����ԭ�ͣ�void TM1638_Serial_Input(uint8_t data)
// �������ܣ�TM1638������������
// ����������data 8λ���루�ӵ�λ����λ�������룩
// ��������ֵ�� ��
//
//*****************************************************************************
void TM1638_Serial_Input(uint8_t data)
{
    uint8_t i;

    GPIOPinTypeGPIOOutput(DIO_PIN_BASE, DIO_PIN);     // DIO����Ϊ���

    for(i = 0; i < 8; i++)
    {
        GPIOPinWrite(CLK_PIN_BASE, CLK_PIN, 0x0);       // CLK����ΪLow
        if(data & 0x01)
            GPIOPinWrite(DIO_PIN_BASE, DIO_PIN, DIO_PIN); // DIO����ΪHIGH
        else
            GPIOPinWrite(DIO_PIN_BASE, DIO_PIN, 0x0);     // DIO����ΪLow

        data >>= 1;

        GPIOPinWrite(CLK_PIN_BASE, CLK_PIN, CLK_PIN);   // CLK����ΪHigh
    }
}

//*****************************************************************************
//
// ����ԭ�ͣ�uint8_t TM1638_Serial_Output(void)
// �������ܣ�TM1638�����������
// ������������
// ��������ֵ�� 8λ�������
//
//*****************************************************************************
uint8_t TM1638_Serial_Output(void)
{
    uint8_t i;
    uint8_t temp = 0;

    GPIOPinTypeGPIOInput(DIO_PIN_BASE, DIO_PIN);     // DIO����Ϊ����

    for(i = 0; i < 8; i++)
    {
        temp >>= 1;
        GPIOPinWrite(CLK_PIN_BASE, CLK_PIN, 0x0);     // CLK����ΪLow
        GPIOPinWrite(CLK_PIN_BASE, CLK_PIN, CLK_PIN); // CLK����ΪHigh

        if( GPIOPinRead(DIO_PIN_BASE, DIO_PIN) )      // ��ȡֵΪ��1��
            temp |= 0x80;
        GPIOPinWrite(CLK_PIN_BASE, CLK_PIN, 0x0);     // CLK����ΪLow
    }

    return temp;
}

//*****************************************************************************
//
// ����ԭ�ͣ�uint8_t TM1638_Readkeyboard(void)
// �������ܣ���ȡ���̵�ǰ״̬
// ������������
// ��������ֵ�� ����ֵ��1-9����0����ǰû�м�������
//
//*****************************************************************************
uint8_t TM1638_Readkeyboard(void)
{
    uint8_t c[4], i, key_code = 0;

    GPIOPinWrite(STB_PIN_BASE, STB_PIN, 0x0);       // STB����ΪLow
    TM1638_Serial_Input(0x42);		                // ������ɨ���ݡ�����

    for(i = 0; i < 5; ++i) {};                      // �ʵ���ʱԼΪ1us

    for(i = 0; i < 4; i++)
        c[i] = TM1638_Serial_Output();              // ��ȡTM1638�����������

    GPIOPinWrite(STB_PIN_BASE, STB_PIN, STB_PIN);   // STB����ΪHigh

    // 4���ֽ����ݺϳ�һ���ֽ�
    if(c[0] == 0x04) key_code = 1;
    if(c[0] == 0x02) key_code = 2;
    if(c[0] == 0x01) key_code = 3;
    if(c[0] == 0x40) key_code = 4;
    if(c[0] == 0x20) key_code = 5;
    if(c[0] == 0x10) key_code = 6;
    if(c[1] == 0x04) key_code = 7;
    if(c[1] == 0x02) key_code = 8;
    if(c[1] == 0x01) key_code = 9;

    return key_code;    // key_code=0����ǰû�м�������
}

//*****************************************************************************
//
// ����ԭ�ͣ�void TM1638_RefreshDIGIandLED(uint8_t digit_buf[8],
//                                         uint8_t pnt_buf,
//                                         uint8_t led_buf[8])
// �������ܣ�ˢ��8λ����ܣ���С���㣩��8��LEDָʾ��
// ����������digit_buf[8] 8λ�������Ҫ��ʾ�����ֻ��ַ�
//           pnt_buf     С����
//           led_buf[8]  8��LEDָʾ��״̬��0���� 1������
// ��������ֵ�� ��
//
//*****************************************************************************
void TM1638_RefreshDIGIandLED(uint8_t digit_buf[8],
                              uint8_t pnt_buf,
                              uint8_t led_buf[8])
{
    uint8_t i, mask, buf[16];

    mask = 0x01;
    for(i = 0; i < 8; i++)
    {
        // �����
        buf[i * 2] = TM1638_DigiSegment(digit_buf[i]);
        if ((pnt_buf & mask) != 0) buf[i * 2] |= 0x80;
        mask = mask * 2;

        // ָʾ��
        buf[i * 2 + 1] = led_buf[i];
    }

    GPIOPinWrite(STB_PIN_BASE, STB_PIN, 0x0);     // STB����ΪLow
    TM1638_Serial_Input(0x40);                    // ���õ�ַģʽΪ�Զ���һ
    GPIOPinWrite(STB_PIN_BASE, STB_PIN, STB_PIN); // STB����ΪHigh

    GPIOPinWrite(STB_PIN_BASE, STB_PIN, 0x0);     // STB����ΪLow
    TM1638_Serial_Input(0xC0);                    // ������ʼ��ַ


    for (i = 0; i < 16; i++)
    {
        TM1638_Serial_Input(buf[i]);
    }
    GPIOPinWrite(STB_PIN_BASE, STB_PIN, STB_PIN); // STB����ΪHigh
}

// TM1638��ʼ������

//*****************************************************************************
//
// ����ԭ�ͣ�void TM1638_Init(void)
// �������ܣ�TM1638��ʼ������
// ������������
// ��������ֵ����
//
//*****************************************************************************
void TM1638_Init(void)
{
    GPIOPinWrite(STB_PIN_BASE, STB_PIN, 0x0);     // STB����ΪLow
    TM1638_Serial_Input(0x8A);  // �������� (0x88-0x8f)8�����ȿɵ�
    GPIOPinWrite(STB_PIN_BASE, STB_PIN, STB_PIN); // STB����ΪHigh
}
