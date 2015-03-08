//////////////////////////////// BASIC 1 //////////////////////////////////////
///////////////////////////////// GPIO ///////////////////////////////////////
// Demonstrates the basics of peripherals and how to work with General Purpose
// I/O (GPIO) port peripheral.
// Toggles LEDs using rough timing.

#include "stdafx.h"
#include "basic1.h"


static void Enable_PeripheralClocks();
static void Initialize_GPIO();
static void Wait(u16 milli);


void Basic1()
{
    Enable_PeripheralClocks();
    Initialize_GPIO();
    
    // Toggle through the 4 LEDs on board.
    while (1)
    {
	GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
	GPIO_ResetBits(GPIOD, GPIO_Pin_15);
	Wait(500);
	
	GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13);
	Wait(500);
	
	GPIO_ToggleBits(GPIOD, GPIO_Pin_13 | GPIO_Pin_14);
	Wait(500);
	
	GPIO_ToggleBits(GPIOD, GPIO_Pin_14 | GPIO_Pin_15);
	Wait(500);
    }
}


// A crude implementation of delay.
static void Wait(u16 milli)
{
    u32 m = SystemCoreClock / 2000;
    for (u32 i=0; i<milli; i++)
    {
    	for (u32 j=0; j<m; j++)
    	{
	    asm("nop");
    	}
    }
}


static void Enable_PeripheralClocks()
{
    // To enable the clock to a specific register or peripheral one needs to look
    // in to the Reference Manual to see on which bus it is located and also the
    // bit on the bus's register which enables it. And then set the appropriate
    // enable bit of the appropriate register in the RCC group.
    // Or we can use the Standard Peripheral library provided by ST.
    
    // Here the GPIO ports are on AHB1. STM32F407VG has 5 16-bit ports labeled A-E.
    // These occupy the first 4 bits on the AHB1ENR register. Port D is bit 3.
   
    ////////////////////////// ENABLING A PORT CLOCK //////////////////////////
    // Enable Port D clock.
    //RCC->AHB1ENR |= (1 << 3);
    
    // By using the std peripheral library.
    // 1st argument values can be found in the source file of the RCC peripheral.
    // 2nd argument can be ENABLE or DISABLE.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    ///////////////////////////////////////////////////////////////////////////
    
}


