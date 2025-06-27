/**
  ******************************************************************************
  * @file           : STOP2Mode.cpp
  * @brief          : This file provides code for the application demonstration.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"
#include "STOP2Mode.h"
#include "STOP2_MX_Config.h"
#include "STOP2_SystemClock_Config.h"

/* External variables / macros -----------------------------------------------*/

/**
  * @brief Sets up the system by calling the initialization functions :
	* HAL_Init, SystemClock_Congif, MX_<PERIPHERAL>_Init.
  * @param None
	* @note This function has to be called in the `setup` Arduino function.
  * @retval None
 */
void System_Setup(void){
	HAL_Init();             // Reset of all peripherals, initializes the Flash interface and the SysTick
  SystemClock_Config();   // Configure the System clock
  // Initialize all configured peripherals
  MX_GPIO_Init();         // GPIO
}

/**
  * @brief LED sequence when entering STOP2 mode.
  * Green and red LEDs blink three times quickly.
  * @param None
  * @retval None
 */
void STOP2_Entry_LEDSequence(void){
	for (int _ = 0; _ < 6; _++){
		HAL_GPIO_TogglePin(GPIOB,  GLED_Pin);
		HAL_GPIO_TogglePin(GPIOB,  RLED_Pin);
		HAL_Delay(50);
	}
}

/**
  * @brief LED sequence when exiting STOP2 mode.
  * Green and red LEDs blink three times slowly.
  * @param None
  * @retval None
 */
void STOP2_Exit_LEDSequence(void){
	for (int _ = 0; _ < 6; _++){
		HAL_GPIO_TogglePin(GPIOB,  GLED_Pin);
		HAL_GPIO_TogglePin(GPIOB,  RLED_Pin);
		HAL_Delay(200);
	}
}

