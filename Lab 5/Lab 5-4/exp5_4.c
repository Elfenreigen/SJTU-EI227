//*******************************************************************************************************
//
// Copyright: 2021-2022, 上海交通大学电子工程系实验教学中心
// File name: Frequency_Measure.c
// Description: 本示例展示如何使用测频率法实现方波频率的测量（待测方波信号连接到T0CCP0/PL4引脚）
// 1.每次按下A2000TM4C底板按键1(SW1)后,程序开始测量方波信号的频率，测量完成后数码管显示测量结果,单位为Hz;
// 2.如果测试频率不在【10，9999】范围内容，则显示Err。
// 3.注意事项：待测方波信号高电平不宜高于3.3V，低电平不宜低于0V，并且一定要与底板共地
//             该程序不能用于检测<=1Hz频率的方波，频率检测误差率正负1Hz
// Author:	上海交通大学电子工程系实验教学中心
// Version: 1.0.0.20210513 
// Date：2021-05-13
// History：
//
//*******************************************************************************************************

//*******************************************************************************************************
//
// 头文件
//
//*******************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"        // 基址宏定义
#include "inc/hw_types.h"         // 数据类型宏定义，寄存器访问函数
#include "inc/hw_timer.h"         // 与定时器有关的宏定义
#include "inc/hw_ints.h"          // 与中断有关的宏定义
#include "driverlib/debug.h"      // 调试用
#include "driverlib/gpio.h"       // 通用IO口宏定义和函数原型
#include "driverlib/pin_map.h"    // TM4C系列MCU外围设备管脚宏定义
#include "driverlib/sysctl.h"	  // 系统控制定义
#include "driverlib/systick.h"    // SysTick Driver 函数原型
#include "driverlib/interrupt.h"  // NVIC中断控制驱动函数原型
#include "driverlib/timer.h"      // 与Timer有关的函数原型  
#include "driverlib/pwm.h"        // 与Timer有关的函数原型
#include "driverlib/uart.h"
#include "driverlib/fpu.h"

#include "tm1638.h"               // 与控制TM1638芯片有关的宏定义和函数原型

//*******************************************************************************************************
//
// 宏定义
//
//*******************************************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTick频率为50Hz，即循环定时周期20ms

#define FREQUENCY_MIN     10           // 有效频率最小值，根据实际需要设置 
#define FREQUENCY_MAX     9999         // 有效频率最大值，根据实际需要设置 
//*******************************************************************************************************
//
// 函数原型声明
//
//*******************************************************************************************************
void GPIOInit(void);        // GPIO初始化
void SysTickInit(void);     // 设置SysTick中断
void Timer0Init(void);      // Timer0初始化
void DevicesInit(void);     // MCU器件初始化，注：会调用上述函数
void PWMInit(uint32_t ui32Freq_Hz);  // 产生频率为ui32Freq_Hz的方波

//*******************************************************************************************************
//
// 变量定义
//
//*******************************************************************************************************

// 8位数码管显示的数字或字母符号
// 注：板上数码位从左到右序号排列为4、5、6、7、0、1、2、3
uint8_t digit[8]={' ',' ',' ','H',' ',' ',' ',' '};

// 8位小数点 1亮  0灭
// 注：板上数码位小数点从左到右序号排列为4、5、6、7、0、1、2、3
uint8_t pnt = 0x00;

// 8个LED指示灯状态，0灭，1亮
// 注：板上指示灯从左到右序号排列为7、6、5、4、3、2、1、0
//     对应元件LED8、LED7、LED6、LED5、LED4、LED3、LED2、LED1
uint8_t led[] = {0, 0, 0, 0, 0, 0, 0, 0};

// 当前按键值
volatile uint8_t key_code = 0;

// 用于记录前一次按键检测时的键盘状态，0表示无键按下，1有键按下
volatile uint8_t key_state = 0;

// 按键操作有效标记，0代表无新操作，1代表有键操作
volatile uint8_t  key_flag = 0;

// 系统时钟频率 
uint32_t g_ui32SysClock;

// 1s计数结束标志
volatile uint8_t g_ui8INTStatus = 0;

// 保存上一次TIMER0边沿计数值
volatile uint32_t g_ui32TPreCount = 0;

// 保存本次TIMER0边沿计数值
volatile uint32_t g_ui32TCurCount = 0;