static void Initialize_GPIO()
{
    /*
    ////////////////////////////////// GPIO ///////////////////////////////////
    // STM32F407 has 5 GPIO ports with 16-bits each.
    // Each GPIO is a seperate peripheral.
    // Each pin of each port can be configured individually.
    // Upon reset, all GPIO pins are placed in the alternate function 0 input floating 
    // state except for a few.
   
    // These few pins are related to debugging. The ST's Serial Wire Debug (SWD)
    // interface.
    // these pins are A13 and A14 on this controller.
    // SWD uses an additional debugger software enabled pin B3 for tracing.
    // JTAG debugging interface uses some additional pins (PB4, PA15)
    // All the debugging pins are in AF (Alternate function) pull-up/down mode after reset.
    // The F4 Discovery board however doesn't support JTAG.
    
    // A GPIO pin can be configured by software in several modes:
    //	Input floating
    //	Input pull-down
    //	Input pull-up
    //	Analog
    //	Output open drain
    //	Output push-pull
    //	Alternate function push-pull with pull up/down capability
    //	Alternate function open-drain with pull up/down capability
    
    // Other peripherals on the controller use the GPIO pins for their I/O.
    // Different GPIO pins have specific "Alternate" functions assigned to them
    // by the vendor. To use a pin for a peripheral's function it must be in the
    // alternate function mode. Only one alternate function may be active on a pin
    // at any given time.
    // Each pin can have upto 16 alternate functions. AF0 (Alternate Function)
    // is the system's alternate function. AF1-13 are the peripheral's alternate
    // functions. AF14 is not used and AF15 is EVENTOUT which is used to send a
    // signal to another microcontroller to wake it from its low power state.
    
    // Each GPIO port has 4 32-bit configuration registers (2 bits per pin):
    //	MODER to select I/O direction (Input, Output, AF, analog)
    //	OTYPER to select output type (push-pull, open-drain)
    //	OSPEEDR to select output speed
    //	PUPDR to select pull-up / pull-down (Pins have internal resistors to
    //	allow for pull up/down).
    
    // Each port has 2 I/O data registers:
    //	IDR (read only) to read data input to the port.
    //	ODR (read-write) to read-write data to be output.
    
    // Each port has a 32-bit register, BSSR (Bit Set Reset Register), for bit-wise handling.
    //	It is divided into 2 parts high and low, BSSRH and BSSRL respectively.
    //	Writing a 1 to BSSRL sets the corresponding port bit.
    //	Writing a 1 to BSSRH clears the corresponding port bit.
    //	Writing 0 does not have any effect.
    
    // Each port has 2 32-bit Alternate function registers, AFR[0..1], to select the
    // alternate function (1 of 16).
    
    // Each port has additional LCKR register to lock the port configuration from
    // being modified accidently. Locking requires a specific read-write sequence to be applied.
    
    // Each port can also be configured as an external interrupt line. There are 16
    // external interrupt lines in this controller.
    
    // NOTE: this controller has 2 additional pins (H0 and H1) which are used for
    // High Speed Oscillator Input and Output respectively. C14 and C15 are used for
    // LSE Input and Output respectively. These pins can only be used as GPIO when the
    // respective oscillator is off.
    ///////////////////////////////////////////////////////////////////////////
    */
    
    /////////////////////////// CONFIGURING A PORT ////////////////////////////
    // Lets configure the LEDs port pins on the F4 Discovery board for output.
    // This would allow us to turn the LEDs on/off.
    // std peripheral library provides a struct to conveninently configure a port.
    // Using this struct and the initialization struct, one does not need to
    // read the reference manual and calculate the bits to be modified for each
    // pin. One simply provides the pins to be modified and their configuration.
    GPIO_InitTypeDef gpioInit;
    
    // Pin is a bit flag. Pins are defined by macros GPIO_Pin_x where x is 0-15.
    // On the F4 Discovery board pins 12-15 of Port D are connected to LEDs.
    gpioInit.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    // Mode takes GPIO_Mode_x where x can be (IN, OUT, AF, AN).
    gpioInit.GPIO_Mode = GPIO_Mode_OUT;
    // OType takes GPIO_OType_x where x can be (PP, OD).
    gpioInit.GPIO_OType = GPIO_OType_PP;
    // Speed takes GPIO_Speed_xMHz where x can be (2, 25, 50, 100).
    gpioInit.GPIO_Speed = GPIO_Speed_2MHz;
     // PuPd takes GPIO_PuPd_x where x can be (NOPULL, UP, DOWN).
    gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    // Configure a port requires calling this method. First arguments is the
    // port memory location. Port memory locations are defined in CMSIS by macros
    // GPIOx where x is the port letter. 2nd argument is the above struct.
    GPIO_Init(GPIOD, &gpioInit);
    ///////////////////////////////////////////////////////////////////////////
    
    //////////////////////////// CHANGING PORT BITS ///////////////////////////
    // Std peripheral library provides the following functions for GPIO ports
    // I/O.
    // GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13);
    // GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13);
    // GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13);
    // GPIO_WriteBit(GPIOD, GPIO_Pin_12, Bit_SET);
    // GPIO_WriteBit(GPIOD, GPIO_Pin_13, Bit_RESET);
    // GPIO_Write(GPIOD, 0x100);
    // u16 a = GPIO_ReadOutputData(GPIOD);
    // a = GPIO_ReadInputData(GPIOD);
    // u8 b = GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_12);
    // b = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12);
    ///////////////////////////////////////////////////////////////////////////
     
     // A port can also be reset to its hardware reset value by:
     //GPIO_DeInit(GPIOD);
}