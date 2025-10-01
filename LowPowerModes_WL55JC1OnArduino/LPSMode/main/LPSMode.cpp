/**
  ******************************************************************************
  * @file           : LPSMode.cpp
  * @brief          : This file provides code for the application demonstration.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"
#include "LPSMode.h"
#include "LPS_MX_Config.h"
#include "LPS_SystemClock_Config.h"

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
  MX_RTC_Init();         // RTC
}

/**
  * @brief LED sequence when entering LPS mode.
  * Blue and red LEDs blink three times quickly.
  * @param None
  * @retval None
 */
void LPS_Entry_LEDSequence(void){
	for (int _ = 0; _ < 6; _++){
		HAL_GPIO_TogglePin(GPIOB,  BLED_Pin);
		HAL_GPIO_TogglePin(GPIOB,  RLED_Pin);
		HAL_Delay(50);
	}
}

/**
  * @brief LED sequence when exiting LPS mode.
  * Blue and red LEDs blink three times slowly.
  * @param None
  * @retval None
 */
void LPS_Exit_LEDSequence(void){
	for (int _ = 0; _ < 6; _++){
		HAL_GPIO_TogglePin(GPIOB,  BLED_Pin);
		HAL_GPIO_TogglePin(GPIOB,  RLED_Pin);
		HAL_Delay(200);
	}
}

/* @brief Easier way to reduce / increase HCLK1 frequency by
 * changing the AHB divider, without reconfiguring oscillators or PLL.
 * Assumes SYSCLK is initially set at 48 MHz.
 * Use register-access instructions (according to datasheet guidance),
 * on RCC clock configuration register (RCC_CFGR).
 * @params dividerX accepts the following values:
 * 2, 3, 4, 5, 6, 8, 10, 16, 32, 64, 128, 256, 512.
 * Any other value does not divide SYSCLK (1 does not divide, >512 divides too much,
 * and by default, any unusual value will not divide).
 * @retval None
 */
void Divide_HCLK1_By_X(uint16_t dividerX = 1){
    uint8_t HCLK1_prescaler;
    if (dividerX >= 2 || dividerX <= 512){
      switch(dividerX){
        case 2:   HCLK1_prescaler = 8;    break; // "1000"
        case 3:   HCLK1_prescaler = 1;    break; // "0001"
        case 4:   HCLK1_prescaler = 9;    break; // "1001"
        case 5:   HCLK1_prescaler = 2;    break; // "0010"
        case 6:   HCLK1_prescaler = 5;    break; // "0101"
        case 8:   HCLK1_prescaler = 10;   break; // "1010"
        case 10:  HCLK1_prescaler = 6;    break; // "0110"
        case 16:  HCLK1_prescaler = 11;   break; // "1011"
        case 32:  HCLK1_prescaler = 7;    break; // "0111"
        case 64:  HCLK1_prescaler = 12;   break; // "1100"
        case 128: HCLK1_prescaler = 13;   break; // "1101"
        case 256: HCLK1_prescaler = 14;   break; // "1110"
        case 512: HCLK1_prescaler = 15;   break; // "1111"
        default:  HCLK1_prescaler = 0; break; // "0000", no division
      }
    } else HCLK1_prescaler = 0; // no division

    // Reset the current prescaler value into bits 7 to 4 (HPRE)
    RCC -> CFGR &= ~(0xF << 4);
    // Insert the new prescaler value
    RCC -> CFGR |= (HCLK1_prescaler << 4);
    /* Wait for the hardware to aknowledge the new prescaler value
     * using HCLK1 prescaler flag (HPREF) on bit 16:
     * 0 → HCLK1 prescaler value not yet applied
     * 1 → HCLK1 prescaler value applied (RCC -> CFGR & (1 << 16))
     * OR using LL function `LL_RCC_IsActiveFlag_HPRE(void)`
     */ while(!(LL_RCC_IsActiveFlag_HPRE())){}
}

/** @brief Makes the CPU goes into low power sleep mode,
  * according to the datasheet guidance.
  * For this implementation, HSE32 is not used.
  * @retval None
  */
void Enter_LowPowerSleepMode(void){
  // 1. Indicate we're about to enter LPS mode (optionnally).
  LPS_Entry_LEDSequence()

	// 2. Disable HSE32 clock (optionnal if not used, which is our case here).

	// 3. Decrease the HCLK clock frequencies below 2 MHz, supposed HCLK = 48 MHz.
	// 48 MHz / X ≤ 2 MHz → X ≥ 24; but 24 is not a usual value of divider
	// among 1, 2, 3, 4, 5, 6, 8, 10, 16, 32, 64, 128, 256 and 512.
	// So we take the next higher value, 32.
	Divide_HCLK1_By_X(32);

	// 4. Suspend the tick to prevent the Tick interrupt every 1 ms
	HAL_SuspendTick();

  // 5. Enter Sleep mode (reminder: LPS = LPR + Sleep)
	HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

/** @brief Wakes up the CPU from low power run mode,
  * according to the datasheet guidance.
  * For this first test, HSE32 is not used.
  * @params newHCLKFreq is the divider applied to SYSCLK (48 MHz) to increase back
  * HCLK frequency over 2 MHz, can take the following values:
  * 1, 2, 3, 4, 5, 6, 8, 10, 16, 32, 64, 128, 256, 512, but MUST BE lower than 24;
  * e.g. among the nine first values.
  * @retval None
  */
void Exit_LowPowerSleepMode(uint8_t newHCLKFreq = 1){
  // 1. Clear the LPS bit in PWR_CR1, and,
	// 2. wait until REGLPF bit is cleared in PWR_SR2.
	HAL_PWREx_DisableLowPowerRunMode();

	// 3. Increase the HCLK clock frequencies over 2 MHz, supposing HCLK = 48 MHz.
	// 48 MHz / X ≥ 2 MHz → X ≤ 24; but 24 is not
	// a usual value of divider (1, 2, 3, 4, 5, 6, 8, 10, 16, 32, 64, 128, 256, 512).
	// So we can take one of the nine lower values, 1 to 16.
	Divide_HCLK1_By_X(newHCLKFreq);
	// (HSE32 is not used here, so we skip this optionnal step.)

  // 4. Indicate we're exiting LPS mode (this is optionnal,
  // AND you might need to reset the MX configuration beforehand).
  // MX_GPIO_Init();
  LPS_Exit_LEDSequence();
}

