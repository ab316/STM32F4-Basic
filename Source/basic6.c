//////////////////////// GENERAL PURPOSE TIMERS ///////////////////////////////
////////////////////////////// PWM GENERATION /////////////////////////////////
// Demonstrates the uses of PWM mode to pulsate an LED.

#include "stdafx.h"
#include "basic6.h"


static void SetupGPIO();
static void SetupTimer();


// Interrupt handlers need the C caller convention. This is necessary if
// the code is compiled as C++ code.
#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef BASIC_6
void TIM4_IRQHandler();
#endif

#ifdef __cplusplus
}
#endif


void Basic6()
{
    // The ARM Cortex M has a standardized debug interface. The debug interface
    // allows extensions for device manufacturers.
    
    // A problem faced during debugging of microcontrollers is that when one
    // is step-wise executing the code, the peripherals are still running at
    // their actual pace (on the processor is halted). Some times this is
    // useful and sometimes it is troublesome. So STM32F4xx devices have
    // the debugging extension to halt peripherals with the peripherals.
    // Std peripheral library makes it easier for you to do this.
    
    // Here we stop the TIM4 with the CPU.
    DBGMCU_APB1PeriphConfig(DBGMCU_TIM4_STOP, ENABLE);
    
    SetupGPIO();
    SetupTimer();
    
    while (1)
    {
    }
}


static void SetupGPIO()
{
    // Clock the port.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    // Initialize the port.
    // Note the mode of this pin is AF (Alternate Function) because this
    // pin will receive the output of the timer. Each pin has specific
    // alternate functions mapped to it by the manufacturer, they are found
    // in the device's data sheet.
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Pin = GPIO_Pin_12;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOD, &gpio);
    
    // A pin can have multiple alternate functions so we must select which
    // alternate function gets activated. Here P12 of GPIOD has an alternate
    // function of being connected to the Channel 1 of Timer 4. In this demo
    // this pin will receive the Output of channel 1 of timer 4.
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
    
    // P13 needs to work as a normal output pin so it must be initialized
    // seperately.
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Pin = GPIO_Pin_13;
    // This does not affect the already configured P12 because the only the
    // bits in the registers corresponding to the pin(s) given in the struct
    // are modified.
    GPIO_Init(GPIOD, &gpio);
}


static void SetupTimer()
{
    // Clock the peripheral.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    // Initialize the time-base as before.
    TIM_TimeBaseInitTypeDef base;
    
    // Still don't know what is this for!
    base.TIM_ClockDivision = TIM_CKD_DIV1;
    
    // Count up. Other options are (Down, CenterAligned1-3)
    base.TIM_CounterMode = TIM_CounterMode_Up;
    
    // 0...999. Gives a count of 1000.
    base.TIM_Period = 1000 - 1;
    
    // Time clock becomes 100 kHz.
    base.TIM_Prescaler = (SystemCoreClock / 100000) - 1;
    
    // Feed it.
    TIM_TimeBaseInit(TIM4, &base);
    
    
    // Std peripheral library saves us a lot of trouble by allow to work
    // with timer channel's in an organized way without going into register
    // level details. But one should explore the library files to discover
    // how it's done.
    
    // Pulse Width Modulation (PWM) allows to generate a signal with a
    // frequency determined by ARR and the duty-cycle determined the corresponding
    // Capture and Compare Channel register (CCRx) for the channel x.
    // PWM mode is a sub-mode of Output Compare mode.
    // As the name suggests, Output Compare Mode needs something to compare with,
    // The Capture and Compare Register (CCRx) (1 for each channel) stores the
    // value to compare against. Like ARR and PSC, it is buffered (or preloaded).
    
    // PWM has 2 modes:
    // 	Mode 1:
    //		In up-count, channel is ACTIVE as long as counter register (CNT) < CCRx
    //		In down-count, channel is INACTIVE when CNT > CCRx
    //	Mode 2:
    //		In up-count, channel is INACTIVE when CNT < CCRx
    //		In down-count, channel is ACTIVE when CNT > CCRx
    
    // Due to fast switching of output, we can not see the LED blinking, instead
    // we see it dimmed.
    
    TIM_OCInitTypeDef oc;
    // This mode enables PWM mode 2.
    // Other modes are (PWM1, Timing, Active, Inactive and Toggle).
    
    // Timing mode doesn't change the output.
    // Active mode sets Output to Active when CNT matches CCRx
    // Inactive does the opposite of Active.
    // Toggle toggles the Output when CNT matches CCRx
    oc.TIM_OCMode = TIM_OCMode_PWM2;
    
    // The output of channel can also be forced to active/inactive by the
    // following call. Where x is the the channel# and y is the timer#.
    //TIM_ForcedOCxConfig(TIMy, TIM_ForcedAction_Active/InActive);
    
    // The channel mode can be entirely configure by the following function.
    // The x is function call is the name and not a variable (there is only
    // one function). The 3 arguments can be one of the modes shown in
    // TIM_OCMode above or one of TIM_ForcedAction above.
    //TIM_SelectOCxM(TIMx, TIM_Channel, TIM_OCMode);
    
    
    // We surely want output. Other option is Enable. Output signal is only
    // sent to the pin when this flag is set. This field actually sets the
    // corresponding output enable bit in CCER.
    oc.TIM_OutputState = TIM_OutputState_Enable;
    
    // Active high output polarity. Other option is Low.
    oc.TIM_OCPolarity = TIM_OCPolarity_High;
    
    // The value to be loaded in CCRx. x is going to be 1 in this case.
    oc.TIM_Pulse = 1000;
    
    // Only for TIM1 and TIM8 (Advanced Timers).
    //oc.TIM_OCNPolarity;
    //oc.TIM_OCNIdleState;
    //oc.TIM_OCIdleState;
    
    // Initialize the channel 1 of TIM4 with this struct.
    TIM_OC1Init(TIM4, &oc);
    
    // The function preloads the CCR1 (so that like PSC and ARR (if preloaded)
    // it only gets updates on an Update Event (UEV).
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    
    // Preload the ARR.
    TIM_ARRPreloadConfig(TIM4, ENABLE);
    
    // Update the registers.
    TIM_GenerateEvent(TIM4, TIM_EventSource_Update);
   
    // Now enable the timer.
    TIM_Cmd(TIM4, ENABLE);
    
    // Update the TIM4 UEV interrupt (which occurs on over/under flow).
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    
    NVIC_EnableIRQ(TIM4_IRQn);
    NVIC_SetPriority(TIM4_IRQn, 0);
}


// TIM4 Interrupt Request Handler.
#ifdef BASIC_6
void TIM4_IRQHandler()
{
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    
    static int dir = -1;
    int newValue = TIM_GetCapture1(TIM4) + (dir * 5);
    //int newValue = TIM4->CCR1 + (dir * 10);
    
    if (newValue >= 1000)
    {
	newValue = 1000;
	dir = -1;
	//GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
    }
    else if (newValue <= 0)
    {
	newValue = 0;
	dir = 1;
	//GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
    }
    
    TIM_SetCompare1(TIM4, (u32)newValue);
    //TIM4->CCR1 = (u32)newValue;
}
#endif