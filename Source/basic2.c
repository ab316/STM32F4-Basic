//////////////////////////////// BASIC 2 //////////////////////////////////////
////////////////////// SYSTEM TICK AND INTERRUPTS /////////////////////////////
// Demonstrates the uses of ARM Cortex SystemTick timer and its interrupt handler
// to produce timing for our applications.
// Blinks and LED using precise timing.


#include "stdafx.h"
#include "basic2.h"


u32 g_iTimer = 0;

static void SetupSystemTick();
static void SetupGPIO();
static void Delay(u32 ms);


// Interrupt handlers need the C caller convention. This is necessary if
// the code is compiled as C++ code.
#ifdef __cplusplus
extern "C" {
#endif

#ifdef BASIC_2
void SysTick_Handler();
#endif

#ifdef __cplusplus
}
#endif


void Basic2()
{
    SetupSystemTick();
    SetupGPIO();
    
    while (1)
    {
	GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
	// With the SysTick timer we can produce accurate timing for our
	// application.
	Delay(1000);
    }
}


static void SetupSystemTick()
{
    // System Tick is an internal timer that is part of the ARM Cortex-M processors
    // it is not specific to ST but to ARM in general. It is a 24-bit counter
    // which auto reloads and produces an interrupt at the end of count. It is
    // intended to provide a standard timer for all Cortex-based microcontrollers.
    // It can be set to run at CPU frequency or its 1/8th.
    
    // This function sets the clock source of the system timer.
    // Possible values are:
    //	SysTick_CLKSource_HCLK
    //	SysTick_CLKSource_HCLK_Div8
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    // Now configure the timer to count to 1/1000th of the system clock.
    // This will produce interrupts at 1 ms intervals.
    SysTick_Config(SystemCoreClock/1000);
}


static void SetupGPIO()
{
    // Learned about this in Basic 1.
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    GPIO_InitTypeDef ioInit;
    ioInit.GPIO_Mode = GPIO_Mode_OUT;
    ioInit.GPIO_OType = GPIO_OType_PP;
    ioInit.GPIO_Pin = GPIO_Pin_12;
    ioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
    ioInit.GPIO_Speed = GPIO_Speed_2MHz;
    
    GPIO_Init(GPIOD, &ioInit);
    
    GPIO_SetBits(GPIOD, GPIO_Pin_12);
}


static void Delay(u32 ms)
{
    g_iTimer = ms;
    while (g_iTimer)
    {
    }
}


// More on interrupts later. For the SysTick interrupt is a system interrupt
// and is enabled by default. The SysTick_Config() functions sets its priority
// to max value (implying lowest priority).
#ifdef BASIC_2
void SysTick_Handler()
{
    if (g_iTimer)
    {
    	g_iTimer--;
    }
}
#endif