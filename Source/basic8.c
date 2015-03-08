//////////////////////// GENERAL PURPOSE TIMERS ///////////////////////////////
//////////////////////////// CAPTURE & COMPARE ////////////////////////////////
///////////////////////////// INPUT CAPTURE ///////////////////////////////////
////////////////////////////// MASTER SLAVE ///////////////////////////////////

// Demonstrates the use of the Input Capture mode of the Capture/Compare unit.
// The timer's counter is captured at the pressing and releasing of the
// push-button on board. This records the time for which the pushbutton was
// held down. The pushbutton's timer works in Master mode. Another timer
// works as Slave of the pushbutton's timer an is enabled by the Master on the
// falling edge of the pushbutton through a trigger. The timer toggles an LED
// so that after releasing the pushbutton, the LED remains on as long as the
// push button was held down.

// Press the pushbutton and release it. The LED will turn on and remain on as
// long as the push button 


#include "stdafx.h"
#include "basic8.h"


static void SetupLEDs();
static void SetupPushButton();
static void SetupPushButtonTimer();
static void SetupLEDTimer();
static u16 GetPrescaler(u32 clock);


#ifdef __cplusplus
extern "C" {
#endif

#ifdef BASIC_8    
void TIM2_IRQHandler();
#endif

#ifdef __cplusplus
}
#endif



void Basic8()
{
    SetupLEDs();
    SetupPushButton();
    SetupPushButtonTimer();
    SetupLEDTimer();
    
    while (1)
    {
    }
}


static void SetupLEDs()
{
    // Clock the peripheral.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    // Initialize the peripheral.
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOD, &gpio);
    
   // GPIO_SetBits(GPIOD, GPIO_Pin_12);
}


static void SetupPushButton()
{
    // Clock it.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
    // Initialize the push button pin as timer 2's alternate function.
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOA, &gpio);
    
    // Configure the pin's alternate function.
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2);
}


