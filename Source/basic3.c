//////////////////////////////// BASIC 3 //////////////////////////////////////
///////////////////////////// INTERRUPTS //////////////////////////////////////
// Demonstrates the use of interrupts and external interrupts.
// Toggles LEDS through the pushbutton on the board.


#include "stdafx.h"
#include "basic3.h"


static void SetupLEDs();
static void SetupPushButton();
static void SetupExternalInterruptSource();
static void ConfigurePushButtonInterrupt();
static void ConfigureNVIC();
static void ConfigureEXTI();

// Interrupt handlers need the C caller convention. This is necessary if
// the code is compiled as C++ code.
#ifdef __cplusplus
extern "C" {
#endif

#ifdef BASIC_3
void EXTI0_IRQHandler();
#endif

#ifdef __cplusplus
}
#endif


void Basic3()
{
    SetupLEDs();
    ConfigurePushButtonInterrupt();
    
    
    while (1)
    {
    }
}


static void SetupLEDs()
{
    // From Basic1.
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    GPIO_InitTypeDef init;
    init.GPIO_Mode = GPIO_Mode_OUT;
    init.GPIO_OType = GPIO_OType_PP;
    init.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    init.GPIO_Speed = GPIO_Speed_2MHz;
    
    GPIO_Init(GPIOD, &init);
    
    GPIO_SetBits(GPIOD, GPIO_Pin_12);
}


static void ConfigurePushButtonInterrupt()
{    
    // ARM Cortex processors have an interrupt controller called the
    // Nested Vector Interrupt Controller (NVIC) can receive
    // interrupts from most hardware built in the microcontroller and can also
    // receive interrupts from ports through the External Interrupt/Event
    // Controller (EXTI). NVIC is part of the ARM Cortex processors and therefore
    // is not specific to the STM32 family. Being a core peripheral it is not
    // clocked either.
    
    // STM32F407 has 82 maskable-interrupts. It has 16 interrupt priority levels.
    // (The Cortex-M core can support upto 256 levels but only 16 are implemented
    // in this microcontroller).
    // The Cortex core allows to divide these levels into preemption priority and
    // sub-priority. Interrupts at the same preemption priority are handled on the
    // basis of their sub-priority.
    // Lower interrupt priority level means higher priority. So for this device
    // 0 is the highest priority level and 15 is the lowest.
    
    
    // Using the interrupt capability requires the following:
    
    //	1- The interrupt must be INITIALIZED and ENABLED in the NVIC.
    //	2- An interrupt handler function must be written.
    //	3- For port EXTI, additional steps are required:
    //		a-	Pin must be configured as input floating.
    //		b-	EXTI source must be selected in System Configuration
    //				Controller (SYSCFG) peripheral.
    //		c-	EXTI line must be initialized in EXTI peripheral.
    
    
    // The STM32F4 device provides 16 external interrupt lines connected
    // to GPIO pins which can be mapped to any pin. The line 0 can be mapped to 
    // pin 0 of any port. Line 1 can be mapped to P1 of any port and so on. 
    // Not all interrupt lines have their own handler, some are grouped.
    
    // Enable the interrupt in the NVIC.
    ConfigureNVIC();
    
    // Configure the EXTI in the EXTI peripheral.
    ConfigureEXTI();
    
    // To setup the pushbutton GPIO pin.
    SetupPushButton();
    
    // Configure the EXTI interrupt source.
    SetupExternalInterruptSource();
}


