/**
  ******************************************************************************
  * @file           : StandbyMode.cpp
  * @brief          : This file provides code for the application demonstration.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"
#include "StandbyMode.h"
#include "Standby_MX_Config.h"
#include "Standby_SystemClock_Config.h"

/* External variables / macros -----------------------------------------------*/

/**
  * @brief Sets up the system by calling the initialization functions :
	* HAL_Init, SystemClock_Congif, MX_<PERIPHERAL>_Init.
  * @param None
	* @note This function is called in the `setup` Arduino function.
  * @retval None
 */
void System_Setup(void){
	HAL_Init();             // Reset of all peripherals, initializes the Flash interface and the SysTick
  SystemClock_Config();   // Configure the System clock
  // Initialize all configured peripherals
  MX_GPIO_Init();         // GPIO
}

/**
  * @brief LED sequence when entering Standby mode.
  * Green and red LEDs blink three times quickly.
  * @param None
  * @retval None
 */
void Standby_Entry_LEDSequence(void){
	for (int _ = 0; _ < 6; _++){
		HAL_GPIO_TogglePin(GPIOB,  GLED_Pin);
		HAL_GPIO_TogglePin(GPIOB,  RLED_Pin);
		HAL_Delay(50);
	}
}

/**
  * @brief LED sequence when exiting Standby mode.
  * Green and red LEDs blink three times slowly.
  * @param None
  * @retval None
 */
void Standby_Exit_LEDSequence(void){
	for (int _ = 0; _ < 6; _++){
		HAL_GPIO_TogglePin(GPIOB,  GLED_Pin);
		HAL_GPIO_TogglePin(GPIOB,  RLED_Pin);
		HAL_Delay(200);
	}
}

/** @brief Checks if the system has woken up from Standby mode.
  * If so, a sequence of blinking LEDs is launched.
  * @retval None
  */
void ResumeFromStandby(void){
	/* Check if the system was resumed from Standby mode */
	if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET){
		/* Clear Standby flag */
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);

		// Reinitialize the GPIO peripheral
		MX_GPIO_Init();
		// Start blinking LEDs sequence
		Standby_Exit_LEDSequence();
	}
	// Clear the Wake-Up flag of all pins
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	// Clear all related wakeup flags - e.g. wake-up pin 1 flag
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF1);
}
