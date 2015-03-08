///////////////////////// BASIC TIMERS & INTERRUPTS ///////////////////////////
// Demonstrates the use of the 2 basic timers in the device.

// One timer blinks one LED. The other timers blinks remaining 3 LEDs on the
// board. After few blinks the blinking speed doubles and after same time
// blinking stops as the timers go into one pulse mode (OPM). Then the
// user push button on board can be used to trigger pulses and blink the LEDs.

#include "stdafx.h"
#include "basic4.h"


static void	SetupLEDs();
static void	SetupPushButton();
static void	SetupBasicTimers();
static void	SetupTimer6();
static void	SetupTimer7();
static u16	GetPrescaler(u32 clockSpeed);
static void	SetTimerClock(TIM_TypeDef* TIMx, u32 clockSpeed);

// Interrupt handlers need the C caller convention. This is necessary if
// the code is compiled as C++ code.
#ifdef __cplusplus
extern "C" {
#endif

#ifdef BASIC_4    
void TIM6_DAC_IRQHandler();
void TIM7_IRQHandler();
void EXTI0_IRQHandler();
#endif

#ifdef __cplusplus
}
#endif


void Basic4()
{
    // The ARM Cortex M has a standardized debug interface. The debug interface
    // allows extensions for device manufacturers.
    
    // A problem faced during debugging of microcontrollers is that when one
    // is step-wise executing the code, the peripherals are still running at
    // their actual pace (on the processor is halted). Some times this is
    // useful and sometimes it is troublesome. So STM32F4xx devices have
    // the debugging extension to halt peripherals with the processor.
    // Std peripheral library makes it easier for you to do this.
    
    // Here we stop the TIM4 with the CPU.
    DBGMCU_APB1PeriphConfig(DBGMCU_TIM6_STOP, ENABLE);
    
    SetupLEDs();
    SetupPushButton();
    SetupBasicTimers();
    
    while (1)
    {
    }
}


static void SetupLEDs()
{
    // CLock the peripheral.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    // Initialize the peripheral description.
    GPIO_InitTypeDef init;
    init.GPIO_Mode = GPIO_Mode_OUT;
    init.GPIO_OType = GPIO_OType_PP;
    init.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    init.GPIO_Speed = GPIO_Speed_2MHz;
    
    // Initialize the peripheral.
    GPIO_Init(GPIOD, &init);
}


static void SetupPushButton()
{
    // Clock Port A.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
    // Initialize the GPIO Port A peripheral.
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode = GPIO_Mode_IN;
    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio);
    
    // Enable the EXTI0 interrupt. This interrupt has a higher priority
    // than the timer.
    NVIC_InitTypeDef nvic;
    nvic.NVIC_IRQChannel = EXTI0_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic);
    
    // Initialize the external interrupt line.
    EXTI_InitTypeDef exti;
    exti.EXTI_Line = EXTI_Line0;
    exti.EXTI_LineCmd = ENABLE;
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&exti);
    
    // Clock the SYSCFG peripheral.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    // Configure the external interrupt line 0 source.
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
}


