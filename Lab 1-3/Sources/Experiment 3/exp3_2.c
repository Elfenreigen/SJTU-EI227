//*****************************************************************************
//
// 头文件
//
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"        // 基址宏定义
#include "inc/hw_ints.h"
#include "inc/hw_types.h"         // 数据类型宏定义，寄存器访问函数
#include "driverlib/debug.h"      // 调试用
#include "driverlib/gpio.h"       // 通用IO口宏定义和函数原型
#include "driverlib/pin_map.h"    // TM4C系列MCU外围设备管脚宏定义
#include "driverlib/sysctl.h"	    // 系统控制定义
#include "driverlib/systick.h"    // SysTick Driver 原型
#include "driverlib/interrupt.h"  // NVIC Interrupt Controller Driver 原型
#include "driverlib/uart.h"       // 与UART有关的宏定义和函数原型

#include "tm1638.h"               // 与控制TM1638芯片有关的宏定义和函数原型

//*****************************************************************************
//
// 宏定义
//
//*****************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTick频率为50Hz，即循环定时周期20ms

#define V_T1000ms	50                  // 1s软件定时器溢出值，50个20ms

//*****************************************************************************
//
// 函数原型声明
//
//*****************************************************************************
void GPIOInit(void);        // GPIO初始化
void SysTickInit(void);     // 设置SysTick中断 
void DevicesInit(void);     // MCU器件初始化，注：会调用上述函数
void UARTInit(void);        // UART初始化
void UARTStringPut(uint32_t ui32Base,const char *cMessage);// 向UART发送字符串

void digitWrite(int pos, int num);              //写数码管，使位置与具体位置对应
void SetTime(uint32_t ui32Base, char* s);       //设置时间
void IncTime(uint32_t ui32Base, const char* addition);       //增加时间
void GetTime(uint32_t ui32Base);       //获得时间
bool strCmp(const char* s1, const char* s2);    //字符串比较函数
bool isTimeLegal(const char *s);                //检查时间是否合法

//*****************************************************************************
//
// 变量定义
//
//*****************************************************************************

// 8位数码管显示的数字或字母符号
// 注：板上数码位从左到右序号排列为4、5、6、7、0、1、2、3
uint8_t digit[8]={' ',' ',' ',' ',' ',' ',' ',' '};

// 8位小数点 1亮  0灭
// 注：板上数码位小数点从左到右序号排列为4、5、6、7、0、1、2、3
uint8_t pnt = 0xA0;

// 系统时钟频率 
uint32_t ui32SysClock;

// 秒检测
uint8_t clock1000ms;

uint8_t led[] = {0, 0, 0, 0, 0, 0, 0, 0};

// 存储接受发送字符串用
char str[50] = {'\0'};

// 板子时间
int board_time = 0;

// 检测读取奇偶，奇读偶不读
bool if_read = false;

//*****************************************************************************
//
// 主程序
//
//*****************************************************************************
int main(void)
{
    

    DevicesInit();            //  MCU器件初始化
	
    SysCtlDelay(60 * ( ui32SysClock / 3000)); // 延时>60ms,等待TM1638上电完成
    TM1638_Init();	          // 初始化TM1638
	
    while (1){}
	
}

//*****************************************************************************
//
// 函数原型：void GPIOInit(void)
// 函数功能：GPIO初始化。使能PortK，设置PK4,PK5为输出；使能PortM，设置PM0为输出。
//          （PK4连接TM1638的STB，PK5连接TM1638的DIO，PM0连接TM1638的CLK）
// 函数参数：无
// 函数返回值：无
//
//*****************************************************************************
void GPIOInit(void)
{
	//配置TM1638芯片管脚
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);				// 使能端口 K	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)){};		// 等待端口 K准备完毕		
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);				// 使能端口 M	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)){};		// 等待端口 M准备完毕		
	
    // 设置端口 K的第4,5位（PK4,PK5）为输出引脚		PK4-STB  PK5-DIO
	GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_4|GPIO_PIN_5);
	// 设置端口 M的第0位（PM0）为输出引脚   PM0-CLK
	GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_0);	
}

