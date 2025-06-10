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
/** @brief Redirect the output of printf to USART2
  * @retval int
  */
int __io_putchar(int ch){
	HAL_UART_Transmit(&huart2, &ch, 1, 1000);
}

/** @brief Simple software delay (blocking)
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

/** @brief Makes the CPU go into STOP0 mode,
  * according to the datasheet guidance.
  * We are waiting on Event
  * @retval None
  */
void Enter_STOP0Mode_OnWFE(void){

    /* All EXTI line pending bits (in EXTI pending register
	 * (EXTI_PR1), and EXTI pending register (EXTI_PR2)),
	 * and the peripheral flags generating wake-up interrupts must be cleared.
	 * Otherwise, the Stop 0 mode entry procedure is ignored and program execution continues.
	 * These bits are cleared when writing '1' to them.
	 */
	// EXTI_PR1 - 19 bits: 22, 21, 16 to 0.
	EXTI -> PR1 |= (3 << 21); // bits 22 & 21
	EXTI -> PR1 |= (1 << 16); // bit 16
	EXTI -> PR1 |= (0xFFFF << 0); // bits 15 to 0
	// EXTI_PR2 - 4 bits: 13, 9, 8, 2.
	EXTI -> PR1 |= (1 << 13); // bit 13
	EXTI -> PR1 |= (3 << 8); // bits 9 & 8
	EXTI -> PR1 |= (1 << 2); // bit 2

	// Wait a bit before entering STOP0 mode
	HAL_Delay(1000);

	// Suspend the tick to prevent the SysTick interrupt every 1 ms
	HAL_SuspendTick();

	// Enter the STOP0 mode, waiting for an event: pending interrupt
	HAL_PWREx_EnterSTOP0Mode(PWR_SLEEPENTRY_WFE);
	/* ####----####----####----####
	 * MCU is asleep in STOP0 mode
	 * ####----####----####----####
	 */
}

/** @brief Executes a user-defined sub-routine after the CPU
  * wakes up from STOP0 mode on an event.
  * Some peripherals are (re)initialized, we display text through
  * USART2 and the three LEDs blink.
  * @retval None
  */
void Exit_STOP0Mode_OnWFE(void){
	// At wake-up, resume the SysTick
	HAL_ResumeTick(); // at wake up, we resume the SysTick

	// (Re)Initialize the peripherals
	HAL_Init(); 			// all peripherals, Flash interface and SysTick
	MX_GPIO_Init();			// GPIO
	MX_USART2_UART_Init(); 	// USART2

	printf("Réveil par pression du Bouton 3 et son évènement en attente.\r\n");
	printf("Séquence de clignotement de LED lancée.\r\n");

	// Toggle the red, green and blue LEDs one by one, twice
	for (int _ = 0; _ < 2; _++){
		for (int l = 0; l < 2; l++){
			HAL_GPIO_TogglePin(GPIOB, RLED_Pin);
			delay(100);
		}
		for (int l = 0; l < 2; l++){
			HAL_GPIO_TogglePin(GPIOB, GLED_Pin);
			delay(100);
		}
		for (int l = 0; l < 2; l++){
			HAL_GPIO_TogglePin(GPIOB, BLED_Pin);
			delay(100);
		}
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
	  printf("\r\n\r\nTest d'une appli mode veille profonde sur STM32\r\n");
  while (1)
  {
	  printf("Exécution de la boucle While\r\n");
	  printf("Le µCU entre en mode STOP0 via WFE - type event\r\n\r\n");

	  // Clock-gating the USART2 peripheral
	  __HAL_RCC_USART2_CLK_SLEEP_DISABLE();

	  Enter_STOP0Mode_OnWFE();
	  /* ####----####----####----####
	   * MCU is asleep in STOP0 mode
	   * ####----####----####----####
	   */

	  Exit_STOP0Mode_OnWFE();
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3|RCC_CLOCKTYPE_HCLK
                              |RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  huart2.Init.BaudRate = 9600;
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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, BLED_Pin|GLED_Pin|RLED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, FE_CTRL3_Pin|FE_CTRL2_Pin|FE_CTRL1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : BLED_Pin GLED_Pin RLED_Pin */
  GPIO_InitStruct.Pin = BLED_Pin|GLED_Pin|RLED_Pin;
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

  /*Configure GPIO pin : UB3_Pin */
  GPIO_InitStruct.Pin = UB3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(UB3_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