static void SetupBasicTimers()
{
    // NOTE: The device timers are hardware peripherals and not software timers,
    // so they work independently of software execution.
    
    // STM32F407 has 2 Basic Timers (TIM6 & TIM7) on APB1.
    
    //////////////////////////////// OVERVIEW //////////////////////////////////
    // The are mainly used to drive the digital-to-analog converter (DAC) but
    // can also be used as generic timers.
    
    // They are 16-bit up counters with auto-reload and programmable prescaler.
    
    // They count up to the auto-reload value and then reset to 0.
    
    // The reset to 0 generates is due to Counter Overflow.
    
    // This can be made to cause (programmable) an Update Event (UEV).
    
    // The update event (UEV) can also generate an interrupt.
    
    // The purpose of prescaler is to divide the counter's clock.
    
    // All these (Auto-reload value, prescaler, UEV generation) are configurable.
    
    // The timers can also be configured to stop at the next UEV. This is called
    // One Pulse Mode (OPM).
    ////////////////////////////////////////////////////////////////////////////
    
    
    /////////////////////////////// REGISTERS //////////////////////////////////
    //	16-bit Auto-Reload Register (ARR)
    //	16-bit Prescaler (PSC)
    //	16-bit Counter (CNT)
    //	Status Register (SR)
    //	Event Generation Register (EGR)
    //	Control Register 1 & 2 (CR1 & CR2)
    //	DMA/Interrupt Enable Register (DIER)
    
    /////////// ARR & PSC /////////////
    // The ARR stores the value after reaching which the counter resets to 0.
    
    // The PSC stores the prescaler value. Value of 0 means the peripheral clock
    // is used. Value of 1 means the clock is divided by 2 and so on.
    
    // Both the ARR and PSC registers are buffered (or pre-loaded). This means
    // updating their value ONLY takes effect after a UEV. These registers are
    // called "preload" registers and are said to have "shadow" registers
    // which are actually used by the timer. The values from "preload" registers
    // are copied into "shadow" registers on aa UEV.
    // The "preloading" of ARR is programmable.
    
    // NOTE: THE ARR (WHEN PRELOADED), PSC REGISTERS ARE UPDATED
    // ONLY AT A UEV. THEREFORE AFTER INITIALIZING THE COUNTER, A UEV MUST BE
    // GENERATED, OTHERWISE THE ACTUAL CHANGE WILL ONLY TAKE PLACE AFTER THE
    // FIRST COUNTER OVER/UNDER FLOW.
    //////////////////////////////////
    
    
    /////////////// CNT //////////////
    // The CNT stores the value of the counter. It is initially at 0 on reset.
    // The counter counts at the timer's clock speed.
    //////////////////////////////////
    
    
    /////////////// SR ///////////////
    // The SR stores the status of the timer interrupt, Update Interrupt Flag
    // (UIF). A value of 1 means an interrupt is pending. This flag is set
    // by hardware on a UEV and MUST be cleared by software otherwise
    // the interrupt will again take place.
    
    // NOTE: This flag is always set on a UEV. Whether the interrupt actually
    // takes place depends on the URS flag in CR1, UIE flag in DIER, and the
    // enabling of interrupt in the NVIC.
    //////////////////////////////////
    
    
    /////////////// EGR //////////////
    // The EGR holds the Update Generation (UG) flag.
    // This allows reset and UEV generation through software.
    // When this flag is set the counter is set to 0 (reset) and a UEV is generated
    // depending on the values of UDIS flags in CR1. (See CR1).
    
    // NOTE: The generation of UEV depends on UDIS flag in CR1 as described above.
    // If UDIS is set, no UEV will be generated (Hence ARR and PSC will not be
    // updated and no interrupt, if enabled, will be generated) but the reset
    // will take place. Also keep in mind that the shadow registers are only
    // updated on a UEV.
    //////////////////////////////////
    
    
    /////////////// CR1 //////////////
    // CR1 houses several important flags:
    
    //	Counter Enable (CEN). 1 means the counter is running.
    
    //	One Pulse Mode (OPM). 1 means the counter stops (CEN=0) at next UEV.
    
    //	Update Disable (UDIS). 1 means no UEV gets generated.
    
    //	Update Request Source (URS).
    //	1 means only UEV on counter overflow can generate an interrupt.
    //	0 means UEV by setting the UG bit in EGR can also do that.
    
    //	Auto-Reload Preload Enable (ARPE).
    //	1 means the ARR is preloaded. The new value is effective only after a UEV.
    //	0 means immediately.
    //////////////////////////////////
    
    // CR2 is not important. It is not used for these timers.
    
    
    //////////////// DIER ////////////
    // DIER holds 2 flags:
    //	Update Interrupt Enable (UIE). 1 means that the timer generates interrupt
    //	UEV.
    //	Update DMA request Enable (UDE). Not important here (Too advanced).
    
    // We have only talked about generating interrupt on UEV, actually these
    // timers can also trigger a Direct Memory Access (DMA) request which is
    // an advanced feature.
    //////////////////////////////////
    
    SetupTimer6();
    SetupTimer7();
    
    // The following functions change the remaining flags which we have not
    // touched here:
    // 	TIM_UpdateDisableConfig(TIMx, ENABLE/DISABLE)	Changes UDIS
    //	TIM_SetCounter(TIMx, u32 value)			Changes CNT
    //	u32 TIM_GetCounter(TIMx)			Gets CNT
    //	u16 TIM_TIM_GetPrescaler(TIMx)			Gets PSC
    //	TIM_UpdateRequestConfig(TIMx, TIM_UpdateSource)	Changes URS
    //		TIM_UpdateSource can be TIM_UpdateSource_Global (0) or
    //					TIM_UpdateSource_Regular (1).
    
    // Remaining functions in the timer peripheral code are not for basic timers.
    
    // NOTE: Although these timers are 16-bit, their counter arugments in functions
    // are 32-bit because the functions are for all timers in the device. Some
    // timers are 32-bit.
}


