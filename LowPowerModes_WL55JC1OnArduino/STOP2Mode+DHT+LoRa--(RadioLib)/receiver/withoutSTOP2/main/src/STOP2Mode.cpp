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
#include <STM32RTC.h>
#include "Radio.h" // RadioLib header for LoRa modem functionality

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

/**
  * @brief Enter STOP2 mode for DELAY_S seconds,
  * wake up on RTC alarm interrupt.
  * @param delay_s: sleep duration offset in seconds
  * @retval None
 */
void Enter_STOP2Mode_WithRTCAlarm(uint16_t delay_s, uint8_t stop2_mode) {
  STOP2_Entry_LEDSequence();  // Indicate the entry in STOP2 mode with the LED sequence
  rtcAlarmFlag = false;
  
  // Put radio in sleep mode while retaining configuration to minimize current 
  radio.sleep(true);

  RTC_Setup(delay_s);  
  
  HAL_SuspendTick();          // Suspend the SysTick Increment

  HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
  /* ####----####----####----####
	 * MCU is asleep in STOP2 mode
	 * ####----####----####----####
	 */

  HAL_ResumeTick();           // Resume the SysTick Increment

  // Wake radio up (standby) after STOP2
  radio.standby();

  STOP2_Exit_LEDSequence();   // Indicate the exit in STOP2 mode with the LED sequence

  // Wait for alarm flag (should be set by callback)
  while (!rtcAlarmFlag) {
      // Optionally, sleep or do nothing
  }

  // After wake-up, additional actions can be performed for transmitter mode
  if (stop2_mode == STOP2_MODE_TRANSMITTER) {
    transmittedFlag = false;
    radio.finishTransmit();
  }
  // De-initialize the RTC
  rtc.end();

  // Reset system part
  SystemClock_Config();
}