//*****************************************************************************
// 
// 函数原型：SysTickInit(void)
// 函数功能：设置SysTick中断
// 函数参数：无
// 函数返回值：无
//
//*****************************************************************************
void SysTickInit(void)
{
	SysTickPeriodSet(ui32SysClock/SYSTICK_FREQUENCY); // 设置心跳节拍,定时周期20ms
	SysTickEnable();  			// SysTick使能
	SysTickIntEnable();			// SysTick中断允许
}

//*****************************************************************************
//
// 函数原型：void UARTStringPut(uint32_t ui32Base,const char *cMessage)
// 函数功能：向UART模块发送字符串
// 函数参数：ui32Base：UART模块
//           cMessage：待发送字符串
// 函数返回值：无
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
// 函数原型：void UARTStringGet(uint32_t ui32Base,const char *cMessage)
// 函数功能：UART模块接收字符串
// 函数参数：ui32Base：UART模块
//           str：待接收字符串
// 函数返回值：无
//
//*****************************************************************************
void UARTStringGet(uint32_t ui32Base, char* s) {
//    char* temp = s;
    while (UARTCharsAvail(ui32Base)) // 重复从接收FIF0读取字符
    {
        *s = (char) UARTCharGetNonBlocking(ui32Base); // 读入一个字符
        ++s;
        SysCtlDelay(1 * ( ui32SysClock / 3000)); //每读1字符延时1ms
    }
    *s = '\0';
//    UARTStringPut(UART0_BASE, (const char *)"Get String:");
//    UARTStringPut(UART0_BASE, temp);
}

//*****************************************************************************
//
// 函数原型：void UARTInit(void)
// 函数功能：UART初始化。使能UART0，设置PA0,PA1为UART0 RX,TX引脚；
//          设置波特率及帧格式。
// 函数参数：无
// 函数返回值：无
//
//*****************************************************************************
void UARTInit(void)
{
    // 引脚配置
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);        // 使能UART0模块   
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);        // 使能端口 A
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));	// 等待端口 A准备完毕 

    GPIOPinConfigure(GPIO_PA0_U0RX);				  // 设置PA0为UART0 RX引脚
    GPIOPinConfigure(GPIO_PA1_U0TX);    			  // 设置PA1为UART0 TX引脚        

	// 设置端口 A的第0,1位（PA0,PA1）为UART引脚
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);   

    // 波特率及帧格式设置
    UARTConfigSetExpClk(UART0_BASE, 
	                    ui32SysClock,
	                    115200,                  // 波特率：115200
	                    (UART_CONFIG_WLEN_8 |    // 数据位：8
	                     UART_CONFIG_STOP_ONE |  // 停止位：1
	                     UART_CONFIG_PAR_NONE)); // 校验位：无
	
    // 初始化完成后向PC端发送"Hello, 2A!"字符串
    UARTStringPut(UART0_BASE, (const char *)"Hello, 2A!");
		
		IntEnable(INT_UART0); // UART0 中断允许
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);// 使能 UART0 RX,RT 中断
}

//*****************************************************************************
// 
// 函数原型：DevicesInit(void)
// 函数功能：CU器件初始化，包括系统时钟设置、GPIO初始化和SysTick中断设置
// 函数参数：无
// 函数返回值：无
//
//*****************************************************************************
void DevicesInit(void)
{
	// 使用外部25MHz主时钟源，经过PLL，然后分频为20MHz
	ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN | 
	                                   SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480), 
	                                   20000000);

  	GPIOInit();             // GPIO初始化
    SysTickInit();          // 设置SysTick中断
    UARTInit();             // UART初始化 
    IntMasterEnable();			// 总中断允许
}

void SysTick_Handler(void)
{
    if (++clock1000ms >= V_T1000ms) {                   //一秒过去了
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

	  uart0_int_status = UARTIntStatus(UART0_BASE, true); // 取中断状态
    UARTIntClear(UART0_BASE, uart0_int_status); // 清中断状态

    UARTStringGet(UART0_BASE, str);
//    UARTStringPut(UART0_BASE, str);

    //提取字符串中命令部分
    for (i = 0; i < 6; ++i) {
        cmd[i] = str[i];
    }
    cmd[6] = '\0';
//    UARTStringPut(UART0_BASE, (const char *)"command:");
//    UARTStringPut(UART0_BASE, cmd);

    //提取字符串中时间部分
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