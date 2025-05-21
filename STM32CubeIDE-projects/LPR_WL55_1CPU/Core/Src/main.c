/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief Redirect the output of printf to USART2
  * @retval int
  */
int __io_putchar(int ch){
	HAL_UART_Transmit(&huart2, &ch, 1, 1000);
}

/* @brief Easier way (attempt #2) to reduce / increase HCLK1 frequency by
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
void Divide_HCLK1_By_X(uint16_t dividerX){
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

/** @brief Makes the CPU goes into low power run mode,
  * according to the datasheet guidance.
  * For this first test, HSE32 is not used.
  * @retval None
  */
void Enter_LowPowerRunMode(void){
	// 1. set the FPDR bit in PWR_CR1.
	HAL_PWREx_EnableFlashPowerDown(PWR_FLASHPD_LPRUN);

	// 2. Disable HSE32 clock (optionnal if not used, which is our case here).

	// 3. Decrease the HCLK clock frequencies below 2 MHz.
	// 48 MHz / X ≤ 2 MHz → X ≥ 24; but 24 is not a usual value of divider
	// among 1, 2, 3, 4, 5, 6, 8, 10, 16, 32, 64, 128, 256 and 512.
	// So we take the next higher value, 32.
	Divide_HCLK1_By_X(32);

	// 4. Set the LPR bit in PWR_CR1.
	HAL_PWREx_EnableLowPowerRunMode();

	// 5. Check if the low-power regulator is ready,
	// by checking the REGLPS bit (PWR_SR2, bit 8)
	while(!(LL_PWR_IsActiveFlag_REGLPS())){}
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
void Exit_LowPowerRunMode(uint8_t newHCLKFreq){
	// 1. Clear the LPR bit in PWR_CR1, and,
	// 2. wait until REGLPF bit is cleared in PWR_SR2.
	HAL_PWREx_DisableLowPowerRunMode();

	// 3. Increase the HCLK clock frequencies over 2 MHz.
	// 48 MHz / X ≥ 2 MHz → X ≤ 24; but 24 is not
	// a usual value of divider (1, 2, 3, 4, 5, 6, 8, 10, 16, 32, 64, 128, 256, 512).
	// So we can take one of the nine lower values, 1 to 16.
	Divide_HCLK1_By_X(newHCLKFreq);
	// (HSE32 is not used here, so we skip this optionnal step.)

	// 4. Reinitialize the USART2 peripheral (since changing the HCLK frequency
	// impacts the related peripherals, such as USART2).
	MX_USART2_UART_Init();
}

/**
  * @brief Simple software delay (blocking)
  * @param ms: approximate milliseconds to delay (not precise, depends on CPU speed)
  * @retval None
  */
void delay(uint32_t ms)
{
    uint32_t freq = HAL_RCC_GetHCLKFreq();
    uint32_t count = (freq / 4000) * ms; // 4 cycles per loop (approx.)
    while(count--) {
        __NOP();
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  	  printf("\r\n\r\nTest d'une appli mode veille sur STM32\r\n");

  while (1)
  {
	  printf("Exécution de la boucle While\r\n");
	  printf("Le µCU entre en mode Low Power Run pendant x secondes\r\n\r\n");
	  __HAL_RCC_USART1_CLK_SLEEP_DISABLE(); // Clock-gating the USART2 peripheral

	  Enter_LowPowerRunMode();

	  /* ####----####----####----####----####
	   * MCU is running in low power run mode
	   * ####----####----####----####----####
	   */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 24;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3|RCC_CLOCKTYPE_HCLK
                              |RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, blue_LED_Pin|green_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, FE_CTRL3_Pin|FE_CTRL2_Pin|FE_CTRL1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : blue_LED_Pin green_LED_Pin */
  GPIO_InitStruct.Pin = blue_LED_Pin|green_LED_Pin;
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

  /*Configure GPIO pins : UB1_Pin UB2_Pin */
  GPIO_InitStruct.Pin = UB1_Pin|UB2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : UB3_Pin */
  GPIO_InitStruct.Pin = UB3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(UB3_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/** @brief User-defined ISR when pressing any of the buttons,
  * text is displayed and LEDs are blinking
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	// Exit the low power run mode before doing anything else;
	// we're going back to 48 MHz, for this first test.
	Exit_LowPowerRunMode(1);

	// If user button 1 (UB1, on PA0) is pressed,
	// the according text is displayed
	if (GPIO_Pin == UB1_Pin){ printf("Sortie par pression du Boutton 1\r\n");
		  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15); delay(200);
		  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15); delay(200);
		  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15); delay(200);
		  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15); delay(200);
	}

	// If user button 2 (UB2, on PA1) is pressed,
	// the according text is displayed
	if (GPIO_Pin == UB2_Pin){ printf("Sortie par pression du Boutton 2\r\n");
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15); delay(200);
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15); delay(200);
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9); delay(200);
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9); delay(200);
	}

	// If user button 3 (UB3, on PC6) is pressed,
	// the according text is displayed
	if (GPIO_Pin == UB3_Pin){ printf("Sortie par pression du Boutton 3\r\n");
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9); delay(200);
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9); delay(200);
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9); delay(200);
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9); delay(200);
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
