/**
  ******************************************************************************
  * @file           : My_Error_Handler.cpp
  * @brief          : This file provides code for the user-defined error handler.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "My_Error_Handler.h"
#include "STOP1Mode.h"

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @note   This function can be modified to handle errors in a user-defined way.
  * @retval None
  */
void My_Error_Handler(void){
    while (1) {
      // Example: blink a LED to indicate error
      HAL_GPIO_TogglePin(GPIOB, RLED_Pin);
      HAL_Delay(100);
        
    }
}
