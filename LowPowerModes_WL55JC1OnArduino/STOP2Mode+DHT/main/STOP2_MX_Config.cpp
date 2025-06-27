/**
  ******************************************************************************
  * @file           : STOP2_MX_Config.cpp
  * @brief          : This file provides code for the peripherals configurations, 
  *                   RTC and GPIO here.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"
#include "STOP2_MX_Config.h"
#include "STOP2Mode.h"
#include "My_Error_Handler.h"

STM32RTC& rtc = STM32RTC::getInstance();
volatile bool rtcAlarmFlag = false;

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
/*static*/ void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GLED_Pin|RLED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, FE_CTRL3_Pin|FE_CTRL2_Pin|FE_CTRL1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : GLED_Pin RLED_Pin */
  GPIO_InitStruct.Pin = GLED_Pin|RLED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : FE_CTRL3_Pin FE_CTRL2_Pin FE_CTRL1_Pin */
  GPIO_InitStruct.Pin = FE_CTRL3_Pin|FE_CTRL2_Pin|FE_CTRL1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : T_VCP_RX_Pin T_VCP_RXA2_Pin */
  GPIO_InitStruct.Pin = T_VCP_RX_Pin|T_VCP_RXA2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/**
  * @brief Sets up the RTC through STM32duino RTC library API and functions, 
  * and the initial sleep duration DEALY_S.
  * @note This function should be called in the setup loop.
  * @param delay_s: sleep duration offset in seconds for the first set up.
  * @retval None
 */
void RTC_Setup(uint16_t delay_s){
  rtc.setClockSource(STM32RTC::LSE_CLOCK, 127, 255);
  rtc.begin();
  rtc.attachInterrupt(RTCAlarmCallback);
  // Set initial time and date (optional, for demo)
  rtc.setTime(0, 0, 0);
  rtc.setDate(1, 1, 25);

  uint8_t hours = rtc.getHours();
  uint8_t minutes = rtc.getMinutes();
  uint8_t seconds = rtc.getSeconds();
  uint32_t subSeconds = rtc.getSubSeconds();

  uint32_t newSubSeconds = subSeconds + SUBSECONDS_OFFSET;
  uint8_t newSeconds = seconds + delay_s;

  if (newSubSeconds >= 1000) { // assuming 1000 ms per second
    newSubSeconds -= 1000;
    newSeconds += 1;
  }
  if (newSeconds >= 60) {
    newSeconds -= 60;
    minutes += 1;
    if (minutes >= 60) {
        minutes = 0;
        hours = (hours + 1) % 24;
    }
  }
  rtc.setAlarmTime(hours, minutes, newSeconds, newSubSeconds);
  rtc.enableAlarm(rtc.MATCH_HHMMSS);
}