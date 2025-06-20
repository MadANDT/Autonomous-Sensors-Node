/**
  ******************************************************************************
  * @file           : STOP1_MX_Config.cpp
  * @brief          : This file provides code for the peripherals configurations, 
  *                   RTC and GPIO here.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"
#include "STOP1_MX_Config.h"
#include "STOP1Mode.h"
#include "My_Error_Handler.h"
// RTC_HandleTypeDef hrtc;

// /**
//   * @brief RTC Initialization Function
//   * @param None
//   * @retval None
//   */
// /*static*/ void MX_RTC_Init(void)
// {

//   /* USER CODE BEGIN RTC_Init 0 */

//   /* USER CODE END RTC_Init 0 */

//   /* USER CODE BEGIN RTC_Init 1 */

//   /* USER CODE END RTC_Init 1 */

//   /** Initialize RTC Only
//   */
//   hrtc.Instance = RTC;
//   hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
//   hrtc.Init.AsynchPrediv = 127;
//   hrtc.Init.SynchPrediv = 255;
//   hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
//   hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
//   hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
//   hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
//   hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
//   hrtc.Init.BinMode = RTC_BINARY_NONE;
//   if (HAL_RTC_Init(&hrtc) != HAL_OK)
//   {
//     My_Error_Handler();
//   }

//   /** Enable the WakeUp
//   */
//   if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0x2800, RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0) != HAL_OK)
//   {
//     My_Error_Handler();
//   }
//   /* USER CODE BEGIN RTC_Init 2 */

//   /* USER CODE END RTC_Init 2 */

// }

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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, BLED_Pin|RLED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, FE_CTRL3_Pin|FE_CTRL2_Pin|FE_CTRL1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : BLED_Pin RLED_Pin */
  GPIO_InitStruct.Pin = BLED_Pin|RLED_Pin;
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

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}