static void SetupPushButtonTimer()
{
    // Clock it.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // Initialize the timer's time base. Up count to maximum value @ 1 kHz.
    TIM_TimeBaseInitTypeDef base;
    base.TIM_ClockDivision = TIM_CKD_DIV1;
    base.TIM_CounterMode = TIM_CounterMode_Up;
    // Counter up to maximum value.
    base.TIM_Period = (u32)-1;
    base.TIM_Prescaler = GetPrescaler(1000);
    
    // Do it.
    TIM_TimeBaseInit(TIM2, &base);
    
    
    ////////////////// INPUT CAPTURE ///////////////////
    
    // Input Capture feature allows to use the device's pins as signals to
    // capture the counter's value and reset or halt the timer.
    // In PWM mode it also allows to measure the period and duty cycle of an
    // applied PWM signal.
    
    // The purpose of capture is to enable measuring relative to a time reference.
    
    // All the channels of a timer are seperately configurable as input capture
    // or output compare.
  
    // Multiple channels can be tied to a single input. Detected edge (rising,
    // falling or both) is also programmable.
    
    // As in Output Compare mode, the Capture/Compare mode selection is done
    // by the CCxS fields in the CCMR1-2 registers. CCMR1-2 register have
    // different meaning of fields depending on the mode (capture/compare)
    // selected.
    
    // In input capture mode each channel has a prescaler field ICxPSC that
    // selects after how many edges the capture is made.
    
    // Each channel is input capture mode also has a filter field ICxF which
    // configures the sampling of the timer input. It defines the sampling
    // frequency the number of samples required to validate a transition.
    // E.g. @ at sampling frequency f with N samples, a transition from 0 to
    // 1 is considered only if the signal switches from 0 to 1 and stays 1
    // for atleast N successive samples @ frequency f.
    
    // Input Capture selection, however, has 3 modes (atleast on TIM2-5):
    //	Each channel can be configured as input capture from 1 of 2 timer inputs.
    //	It can also be configured to take input from an internal trigger (TRC)
    //	if one is selected by the TS field in SMCR.
    
    // Each channel, needs to be enabled in CCER register.
    // The polarity bits (CCxNP/CCxP) in CCER refer to the
    // detected edge (rising/falling/both) in Input Capture mode.
    
    // Each channel has its own Capture/Compare Interrupt/DMA enable flag in DIER.
    // Capture/Compare produces a Capture-Compare event which can be set to
    // produce an interrupt.
    
    // The Capture/Compare registers CCRx hold the value of counter captured.
    
    // The status register has Capture/Compare Overcapture Flags CCxOF for each
    // channel which is set when a capture is made while the CCxIF (Capture/
    // Compare Interrupt Flag) was already set (CCxIF is set when a capture
    // is made, it results in a capture interrupt if enabled, the interrupt
    // handler is supposed reset this flag indicating that the capture was
    // acknowledged.).
    ////////////////////////////////////////////////////
    
    // We are going to initialize 2 channels. Both will capture input from
    // the same input TI1. One will detect rising edge, other falling edge.
    // We must set the falling edge on Channel 1 because we want the LED
    // timer to start on the falling edge of the push button and the Master
    // mode timer can send the trigger only on Capture/Compare channel 1 event.
    
    // Setup the Input Capture channel 1.
    TIM_ICInitTypeDef ic;
    // Channel 1 is going to be used.
    ic.TIM_Channel = TIM_Channel_1;
    // We capture at rising edge. Other options: (Rising, BothEdge).
    // When push button is released.
    ic.TIM_ICPolarity = TIM_ICPolarity_Falling;
    // Pushbutton are notorious for bouncy. So we use the highest filtering
    // level for them. @ 1/32th the normal sampling frequency and 8 samples.
    ic.TIM_ICFilter = 0xF;
    // We want a capture at every transition. Other options: (2, 4, 8).
    ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    // As each channel can be mapped to 2 input pins this field selects the
    // input to map to. Channel 1 & 2 can map to TI1 (Timer Input 1) & TI2.
    // Direct means CH1 maps to TI1 indirect mean CH1 maps to TI2. The case
    // is opposite for CH2. Each channel can also be mapped to an internal
    // trigger.
    // Other options: (IndirectTI, TRC)
    ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
    
    // Feed the data. Initialize the capture channel 1. This calls sets the
    // register appropriately.
    TIM_ICInit(TIM2, &ic);
    
    
    // Now to initialize channel 2.
    ic.TIM_Channel = TIM_Channel_2;
    // When push-button is pressed.
    ic.TIM_ICPolarity = TIM_ICPolarity_Rising;
    // Indirect means CH2 maps to TI1.
    ic.TIM_ICSelection = TIM_ICSelection_IndirectTI;
    TIM_ICInit(TIM2, &ic);
    
    
    ///////////////////// MASTER MODE ///////////////////////
    // The timers are internally linked for synchronization. One timer can
    // control the other. The timer that controls is put in Master Mode and
    // the one to be controlled is set in Slave Mode.
    
    // The Master Mode timer broadcasts a trigger output TRGO. The Slave Mode
    // timers linked to the Master Mode timer receive this event and can
    // control their action depending on it. This receiving event is known
    // as Trigger Event and can raise an Interrupt/DMA request of the Slave
    // timer if enabled.
    
    // By default all timers are in master mode.
    // We can select when the TRGO is set by the Master in the Master Mode
    // Selection (MMS) field in Control Register 2 (CR2). Following options are
    // available:
    //	1- On reset, UG bit from EGR is used as TRGO.
    //	2- On enable. Counter Enable bit is used.
    //	3- On Update Event (UEV).
    //	4- On the Capture/Compare Channel 1 event. CC1IF flag.
    //	5- Output of one of the Output Compare channels.
    
    // To reduce the delay between TRGI (see slave mode below) and TRGO
    // a flag is available in the Slave Mode Controller Register (SMCR) called
    // Master/Slave mode. Setting this flag for the Master Mode timer allows
    // for perfect synchronization between the Master & Slave timers. It is
    // useful when they are several slave timers.
    // In std peripheral library it is set as follows:
    // TIM_SelectMasterSlaveMode(TIMx, TIM_MasterSlaveMode_Enable/Disable);
    /////////////////////////////////////////////////////////
    
    
    // Here we select when the Master generated a TRGO. We set it to generate
    // it on the CC CH1 event. That is, on the falling edge (pushbutton
    // released) of the timer.
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_OC1);
    
    // Enable the capture interrupts for the 2 channels.
    TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
    TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
    
    // Enable the timer.
    TIM_Cmd(TIM2, ENABLE);
    
    // Enable the interrupt.
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 0);
}


