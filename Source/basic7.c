//////////////////////// GENERAL PURPOSE TIMERS ///////////////////////////////
/////////////////////////// EXTERNAL TRIGGER CLOCK ////////////////////////////
// Demonstrates the setting of external input as the timer's clock.
// The LED turns on on every 2nd hit of the push-button. It may sometime
// not seems so because of the bouncing of the push-button. They are notorious
// for this behaviour.

#include "stdafx.h"
#include "basic7.h"


static void SetupLEDs();
static void SetupPushButton();
static void SetupTimer2();


#ifdef __cplusplus
extern "C" {
#endif
 
#ifdef BASIC_7
void TIM2_IRQHandler();
#endif

#ifdef __cplusplus
}
#endif


void Basic7()
{
    SetupLEDs();
    SetupPushButton();
    SetupTimer2();
    
    while (1)
    {
    }
}


static void SetupLEDs()
{
    // Clock enable.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    // Initialize the LEDs.
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    
    GPIO_Init(GPIOD, &gpio);
}


static void SetupPushButton()
{
    // Clock.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
    // Initialize the push button with alternate function of being a timer 2 input.
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_DOWN;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &gpio);
    
    // PA0 (Pin 0 of port A) has alternate function mappings of TIM2 CH1 and
    // TIM2 External Trigger Input (ETR).
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2);
}


static void SetupTimer2()
{
    // Have to choose TIM2 because the push button is on PA0 and PA0 has
    // alternate function mapping of either TIM2, 5 or 8. TIM8 is an
    // Advanced Timer. TIM2 & 5 are same in features. Just picked one.
    
    // Clock it.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    TIM_TimeBaseInitTypeDef base;
    // Initialize the struct with default arguments.
    base.TIM_ClockDivision = TIM_CKD_DIV1;
    base.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
    base.TIM_Period = 2;
    base.TIM_Prescaler = 0;
    
    // Feed the time base into TIM2. This function also generates a UEV by
    // setting the UG flag.
    TIM_TimeBaseInit(TIM2, &base);
    // Stop the interrupt from being generated from this first UE.
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    
    
    //////////////////////// EXTERNAL CLOCK SELECTION //////////////////////////
    // There are 2 external clock selection modes:
    //	1- External clock connected to T1 or T2 pins
    //	2- External clock connected to ETR pin (TIM2-4 only)
    
    // In this device atleast ETR and T1 pins are same. So it doesnt matter which
    // mode you use. Just stick to mode 1 because mode 2 is not available on
    // all timers.
    
    ///// MASTER AND SLAVE MODE /////
    // When the timer is controlled by the internal (prescaled) clock, the timer
    // is said to be in MASTER mode.
    
    // When the timer is controlled by another signal (external or internal,
    // which may be another timer), the timer is said to be in SLAVE mode.
    
    // Controlling here means the controlling the clock going to the timer's
    // counter.
    
    // The timer's mode is selected by the Slave Mode Selection (SMS) field
    // in the Slave Mode Control Register (SMCR). It has several options. Our
    // option of interest here is External Clock Mode 1 which clocks the
    // counter on the rising edges of the selected Trigger TRGI.
    
    // SMCR has another register, Trigger Selection (TS), to select the signal
    // or trigger (TRGI) to clock the timer. This clock of timer with another
    // signal is also referred to as synchronizing the timer or the counter.
    
    // Trigger Selection (TS) values
    //	1- Internal Trigger  0-3. These triggers are internally connected and are
    //	  defined by the vendor. These are connected to other timers and are
    //	  given in the reference manual.
    //
    //	2- TI1 Edge Detector. This outputs a pulse on every transition of the 
    //	   TI1 pin.
    //
    //	3- Filtered Timer Input 1-2 The T1 or T2 inputs after filtered are used.
    //	   filtering includes filtering the required edge and synchronizing
    //	   the signal with the bus.
    //
    //	4- External Trigger Input. ETR pin. Leave it. Its the same as TI1 pin.
    ////////////////////////////////
    
    
    // As the external signal coming from the pin needs to be input to drive
    // the counter. It must be connected to a channel or external trigger
    // ETR (which is on the same pin as channel 1 for this device atleast).
    
    
    /////// INPUT CAPTURE ///////
    // To input the pin signal into a channel we need to place that channel in
    // input capture mode and set the appropriate pin in its alternate function
    // mode.
    
    // The Capture/Compare functionality is enabled by the CCxE bit in the
    // Capture Compare Enable (CCER) register. x is the channel no. The channel
    // polarity (active high/low for Output Compare and rising/falling/both
    // edges for Input Capture) are controlled by the CCxNP and CCXP bits in the
    // same register.
    
    // The selection between Capture/Compare modes is done by the CCxS bit field
    // in the Capture Compare Mode Registers (CCMR1-2).
    /////////////////////////////
    
    
    // To select the external clock on a channel 1 pin we need to:
    //	Setup the appropriate device pin. A channel 1 pin.
    //	Set the appropriate filter on the input channel.
    //	Set the trigger selection (TS) to select the TI1 Edge Detector.
    //	Set the Slave Mode to External Clock Mode 1.
    
    
    // Standard peripheral library provides the following function to do it all
    // for you.
    TIM_TIxExternalClockConfig(TIM2, TIM_TIxExternalCLK1Source_TI1, TIM_ICPolarity_Rising, 0);
    
    // Enable the update event timer interrupt.
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    
    // Enable the timer.
    TIM_Cmd(TIM2, ENABLE);
    
    // Enable the interrupt in the NVIC.
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 0);
}


#ifdef BASIC_7
void TIM2_IRQHandler()
{
    // Must do this.
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    // Just toggle the LED.
    GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
}
#endif