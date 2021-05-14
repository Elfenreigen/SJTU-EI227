//*****************************************************************************
//
// tm1638.c - API for TM1638.
//
// Copyright：2020-2021, 上海交通大学电子工程系实验教学中心
// 
// Author:	上海交通大学电子工程系实验教学中心
// Version: 1.0.0.20201228 
// Date：2020-12-28
// History：
//
//*****************************************************************************

#include "tm1638.h"

//*****************************************************************************
// 
// 函数原型：void SysTick_Handler(void)
// 函数功能：将显示数字或符号转换为共阴数码管的笔画值
// 函数参数：digit 需要显示数字或符号
// 函数返回值： 需要显示数字或符号的共阴数码管的笔画值
//
//*****************************************************************************
uint8_t TM1638_DigiSegment(uint8_t digit)
{
	uint8_t segment = 0;
	switch (digit)
  {
	  case 0:  segment = 0x3F; break;
	  case 1:  segment = 0x06; break;
	  case 2:  segment = 0x5B; break;
	  case 3:  segment = 0x4F; break;
	  case 4:  segment = 0x66; break;
	  case 5:  segment = 0x6D; break;
	  case 6:  segment = 0x7D; break;
	  case 7:  segment = 0x07; break;
	  case 8:  segment = 0x7F; break;
	  case 9:  segment = 0x6F; break;
	  case 10: segment = 0x77; break;
	  case 11: segment = 0x7C; break;
	  case 12: segment = 0x39; break;
	  case 13: segment = 0x5E; break;
	  case 14: segment = 0x79; break;
	  case 15: segment = 0x71; break;
	  case '_':segment = 0x08; break;
	  case '-':segment = 0x40; break;
	  case ' ':segment = 0x00; break;
	  case 'G':segment = 0x3D; break;
	  case 'A':segment = 0x77; break;
	  case 'I':segment = 0x06; break;
	  case 'N':segment = 0x37; break;
	  case 'F':segment = 0x71; break;
	  case 'U':segment = 0x3E; break;
	  case 'L':segment = 0x38; break;
	  case 'R':segment = 0x50; break;
	  case 'E':segment = 0x79; break;
	  case 'D':segment = 0x5E; break;
	  case 'Y':segment = 0x6E; break;
	  case 'H':segment = 0x76; break;
	  case 'o':segment = 0x5C; break;
	  case 'b':segment = 0x7C; break;
	  case 'C':segment = 0x39; break;
	  default: segment = 0x00; break;
	}

	return segment;
}

//*****************************************************************************
// 
// 函数原型：void TM1638_Serial_Input(uint8_t data)
// 函数功能：TM1638串行数据输入
// 函数参数：data 8位输入（从低位到高位依次输入）
// 函数返回值： 无
//
//*****************************************************************************
void TM1638_Serial_Input(uint8_t data)
{
	uint8_t i;

	GPIOPinTypeGPIOOutput(DIO_PIN_BASE, DIO_PIN);     // DIO设置为输出
	
	for(i = 0; i < 8; i++)
	{
		GPIOPinWrite(CLK_PIN_BASE, CLK_PIN, 0x0);       // CLK设置为Low
		if(data & 0x01)
			GPIOPinWrite(DIO_PIN_BASE, DIO_PIN, DIO_PIN); // DIO设置为HIGH
		else
			GPIOPinWrite(DIO_PIN_BASE, DIO_PIN, 0x0);     // DIO设置为Low
	
		data >>= 1;
		
		GPIOPinWrite(CLK_PIN_BASE, CLK_PIN, CLK_PIN);   // CLK设置为High
	}
}

//*****************************************************************************
// 
// 函数原型：uint8_t TM1638_Serial_Output(void)
// 函数功能：TM1638串行数据输出
// 函数参数：无
// 函数返回值： 8位串行输出
//
//*****************************************************************************
uint8_t TM1638_Serial_Output(void)
{
	uint8_t i;
	uint8_t temp = 0;
	
	GPIOPinTypeGPIOInput(DIO_PIN_BASE, DIO_PIN);     // DIO设置为输入
	
	for(i = 0; i < 8; i++)
	{
		temp >>= 1;
		GPIOPinWrite(CLK_PIN_BASE, CLK_PIN, 0x0);     // CLK设置为Low
		GPIOPinWrite(CLK_PIN_BASE, CLK_PIN, CLK_PIN); // CLK设置为High
		
		if( GPIOPinRead(DIO_PIN_BASE, DIO_PIN) )      // 读取值为‘1’
			temp |= 0x80;
		GPIOPinWrite(CLK_PIN_BASE, CLK_PIN, 0x0);     // CLK设置为Low
	}
	
	return temp;
}

