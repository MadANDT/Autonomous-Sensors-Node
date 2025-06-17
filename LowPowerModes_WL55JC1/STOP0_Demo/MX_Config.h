// File: MX_Config.h

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32wlxx_hal.h"

extern UART_HandleTypeDef huart2;

void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);

#ifdef __cplusplus
}
#endif
