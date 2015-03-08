//////////////////////// GENERAL PURPOSE TIMERS ///////////////////////////////
////////////////////////////// BASIC SETUP ////////////////////////////////////
// Demonstrates the use of general purpose timers. Shows only the basic setup
// and, therefore, just blinks an LED.

#include "stdafx.h"
#include "basic5.h"


static void SetupGPIO();
static void SetupTimer();


// Interrupt handlers need the C caller convention. This is necessary if
// the code is compiled as C++ code.
#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef BASIC_5
void TIM4_IRQHandler();
#endif

#ifdef __cplusplus
}
#endif


void Basic5()
{
    SetupGPIO();
    SetupTimer();
    
    while (1)
    {
    }
}


static void SetupGPIO()
{
    // Clock port D.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    // Initialize the GPIO.
    GPIO_InitTypeDef gpio;
    
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOD, &gpio);
    
    GPIO_SetBits(GPIOD, GPIO_Pin_12);
}


static void SetupTimer()
{
    // STM32F407VG has 10 General Purpose Timers. Some are 16-bit and some are
    // 32-bit. They also vary in other features.
    
    // They are used for many purposes, including generating output waveform
    // (compare and pulse width modulation) and measuring pulse lengths of
    // input (input capture).
    
    ///////////////////////////////// OVERVIEW /////////////////////////////////
    // 	TIM2 & TIM5 are 32-bit while TIM3 & TIM4 are 16-bit. These 4 times
    // 	have the same features apart from the bits. Timers 9-14 are also
    //	general purpose timers have the same functionality but different
    //  feature count: They all are 16-bit. They only count up. They are on
    //	different busses (so different max speed) and have different no. of
    //	channels.
    
    // TIM2-5 are the focus here as they have max functionality of general
    // purpose timers.
    
    //	Up, Down, Centered (Up-Down) count.
    
    //	Auto-reload.
    
    //	16-bit Programable prescaler.
    
    //	Synchronization circuit to control timers through external signal and other timers.
    
    //	4 independent channels for
    //		Input capture
    //		Output compare
    //		Pulse Width Modulated (PWM) output
    //		One Pulse Mode output (OPM)
    //	These timers utilize the device's pins for the I/O channels.
    
    //	Interrupt/DMA can be generated on:
    //		Update Event (UEV) as in the Basic Timers
    //		Trigger Event (counter start, stop, inititalization or internal/external trigger)
    //		Input capture
    //		Output compare
    
    // These timers are superset of the Basic Timers and, therefore, include
    // additional functionality. So only the additional functionality will
    // be described here.
    ///////////////////////////////////////////////////////////////////////////
    
    
    ////////////////////////// Counting Modes /////////////////////////////////
    // 1- Up-Count, the counter counts from 0 to ARR and then resets.
    
    // 2- Down-Count, the counter counts from ARR to 0 and then resets.
    
    // 3- Center-Aligned, the counter counts from 0 to ARR-1, generate counter
    //		overflow event. Then counts from ARR to 1 and generates a
    // 		counter underflow event. Then restarts counting from 0.
    //		This mode actually has 3 sub-modes selected by the Centre-aligned
    //		Mode Selection (CMS) field in CR1. The affects the Output Compare
    //		Mode (See below).
    
    
    //	Up and Down count modes are known as Edge-aligned modes.
    
    //	The Centre-aligned Mode Selection (CMS) field in CR1 selects between
    //	Centre and Edged Aligned modes.
    
    //	The direction is specified by the DIR bit the Control Register (CR1).
    //	In center-aligned mode, this bit is controlled by hardware.
    ///////////////////////////////////////////////////////////////////////////
    
    
    //////////////////////////////// PINS /////////////////////////////////////
    // Unlike Basic Timers, these timers have functions that map to to certain
    // 	pins on the device.
    // These pins, when put in the Alternate Function mode of these timers,
    // allow I/O with the timers.
    
    // Every timer has one or more Channels (CH) for I/O which are configurable.
    // TIM2-5 discussed here have 4 channels.
    // Each channel can be mapped to a certain pin as described in the datasheet.
    // These pins are referred to as TIx where x is the channel number.
    
    // In these timers, The channel 1 input (if in an Input mode) can be changed
    // to be the XOR of input of channels 1, 2 and 3.
    // Selection is based on the TI1 Selection (TI1S) flag in Control Register 2 (CR2)
    // Setting this bit enables the XOR functionality.
    
    // These timers also have an External Trigger function which maps as ETR pin.
    ///////////////////////////////////////////////////////////////////////////
    
    
    ////////////////////////// Clock Selection ////////////////////////////////
    // The counter clock can be selected from different sources.
    // Selection is done by the Slave Mode Selection (SMS) field in
    // Slave Mode Control Register (SMCR).
    
    //	Internal Clock:	The default clock
    
    //	External Clock Mode 1: Edge-trigged applied on TI1 or TI2. Prescaler
    // 		is not applied.
    
    //	External Clock Mode 2: External Trigger input (ETR). NOT FOR TIM5.
    
    //	Internal Trigger Input (ITRx): Using one timer as prescaler for another.
    //		The controlling timer acts in Master mode and the controlled
    //		timer is in Slave mode.
    //		SMCR controls the Master/Slave mode.
    
    // Only internal clock is used here.
    ////////////////////////////////////////////////////////////////////////////
    
    
    ///////////////////// Capture Compare Unit ////////////////////////////////
    //	These timers have this unit for input capture and output compare feature.
   
    //	Each timer has 2 Capture Compare Mode Register (CCMRx) to configure
    //	the mode for each channel:
    //		Each register manages 2 of the channels.
    //		Each register has CCxS field to select the Capture/Compare mode for channel x.
    //		Each register has OCxPE field to preload the CCRx register in
    //		 Output Compare mde.
    
    //	Each channel has a Capture/Compare Register (CCRx) whose purpose depends on the mode.
    //	These registers are (pre-loaded) like ARR and PSC.
    
    //	Each timer has a Capture/Compare Enable Register (CCER) which has for each channel:
    //		A Capture/Compare output enable bit (CCxE)
    //		An Output Polarity bit (0 is active high) for Output mode. (CCxP)
    //		An other Polarity bit (CCxNP) for input mode (trigger/capture)
    //			when the channel is in input mode, CCxP and CCxNP combine
    //			to configure the input edge-detection.
    
    //	For each channel, the status register (SR) has a capture compare
    // 	interrupt flag (CCxIF).
    //	Similarly for each channel, the Event Generation Register (EGR) has 
    //  has a capture-compare event generation flag (CCxG).
    //	These registers and their functions were described in Basic Timers.
    ///////////////////////////////////////////////////////////////////////////
    
    // The capture/compare unit presents a lot of functionality. Here we will
    // only use the timers just like the basic timers.
    
    // Features of General Purpose Timers will be shown in individual demos
    // starting from this demo.
    
    
    // Clock the timer. Its on APB1.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    // The basic configuration of these timers are defined in this struct
    // in the std peripheral library.
    TIM_TimeBaseInitTypeDef timeBase;
     
    // Can be (TIM_CKD_DIV1, TIM_CKD_DIV2, TIM_CKD_DIV4)
    // Don't know right now what it is used for. Doesn't seem to change the
    // counter clock.
    timeBase.TIM_ClockDivision = TIM_CKD_DIV1;
    
    // Can be (TIM_CounterMode_Up, Down, CenterAligned1, CenterAligned2, CenterAligned3)
    // 3 versions of CentreAligned differ only in their working with Output Compare.
    timeBase.TIM_CounterMode = TIM_CounterMode_Up;
   
    // The Auto-Reload Register (ARR) value. Total count of 1000.
    timeBase.TIM_Period = 1000 - 1;
    
    // Gives a clock of 1 kHz.
    timeBase.TIM_Prescaler = (SystemCoreClock/1000) - 1;
    
    // This is only used by TIM1 & TIM8 which are Advanced Timers.
    //timeBase.TIM_RepetitionCounter;
    
    // Feed the timer's registers with the time base data.
    TIM_TimeBaseInit(TIM4, &timeBase);
    
    // Update the shadow registers.
    TIM_GenerateEvent(TIM4, TIM_EventSource_Update);
    
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
    
    NVIC_EnableIRQ(TIM4_IRQn);
    NVIC_SetPriority(TIM4_IRQn, 0);
}


#ifdef BASIC_5
void TIM4_IRQHandler()
{
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
}
#endif