static void SetupTimer6()
{
    // Being a peripheral it must be clocked. Both TIM6 & TIM7 are on APB1.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    
    // Set the auto-reload register (ARR). The counter will now count to 999
    // and then reset to 0. Giving a total count of 1000.
    TIM_SetAutoreload(TIM6, 1000-1);
    
    // Enable ARPE. Auto-Reload Preload Enable. Now a change in value of ARR
    // will only take effect on a UEV. Notice however that the value of ARR set
    // up has already taken effect because this function is called later.
    TIM_ARRPreloadConfig(TIM4, ENABLE);
    
    // Set the timer prescaler.
    SetTimerClock(TIM6, 1000);
    
    // Generate a UEV. This will cause the new values of ARR and PSC to take
    // effect. Otherwise the effective value of PSC will only change after the
    // first overflow. Hence, in the first cycle, the counter will count quickly
    // to ARR because the actual prescaler is would still be 0 and the timers
    // would be running at CPU speed (16 MHz) instead of 1 kHz which we have
    // set.
    // This will also raise an interrupt as soon as the interrupt is enabled
    // in the NVIC because the UIF (Update Interrupt Flag) flag is set on a UEV
    // Whether the interrupt actually takes place or not depends on the UIE
    // (Update Interrupt Enable) and URS flags.
    
    // Different timers can generate different types events. Basic Timers, however
    // generate only UEV.
    TIM_GenerateEvent(TIM6, TIM_EventSource_Update);
    
    // Now enable the timer UEV interrupt. This sets the UIE flag in DIER.
    // 2nd argument is the interrupt type.
    // Since Basic timers can only generate interrupt on UEV, its value must
    // be TIM_IT_Update.
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    
    // Enable the timer counter. This actually sets the CEN flag in CR1.
    TIM_Cmd(TIM6, ENABLE);
    
    
    // Now we need to enable the interrupt handler in the NVIC. For information
    // on the name see the handler function below.
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
    
    // Set the interrupt priority. 1 means this is the 2 highest priority
    // user interrupt.
    NVIC_SetPriority(TIM6_DAC_IRQn, 1);
}


static void SetupTimer7()
{
    // Clock the timer.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    
    // Set the ARR.
    TIM_SetAutoreload(TIM7, 999);
    
    // Set the prescaler (PSC).
    SetTimerClock(TIM7, 3000);
    
    // Enable the timer interrupt (Set the UIE flag).
    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
    
    // Enable the counter (Set the CEN flag).
    TIM_Cmd(TIM7, ENABLE);
    
    // Enable the interrupt and set its priority. This interrupt has lowest
    // priority in this demo.
    NVIC_EnableIRQ(TIM7_IRQn);
    NVIC_SetPriority(TIM7_IRQn, 2);
}


// Returns a prescaler for timer required by it to count at the desired speed.
static u16 GetPrescaler(u32 clockSpeed)
{
    u32 f = SystemCoreClock / clockSpeed;
    // Since 1 means divide by 2 and not divide by 1.
    f--;
    return (u16)f;
}


// Sets the timers clock to the desired speed by setting its prescaler
// appropriately.
static void SetTimerClock(TIM_TypeDef* TIMx, u32 clockSpeed)
{
    /// Set the prescaler of the timer.
    // As we know that the prescaler get updated only on UEV.
    // This also resets the counter.
    // In the 3rd arugment Update doesn't sets the UG flag and the change takes
    // place on the next UEV. Immediate sets the UG flag which resets the counter
    // and generates a UEV depending on UDIS.
    TIM_PrescalerConfig(TIMx, GetPrescaler(clockSpeed), TIM_PSCReloadMode_Update);
}


#ifdef BASIC_4
static u32 times = 0;
// This is the TIM6 interrupt handler. Notice the name of the handler. This is
// because this handler is shared by DAC. But since we are not using DAC we
// don't need to worry about checking which peripheral made the interrupt.
void TIM6_DAC_IRQHandler()
{
    // The interrupt pending bit of the timer (UIF in SR) must be cleared
    // othewise the interrupt will occur again as soon as this function returns.
    // This function does the job for you.
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    
    // Toggle the LED.
    GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
    
    times++;
    if (times == 5)
    {
	SetTimerClock(TIM6, 2000);
    }
    else if (times == 10)
    {
	// This sets the timers in one pulse mode. Use Repetitive in place of
	// Single to revert from OPM.
	TIM_SelectOnePulseMode(TIM6, TIM_OPMode_Single);
    }
}


// TIM7 Interrupt Request Handler. TIM7 does not share an interrupt handler
// with any other peripheral.
void TIM7_IRQHandler()
{
    static u32 pins[] = {
	GPIO_Pin_13,
	GPIO_Pin_14,
	GPIO_Pin_15,
    };
    
    static u32 currentPin = 0;
    static u32 oldPin = 2;
    
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    
    GPIO_SetBits(GPIOD, pins[currentPin]);
    GPIO_ResetBits(GPIOD, pins[oldPin]);
    
    oldPin = currentPin;
    currentPin++;
    currentPin %= 3;
    
    if (times == 5)
    {
	SetTimerClock(TIM7, 6000);
    }
    else if (times == 10)
    {
	TIM_SelectOnePulseMode(TIM7, TIM_OPMode_Single);
    }
}


void EXTI0_IRQHandler()
{
    // The EXTI interrupt pending bit must also be cleared.
    EXTI_ClearITPendingBit(EXTI_Line0);
    TIM_Cmd(TIM6, ENABLE);
    TIM_Cmd(TIM7, ENABLE);
}
#endif