//*******************************************************************************************************
//
// 主程序
//
//*******************************************************************************************************
int main(void)
{
    uint32_t ui32Freq;  // 记录测量的方波频率
       
    DevicesInit();            //  MCU器件初始化
	
    SysCtlDelay(60 * ( g_ui32SysClock / 3000));   // 延时>60ms,等待TM1638上电完成
    TM1638_Init();	          // 初始化TM1638
	    
    //PWMInit(1234);   //  产生3998Hz方波，当用户没有信号发生器时用于产生测试信号,可以自行修改频率
        
    while (1)
    {	
        if(key_flag == 1)    // 有键按下
        {
            key_flag = 0;   
            
            if(key_code == 1)  // SW1按下，启动Timer0A
            {

                TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT); // Timer1A 超时中断使能
                IntEnable(INT_TIMER1A);                 // 开启 TIMER1A 中断源
                TimerEnable(TIMER1_BASE, TIMER_A);      // TIMER1 开始计时
                
                TimerEnable(TIMER0_BASE, TIMER_A);       // TIMER0 开始计数   
            }                   
        }
                
        if(g_ui8INTStatus == 1)   // 1s定时结束，开始计算频率
        {
            g_ui8INTStatus = 0;
            
            ui32Freq = g_ui32TCurCount >= g_ui32TPreCount ?
                      (g_ui32TCurCount - g_ui32TPreCount):(g_ui32TCurCount - g_ui32TPreCount + 0xFFFF);
            
            if((ui32Freq >= FREQUENCY_MIN) && (ui32Freq <= FREQUENCY_MAX))
            {
                digit[4] = ui32Freq / 1000 % 10;    // 计算千位数
			    digit[5] = ui32Freq / 100 % 10;     // 计算百位数                			    
                digit[6] = ui32Freq / 10 % 10; 	    // 计算十位数
			    digit[7] = ui32Freq % 10;           // 计算个位数
           
            }
            else  // 测量频率超过指定范围
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
// 函数原型：void GPIOInit(void)
// 函数功能：GPIO初始化。使能PortK，设置PK4,PK5为输出；使能PortM，设置PM0为输出。
//          （PK4连接TM1638的STB，PK5连接TM1638的DIO，PM0连接TM1638的CLK）
// 函数参数：无
// 函数返回值：无
//
//*******************************************************************************************************
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

//*******************************************************************************************************
//
// 函数原型：void Timer0Init(void)
// 函数功能：设置Timer0为输入边沿（上升沿）计数模式，T0CCP0(PL4)为捕获引脚
// 函数参数：无
// 函数返回值：无
//
//*******************************************************************************************************
void Timer0Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);  // 使能TIMER0
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);   // 使能GPIOL
    
    GPIOPinConfigure(GPIO_PL4_T0CCP0);             // 配置引脚复用   
    GPIOPinTypeTimer(GPIO_PORTL_BASE, GPIO_PIN_4); // 引脚映射
    GPIOPadConfigSet(GPIO_PORTL_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); // 将引脚弱上拉
    
    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT_UP); // 半长定时器，增计数
    //TimerPrescaleSet(TIMER0_BASE, TIMER_A, 255);   // 预分频256
    TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE); // 初始化配置为捕捉上升沿        
}

//*******************************************************************************************************
//
// 函数原型：void Timer1Init(void)
// 函数功能：设置Timer1为一次性定时器，定时周期为1s
// 函数参数：无
// 函数返回值：无
//
//*******************************************************************************************************
void Timer1Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);  // TIMER1 使能
       
    TimerConfigure(TIMER1_BASE, TIMER_CFG_ONE_SHOT); // 设置为 32 位 1次 定时器
    TimerLoadSet(TIMER1_BASE, TIMER_A, g_ui32SysClock);  // TIMER1A装载计数值1s        
}

//*******************************************************************************************************
// 
// 函数原型：SysTickInit(void)
// 函数功能：设置SysTick中断
// 函数参数：无
// 函数返回值：无
//
//*******************************************************************************************************
void SysTickInit(void)
{
	SysTickPeriodSet(g_ui32SysClock/SYSTICK_FREQUENCY); // 设置心跳节拍,定时周期20ms
	SysTickEnable();  			// SysTick使能
	SysTickIntEnable();			// SysTick中断允许
}

