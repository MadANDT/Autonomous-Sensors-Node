/**
  ******************************************************************************
  * @file    STOP1_example.h
  * @author  Andriatsilavo Matteo
  * @brief   Header file of the functions used in STOP1 mode project.
  ******************************************************************************
  */

#ifndef STOP1_EXAMPLE_H
#define STOP1_EXAMPLE_H

#include "stm32wlxx_hal.h" // or your relevant HAL header
#include <stdio.h>

extern UART_HandleTypeDef huart2;

/** @brief  Redirect the output of printf
  * @retval int
  */
int __io_putchar(int ch);

/** @brief Simple software delay (blocking)
  * @param ms: approximate milliseconds to delay (not precise, depends on CPU speed)
  * @retval None
  */
void delay(uint32_t ms);

/**  @brief Easier way to reduce / increase HCLK1 frequency by
  * changing the AHB divider, without reconfiguring oscillators or PLL.
  * @param dividerX: accepts the following values:
  * 2, 3, 4, 5, 6, 8, 10, 16, 32, 64, 128, 256, 512.
  * Any other value does not divide SYSCLK (1 does not divide, >512 divides too much,
  * and by default, any unusual value will not divide).
  * @retval None
 */
void Divide_HCLK1_By(uint16_t dividerX);

/** @brief Makes the CPU goes into low power run mode (LPRun),
  * according to the datasheet guidance. HSE32 is not used,
  * HSI16 is.
  * @retval None
  */
void Enter_LowPowerRunMode(void);

/** @brief Wakes up the CPU from low power run mode (LPRun),
  * according to the datasheet guidance. HSI16 is used.
  * @param newHCLKFreq: divider applied to SYSCLK (16 MHz here) to increase back
  * HCLK1 frequency over 2 MHz, can take the following values:
  * 1, 2, 3, 4, 5, 6, 8, 10, 16, 32, 64, 128, 256, 512, but MUST BE lower than 10;
  * e.g. among the nine first values.
  * @retval None
  */
void Exit_LowPowerRunMode(uint8_t newHCLKFreq);

/** @brief Makes the CPU go into STOP1 mode,
  * according to the datasheet guidance.
  * We are waiting on an Interrupt (WFI).
  * @retval None
  */
void Enter_STOP1Mode_OnWFI(void);



#endif // STOP1_EXAMPLE_H