static void SetupLEDTimer()
{
    // Clock it.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    // Setup the time base.
    TIM_TimeBaseInitTypeDef base;
    base.TIM_ClockDivision = TIM_CKD_DIV1;
    base.TIM_CounterMode = TIM_CounterMode_Up;
    // Doesn't need to be set here because it is modified by the push-button
    // timer interrupt.
    base.TIM_Period = 1000;
    // 1 ms clock.
    base.TIM_Prescaler = GetPrescaler(1000);
    
    // Initialize the time base.
    TIM_TimeBaseInit(TIM4, &base);
    // Don't the the UEV interrupt to be generated.
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    
    
    ////////////////////// SLAVE MODE ////////////////////////
    // The Slave Mode timers can peform different actions depending on the
    // trigger input (TRGI). The Trigger Input TRGI of the Slave timer
    // is the Trigger Output TRGO of its Master timer.
    
    // Slave mode configuration of timers' is done in the Slave Mode
    // Configuration Register (SMCR).
    // 2 options are necessary to configure a timer in slave mode:
    
    //	1- Trigger selection (TS): Where the trigger comes from? Previously we
    //		used the external input 1 TI1 as trigger. Here we want an
    //		internal trigger (another timer) as trigger.
    
    //	2- Slave Mode Selection (SMS). What to do on the trigger? Available options
    // 		are:
    //			Reset: Rising edge of TRGI resets the timer.
    //			Gated: Clock enable when TRGI high.
    //			Trigger: Counter enabled on rising edge of TRGI.
    //			External Clock Mode 1: Rising edges clocks the counter
    //				(we used this mode previously to clock the timer)
    /////////////////////////////////////////////////////////
    
    // We want the LED timer to start counting on the rising edge of TRGI.
    // So when the pushbutton is released (falling edge of signal). The Master
    // timer sets TRGO to high. The Slave timer receives it as TRGI and
    // enables itself. This sets the SMS field.
    TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Trigger);
    
    // We need to select which timer is the Master timer. This sets the TS
    // field. These links must be looked up in the reference manual. However
    // there are 4 internal trigger selections. Internal Trigger 1 maps to
    // TIM2 for TIM4.
    TIM_SelectInputTrigger(TIM4, TIM_TS_ITR1);
    
    // We want the Autoreload register to reload immediately because the timer
    // is enabled as soon as the Master sets the trigger. The counter will
    // be already running when we update its ARR and as it is supposed to run
    // only once we can wait to set it for the next time.
    TIM_ARRPreloadConfig(TIM4, DISABLE);
    
    // Set the timer in one pulse mode. This way the timer runs once, resets
    // and stop after generating a UEV (and turning off the LED).
    TIM_SelectOnePulseMode(TIM4, TIM_OPMode_Single);
    
    // Don't enable the timer. Its a slave timer, Duh!
    //TIM_Cmd(TIM4, ENABLE);
    
    // Enable the UEV interrupt.
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    // Trigger interrupt is generated on the rising edge of TRGI.
    //TIM_ITConfig(TIM4, TIM_IT_Trigger, ENABLE);
    
    // Enable the timer interrupt.
    NVIC_EnableIRQ(TIM4_IRQn);
    NVIC_SetPriority(TIM4_IRQn, 1);
}


// Returns a prescaler to run the clock @ the required frequency it Hz.
static u16 GetPrescaler(u32 clock)
{
    u32 t = (SystemCoreClock / clock) - 1;
    return t;
}


// Counter to store the time differnence between rising and falling edges of
// the push-button.
static u32 counter = 0;

// Push button timer interrupt request handler. Master mode timer.
#ifdef BASIC_8
void TIM2_IRQHandler()
{
    // Multiple timer interrupt are enabled so we must check which interrupt
    // ocurred.
    
    // Butten released. Falling edge.
    if (TIM_GetITStatus(TIM2, TIM_IT_CC1) == SET)
    {
	// Clear the pending bit. Must do it.
	TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
	
	// Find the time difference between rising and falling edge.
	counter = TIM_GetCapture1(TIM2) - counter;
	// Update the timer 4's auto-reload value. Now it will count to the
	// time difference calculated above. Since both timers are running
	// at the same clock, this means that the timer will count as long as
	// the push button was down and then reset.
	TIM_SetAutoreload(TIM4, counter);
	
	// Turn off the indicator LED.
	GPIO_ResetBits(GPIOD, GPIO_Pin_12);
	
	// Turn on the timed LED.
	GPIO_SetBits(GPIOD, GPIO_Pin_14);
    }
    // Pushbutton pressed. Rising edge.
    else if (TIM_GetITStatus(TIM2, TIM_IT_CC2) == SET)
    {
	// Clear the interrupt pending bit.
	TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
	
	// Get the captured value of the counter.
	counter = TIM_GetCapture2(TIM2);
	
	// Turn on the indicator LED.
	GPIO_SetBits(GPIOD, GPIO_Pin_12);
    }
}


// Slave mode timer interrupt request handler. Time LED timer.
void TIM4_IRQHandler()
{
    // Update event?
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
    {
	// Clear the pending bit.
    	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	
	// Toggle the timed LED. It is turned on before the code gets here so
	// it gets turned off.
    	GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
    }
//    else if (TIM_GetITStatus(TIM4, TIM_IT_Trigger) == SET)
//    {
//	TIM_ClearITPendingBit(TIM4, TIM_IT_Trigger);
//    	GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
//    }
}
#endif