#pragma once

#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_dbgmcu.h>

// Change the number to select the program to run.
// This is necessary because different programs may use different
// interrupt handlers or other things differently which may pose a problem.
#define BASIC_4