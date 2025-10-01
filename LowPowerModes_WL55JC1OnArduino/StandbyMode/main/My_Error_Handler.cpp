/**
  ******************************************************************************
  * @file           : My_Error_Handler.cpp
  * @brief          : This file provides code for the user-defined error handler.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "My_Error_Handler.h"
#include "StandbyMode.h"

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @note   This function can be modified to handle errors in a user-defined way.
  * @retval None
  */
void My_Error_Handler(void){
  uint8_t i = 0;
  while (1){
	  i = (i > 3) ? 0: i++;
	  HAL_GPIO_TogglePin(GPIOB,  RLED_Pin);	HAL_Delay(100);
	  if (i == 0) HAL_GPIO_TogglePin(GPIOB,  GLED_Pin);
  }
}
