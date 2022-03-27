#ifndef __HW_H__
#define __HW_H__

#include "stdio.h"
#include <stdint.h>
#include <stdbool.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx_it.h"
#include "hw-spi.h"
#include "hw-uart.h"
#include "hw-gpio.h"
#include "i2c.h"

#include "sx1280.h"
#include "sx1280-hal.h"


#include "boards.h"

//#define USE_DMA

void HwInit( void );

void HwSetLowPower( void );


void SystemClock_Config( void );
void SystemClock_Config_24M(void);
void SystemClock_Config_1M(void);
void HAL_Delay( uint32_t Delay );



void Error_Handler( void );

#endif // __HW_H__