static void ConfigureNVIC()
{
    // As NVIC is part of ARM Cortex cores, it is not defined in the ST standard
    // peripheral library but in CMSIS. But the standard library provides a
    // convenient struct to initialize an interrupt.
    
    // CMSIS provides the following NVIC functions:
    //	NVIC_EnableIRQ(IRQn_Type IRQ)
    //	NVIC_DisableIRQ(IRQn_Type IRQ)
    //	NVIC_SetPendingIRQ(IRQn_Type IRQ)
    //	NVIC_ClearPendingIRQ(IRQn_Type IRQ)
    //	NVIC_GetPendingIRQ(IRQn_Type IRQ)
    //	NVIC_DisableIRQ(IRQn_Type IRQ)
    //	NVIC_SetPriority(IRQn_Type IRQ, u32 Priority)
    //	NVIC_GetPriority(IRQn_Type IRQ)
    
    // In standard peripheral library, an interrupt is initialized in the following struct.
    NVIC_InitTypeDef init;
    
    // The interrupt to initialize. Refer to the device Reference manual and
    // the device's header file (stm32f4xx.h) for available interrupts.
    // Here we enable the external interrupt line 0.
    // The User push button on the Discovery board is on PA0 (Pin A0).
    // So we use external interrupt line 0.
    init.NVIC_IRQChannel = EXTI0_IRQn;
    
    // The state of the interrupt: ENABLE or DISABLE.
    init.NVIC_IRQChannelCmd = ENABLE;
    
    // The two priority levels. The 16 levels (4 bits) of the device are divided
    // among these 2 fields. So make sure you are consistent with your values.
    init.NVIC_IRQChannelPreemptionPriority = 0;
    init.NVIC_IRQChannelSubPriority = 0;
    
    // This function is called to initialize the interrupt.
    NVIC_Init(&init);
}


static void ConfigureEXTI()
{
    // EXTI is important for power control of STM32. It is not a clocked
    // peripheral. It can be used to wake up the device from low power modes
    // in which the oscillators are halted.
    
    // An EXTI line is initialized as follows.
    EXTI_InitTypeDef init;
    
    // The Line(s) to configure. ORed combinations allowed.
    init.EXTI_Line = EXTI_Line0;
    // Functional State of the line(s): ENABLE or DISABLE.
    init.EXTI_LineCmd = ENABLE;
    // Mode of line(s). Interrupt or Event. Interrupt produces an interrupt signal.
    // Event only wakes the processor from low power halted mode.
    init.EXTI_Mode = EXTI_Mode_Interrupt;
    // Interrupts are edge-detected so trigger specifies which edge to trigger on.
    // Can be (Rising, Falling, Rising_Falling).
    init.EXTI_Trigger = EXTI_Trigger_Rising;
    
    EXTI_Init(&init);
}


static void SetupPushButton()
{
    // Clock the port.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
    // Initialize the struct.
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode = GPIO_Mode_IN;
    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    // Speed and OType are for necessary Output modes only.
    // Push-Pull is only necessary for Input modes.
    
    // Initialize the port.
    GPIO_Init(GPIOA, &gpio);
}


static void SetupExternalInterruptSource()
{
    // The external interrupt line sources are managed by the System Configuration
    // Controller (SYSCFG) peripheral.
    
    // System Configuration Controller, SYSCFG, Being a peripheral it must be
    // clocked before it can be used.
    // Clock the SYSCFG.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    // Configure the interrupt line source.
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
}



// This is the interrupt handler. The name of the handler is defined in the
// startupt_stm32f4xx.s inside the vector table. The vector table defines addresses
// of all interrupt handlers.
// The file also provide weak definitions of all interrupts
// which start an infinite loop. When you provide your own definition, the linker
// discards the weak ones. The condition is that the name of the handler must match
// the one defined in the vector table.
#ifdef BASIC_3

void EXTI0_IRQHandler()
{
    static u16 pins[] = {
	GPIO_Pin_12,
	GPIO_Pin_13,
	GPIO_Pin_14,
	GPIO_Pin_15
    };
    
    static u16 currentPin = 0;
    
    // The interrupt pending bit of an EXTI line is not cleared automatically.
    // We must clear it otherwise the interrupt will be generated again.
    EXTI_ClearITPendingBit(EXTI_Line0);
    // The EXTI peripheral can also generate interrupts through software by
    // setting the appropriate bit in the Software Interrupt/Event Register (SWIER)
    // The function EXTI_GenerateSWInterrupt(u32 EXTI_Linex) makes it easy to do so.
    
    // The status of an interrupt line can also be queried using:
    // ITStatus EXTI_GetITStatus(u32 EXTI_Linex).
    // where ITStatus is an enum of (RESET, SET).
    
    int nextPin = (currentPin + 1) % 4;
    GPIO_ToggleBits(GPIOD, pins[currentPin] | pins[nextPin]);
    currentPin = nextPin;
}

#endif