//*****************************************************************************
// 
// 函数原型：uint8_t TM1638_Readkeyboard(void)
// 函数功能：读取键盘当前状态
// 函数参数：无
// 函数返回值： 按键值（1-9），0代表当前没有键被按下 
//
//*****************************************************************************
uint8_t TM1638_Readkeyboard(void)
{
	uint8_t c[4], i, key_code = 0;
	
	GPIOPinWrite(STB_PIN_BASE, STB_PIN, 0x0);       // STB设置为Low
	TM1638_Serial_Input(0x42);		                // “读键扫数据”命令
	
	for(i = 0; i < 5; ++i){};                       // 适当延时约为1us
		
	for(i = 0; i < 4; i++)                          		
		c[i] = TM1638_Serial_Output();              // 读取TM1638串行数据输出
	
	GPIOPinWrite(STB_PIN_BASE, STB_PIN, STB_PIN);   // STB设置为High
	
	// 4个字节数据合成一个字节
	if(c[0] == 0x04) key_code = 1;
	if(c[0] == 0x02) key_code = 2;
	if(c[0] == 0x01) key_code = 3;
	if(c[0] == 0x40) key_code = 4;
	if(c[0] == 0x20) key_code = 5;
	if(c[0] == 0x10) key_code = 6;
	if(c[1] == 0x04) key_code = 7;
	if(c[1] == 0x02) key_code = 8;
	if(c[1] == 0x01) key_code = 9;
	
	return key_code;    // key_code=0代表当前没有键被按下
}

//*****************************************************************************
// 
// 函数原型：void TM1638_RefreshDIGIandLED(uint8_t digit_buf[8],
//                                         uint8_t pnt_buf,
//                                         uint8_t led_buf[8])
// 函数功能：刷新8位数码管（含小数点）和8个LED指示灯
// 函数参数：digit_buf[8] 8位数码管需要显示的数字或字符
//           pnt_buf     小数点    
//           led_buf[8]  8个LED指示灯状态（0：灭 1：亮）
// 函数返回值： 无
//
//*****************************************************************************
void TM1638_RefreshDIGIandLED(uint8_t digit_buf[8],
                              uint8_t pnt_buf,
                              uint8_t led_buf[8])
{
	uint8_t i, mask, buf[16];

	mask = 0x01;
	for(i = 0;i < 8; i++)
	{
		// 数码管
		buf[i * 2] = TM1638_DigiSegment(digit_buf[i]);
		if ((pnt_buf & mask) != 0) buf[i * 2] |= 0x80;
		mask = mask * 2;

		// 指示灯
		buf[i * 2 + 1] = led_buf[i];
	}
	
	GPIOPinWrite(STB_PIN_BASE, STB_PIN, 0x0);     // STB设置为Low
	TM1638_Serial_Input(0x40);                    // 设置地址模式为自动加一
	GPIOPinWrite(STB_PIN_BASE, STB_PIN, STB_PIN); // STB设置为High   
	
	GPIOPinWrite(STB_PIN_BASE, STB_PIN, 0x0);     // STB设置为Low
	TM1638_Serial_Input(0xC0);                    // 设置起始地址
	
	
	for (i = 0; i < 16; i++)
	{
		TM1638_Serial_Input(buf[i]);
	}
	GPIOPinWrite(STB_PIN_BASE, STB_PIN, STB_PIN); // STB设置为High
}

// TM1638初始化函数

//*****************************************************************************
// 
// 函数原型：void TM1638_Init(void)
// 函数功能：TM1638初始化函数
// 函数参数：无
// 函数返回值：无 
//
//*****************************************************************************
void TM1638_Init(void)
{
	GPIOPinWrite(STB_PIN_BASE, STB_PIN, 0x0);     // STB设置为Low
	TM1638_Serial_Input(0x8A);  // 设置亮度 (0x88-0x8f)8级亮度可调
	GPIOPinWrite(STB_PIN_BASE, STB_PIN, STB_PIN); // STB设置为High
}