//*******************************************************************************************************
// 
// 函数原型：DevicesInit(void)
// 函数功能：MCU器件初始化，包括系统时钟设置、GPIO初始化和SysTick中断设置
// 函数参数：无
// 函数返回值：无
//
//*******************************************************************************************************
void DevicesInit(void)
{
	// 使用外部25MHz主时钟源，经过PLL，然后分频为16MHz
	g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN | 
	                                   SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480), 
	                                   16000000);

    //UART_Init();//串口初始化
	GPIOInit();             // GPIO初始化
    Timer0Init();           // Timer0初始化
    Timer1Init();
    SysTickInit();          // 设置SysTick中断
    IntMasterEnable();	    // 总中断允许
        
    FPULazyStackingEnable();
    FPUEnable();//使能FPU
    
    IntPrioritySet(INT_TIMER1A,0x00);		// 设置INT_TIMER0A最高优先级   
    IntPrioritySet(INT_TIMER0A,0x01);		// 设置INT_TIMER0A最高优先级
    IntPrioritySet(FAULT_SYSTICK,0xe0);	    // 设置SYSTICK优先级低于INT_TIMER0A的优先级    
}

//*******************************************************************************************************
// 
// 函数原型：void SysTick_Handler(void)
// 函数功能：SysTick中断服务程序，检测按键动作，刷新数码管显示
// 函数参数：无
// 函数返回值：无
//
//*******************************************************************************************************
void SysTick_Handler(void)    // 定时周期为20ms
{

	// 刷新全部数码管和LED指示灯
	TM1638_RefreshDIGIandLED(digit, pnt, led);

	// 检查当前键盘输入，0代表无键操作，1-9表示有对应按键
	// 键号显示在一位数码管上
	key_code = TM1638_Readkeyboard();

	// 按键操作在SysTick中断服务程序中的状态转移处理程序
	
	// key_state用于记录前一次按键检测时的键盘状态，0表示无键按下，1有键按下
	switch (key_state)
	{
        case 0:   // 前一次按键检测时无键按下
            if (key_code > 0)   // 本次按键检测有键按下
            {
                key_state = 1;
                key_flag = 1;			
            }
		    break;
        case 1:    // 前一次按键检测时有键按下
            if (key_code == 0)  // 本次按键检测时无键按下
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
// 函数原型：void TIMER1A_Handler(void)
// 函数功能：Timer1A中断服务程序，记录捕获方波上升沿时定时器（TIMER0）的计数值
// 函数参数：无
// 函数返回值：无
//
//*******************************************************************************************************
void TIMER1A_Handler(void)
{
    
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);     // 清除中断标志
    g_ui32TPreCount = g_ui32TCurCount;   // 保存上一次TIMER0边沿计数值               
    
    g_ui32TCurCount = TimerValueGet(TIMER0_BASE, TIMER_A);  // 读取TIMER0边沿计数值

    TimerDisable(TIMER0_BASE, TIMER_A);  // 停止TIMER0边沿计数

    g_ui8INTStatus = 1;                                    // 1s计数完成
}

//*******************************************************************************************************
// 
// 函数原型：void PWMInit(uint32_t ui32Freq_Hz)
// 函数功能：产生频率为ui32Freq_Hz的方波(占空比为50%的PWM)，输出引脚为M0PWM4(PG0)
//          该函数是为了方便用户没有信号发生器时产生测试信号而编写的。
// 函数参数：ui32Freq_Hz 需要产生的方波的频率
// 函数返回值：无
//
//*******************************************************************************************************
void PWMInit(uint32_t ui32Freq_Hz)
{
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);             // 不分频
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);     // PWM0使能
    PWMOutputState(PWM0_BASE, PWM_OUT_4_BIT, true); // 使能(允许)PWM0_4的输出
    PWMGenEnable(PWM0_BASE, PWM_GEN_2);             //使能PWM0模块的2号发生器(因为4号PWM是2号发生器产生的)
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, g_ui32SysClock / ui32Freq_Hz); // 根据Freq_Hz设置PWM周期

   
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);    // 使能GPIOG
    GPIOPinConfigure(GPIO_PG0_M0PWM4);              // 配置引脚复用
    GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_0);    // 引脚映射
    
    PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);   //配置PWM发生器
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4,(PWMGenPeriodGet(PWM0_BASE, PWM_GEN_2)/ 2)); //设置占空比为50%
}
