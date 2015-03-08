/*
// This project is a bare metal CMSIS based STM32F407VG project.
// It does not work on top of the ST's HAL Layer. It only employs CMSIS and
// ST's standard peripheral library for the STM32F4xx microcontrollers.
// It is started to understand the operation of the ARM Cortex-M STM32F4 family
// in detail without going into too much abstraction.
// When making a new project, use this project as a template. The important
// thing is the project options which must be configured appropriately.
// Following project options must be set:
//	Device type	(STM32F407VG in this case).
//	Addition include directories.
//	Preprocessor directives.
//	Debugger type (ST-LINK with SWD port in this case).
//	If Simulator debugger is not used, then Download to Flash must be selected. (Atleast here).
//	The CPU Clock in the selected debugger should be the same as set
//	by the sofware.


//////////////////////////// DIRECTORY STRUCTURE //////////////////////////////
// Drivers (DO NOT TOUCH DIRECTORY)
//	CMSIS ->
//		Include	(Contains ARM Cortex and microcontroller specific definitions)
//		Source ->
//			system_stm32f4xx.c (Contains SystemInit() functions which
//			resets the Clock configuration, Also contains a functions to
//			update SystemCoreClock variable). Details unneccessary.
//
//	Peripherals	(DO NOT TOUCH DIRECTORY) (Contains header and source files
//			for the ST's Standard Peripheral Library).
//
//	System ->
//		startup_stm32f4xx.s	(Contains the vector table required by
//		the ARM core at the start of memory, The vector table holds information
//		about the initial stack pointer and all the interrupt handlers.
//		Program execution starts at the address pointed by the ResetHandler.
//		
//		stm32f4xx_conf.h	(This file is used by the standard peripheral
//		library. One should uncomment all the peripheral drivers includes
//		used in this file. It has defines the value of the HSE which
//		here is set to 8 MHz used in the F4 Discovery board.

//	User ->		(This folder is where the user files should be placed.)
//			(Feel free to customize everything in this folder).
//		stdafx.h	(Contains the standard includes).
//		main.h		(Needs no introduction)
//		main.cpp	(This file)
//		interrupt.h	(Prototypes of interrupt handlers used).
//		interrupt.c	(Interrupt routines).
//		basicx.h/c	(Different basic programs. Follow them in
//				ascending order).
///////////////////////////////////////////////////////////////////////////////
*/

#include "stdafx.h"
#include "main.h"
#include "basic1.h"
#include "basic2.h"
#include "basic3.h"
#include "basic4.h"
#include "basic5.h"
#include "basic6.h"
#include "basic7.h"
#include "basic8.h"

int main()
{
    /*
    //////////////////////////////// CMSIS ////////////////////////////////////
    // Cortex Microcontroller Software Interface Standard (CMSIS) provides a 
    // Hardware Abstraction Layer (HAL) for ARM Cortex-M based microcontrollers.
    // It provides common definition for core peripherals and components.
    // Microcontroller vendors define another layer on top of it for their
    // device specific peripherals.
    ///////////////////////////////////////////////////////////////////////////
    
    
    /////////////////////////////// CLOCK /////////////////////////////////////
    // The STM32 controller is clocked by either Internal or External Oscillators
    // or from an internal PLL(Phase Locked Loop) which itself can be driven from
    // Internal or External Oscillators.
    // There are 2 types of external oscillators: High Speed (HSE) and Low Speed (LSE).
    // HSE is used to clock the main core and the peripherals. LSE is mainly used to clock
    // the Real Time Clock (RTC).
    // On Reset he SMT32 controller is clocked by the High Speed Internal Oscillator
    // (HSI) which runs at 16 MHz.
    
    // All the peripherals need to be clocked. After reset, the clock to all peripherals 
    // is initially stopped. The software is responsible for enabling clock to required
    // peripherals.
    ///////////////////////////////////////////////////////////////////////////
    
    
    //////////////////////////// REGISTERS AND PERIPHERALS ////////////////////
    // The registers are just locations on the memory.
    // Peripherals are hardware devices in the microcontroller e.g. GPIO, Timers
    // ADC, USB, etc. Learning to work with a microcontroller is actually
    // learning to configure and use employ its peripherals for your purposes.
    
    // The registers and peripherals are connected to the core using a bus matrix.
    // The busses are also clocked. Different bus can run at different speeds and have their
    // own max speed and therefore it is necessary to set their clocks appropriately.
    
    // There are two main kinds of busses:
    // 	Advanced High Speed Bus (AHB)
    // 	Advanced Peripheral Bus (APB)
    
    // The Flash memory is connected to the core using a dedicated Instruction bus (I-bus).
    
    // STM32F407 has the following busses: AHB1, AHB2, AHB3, APB1 (via AHB1), APB2 (via AHB1).
    
    // Peripherals on AHB can run at the system clock (168 MHz max on this device).
    // Peripherals on APB1 can run at max 42 MHz.
    // Peripherals on APB2 can run at max 84 MHz.
    
    // Other peripherals are connected differently.
    // In STM32F407VG General Purpose Input Output (GPIO) ports are on AHB1.
    
    // Each peripheral is controlled by a group of registers. As registers are just
    // memory locations, they are accessed through pointers.
    ///////////////////////////////////////////////////////////////////////////
    
    
    ///////////////////////////// BASIC STEPS /////////////////////////////////
    // Initializing a peripheral on STM32 requires the following steps:
    // 	1- Enable the clock to the perihpheral.
    // 	2- Configure pins required by the peripheral.
    // 	3- Configure peripheral hardware.
    ///////////////////////////////////////////////////////////////////////////
    
    
    /////////////////// RESET AND CLOCK CONTROL REGISTER GROUP ////////////////
    // RCC is the Reset & Clock Control register group. All oscillator, PLL and
    // bus configuration registers are located in this group. Its consists of:
    // 	A Clock control register (CR).
    //	A PLL Configuration register (PLLCFR).
    //	A Clock Configuration register (CIR).
    //	A reset register per bus (xxxRSTR).
    //	A clock enable register per bus (xxxENR).
    // 	A clock enable in low power mode register per bus (xxxLPENR).
    ///////////////////////////////////////////////////////////////////////////
    */
    
#ifdef BASIC_1
    Basic1();
#endif
    
#ifdef BASIC_2
    Basic2();
#endif
    
#ifdef BASIC_3
    Basic3();
#endif
    
#ifdef BASIC_4
    Basic4();
#endif
    
#ifdef BASIC_5
    Basic5();
#endif
    
#ifdef BASIC_6
    Basic6();
#endif
    
#ifdef BASIC_7
    Basic7();
#endif
    
#ifdef BASIC_8
    Basic8();
#endif
}


#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
  while (1)
  {
  }
}
#endif