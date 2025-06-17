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
#include "STOP1_example.h" // Include our custom header for STOP1 mode example
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
// Define modes using enum
typedef enum {
    MODE_RUN = 0,
    MODE_LPRUN = 1,
    MODE_STOP1 = 2
} PowerMode;
uint8_t currentMode = MODE_RUN; 				// Initialize current mode to RUN
uint8_t nextMode = MODE_RUN;					// Set another variable to store the next mode to be enter
volatile uint8_t button_pressed = 0; 			// Flag waiting for a button reset
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void Switch_Mode(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
    printf("\r\n\r\nTest d'une appli mode veille profonde sur STM32.\r\n");
    printf("Les différents modes testés sont:\r\n");
    printf("\tRUN = 0, LPRUN = 1, STOP1 = 2\r\n");
  while (1)
  {
	  button_pressed = 0; // Reset flag before waiting
	  printf("Exécution de la boucle While.\r\n");
	  printf("Modes disponibles:\r\n");
	  printf("\tRUN = 0, LPRUN = 1, STOP1 = 2\r\n");
	  printf("\tMode actuel: %d\r\n\r\n", currentMode);

	  while (button_pressed == 0){}

	  Switch_Mode();
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
  * text is displayed and LEDs are blinking.
  * Power mode is switched according to the button pressed and
  * `currentMode` is updated.
  * @param GPIO_Pin: Pin number that triggered the interrupt
  * @note  This function is called by the HAL when an external interrupt occurs.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	// Update our user defined flags
	button_pressed = 1;
	// Cases differ depending on the button pressed
	// Actions to do if we are currently in STOP1 mode
	if (currentMode == MODE_STOP1 || currentMode == MODE_LPRUN){
		if (currentMode == MODE_STOP1){
			// Resume the SysTick increment
			HAL_ResumeTick();
		}
		else if (currentMode == MODE_LPRUN){
			Exit_LowPowerRunMode(1);
		}
		// (Re)Initialize the peripherals
		HAL_Init(); 			// all peripherals, Flash interface and SysTick
		MX_GPIO_Init();			// GPIO
		MX_USART2_UART_Init(); 	// USART2
	}
	/* If user button 1 (UB1, on PA0) is pressed,
     * we are currently in STOP1 mode, so no change.
     */
	if (GPIO_Pin == UB1_Pin){
		nextMode = MODE_STOP1;
        // Nothing else to do here.
	}
	/* If user button 2 (UB2, on PA1) is pressed,
	 * switch mode to LPRun mode.
	 */
	if (GPIO_Pin == UB2_Pin){
		printf("Pression du Bouton 2 détectée.\r\n");
		printf("Le µCU entrera en mode LPRun.\r\n");
		nextMode = MODE_LPRUN;
	}
	/* If user button 3 (UB3, on PC6) is pressed,
	 * switch mode to Run mode.
	 */
	if (GPIO_Pin == UB3_Pin){
		printf("Pression du Bouton 3 détectée.\r\n");
		printf("Le µCU entrera en mode Run.\r\n");
		nextMode = MODE_RUN;
	}
}

/** @brief Makes the CPU change its power mode,
  * between the Run, LPRun and STOP1,
  * according to the button pressed.
  * @param GPIO_Pin may take different values, each leading to a specific mode:
  * 	GPIO_Pin_0 || UB1_Pin sets the CPU to STOP1 mode
  * 	GPIO_Pin_1 || UB2_Pin sets the CPU to LPRun mode
  * 	GPIO_Pin_6 || UB3_Pin sets the CPU to Run mode
  * @retval None
  */
void Switch_Mode(void){
	// Cases differ depending on the button pressed
	/* If user button 1 (UB1, on PA0) was pressed,
	 * switch mode to STOP1 mode.
	 */
	if (nextMode == MODE_STOP1){
		// Launch blue LED sequence (3 quick blinks)
		for (int _ = 0; _ < 6; _++){
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15);
			delay(50);
		}
		// If we are in LPRun or Run mode, switch to STOP1 mode
		if (currentMode != MODE_STOP1){
			// Procedure to enter STOP1 mode is the same, but exiting LPRun is an extra step
			if (currentMode == MODE_LPRUN){
				// Exit first from LPRun, working back at 16 MHz
				Exit_LowPowerRunMode(1);
				// Reset the USART2 peripheral, as HCLK1 frequency is now different
				MX_USART2_UART_Init();
			}
			printf("Le µCU entre en mode STOP1 via WFI.\r\n");
			// Suspend the SysTick increment
			HAL_SuspendTick();
			// Clock-gating the USART2 peripheral
			__HAL_RCC_USART2_CLK_SLEEP_DISABLE();
			// Set the current mode to STOP1
			currentMode = MODE_STOP1;
      // Enter STOP1 mode
      Enter_STOP1Mode_OnWFI();
		}
	}
	/* If user button 2 (UB2, on PA1) was pressed,
	 * switch mode to LPRun mode.
	 */
	else if (nextMode == MODE_LPRUN){
		// Launch green LED sequence (3 quick blinks)
		for (int _ = 0; _ < 6; _++){
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
			delay(50);
		}
		// If we are in Run or STOP1 mode, switch to LPRun mode
		if (currentMode != MODE_LPRUN){
			// Procedure to enter LPRun mode is the same, but exiting STOP1 requires extra steps
			if (currentMode == MODE_STOP1){
				// Resume the SysTick increment
				HAL_ResumeTick();
				// (Re)Initialize the peripherals
				HAL_Init(); 			// all peripherals, Flash interface and SysTick
				MX_GPIO_Init();			// GPIO
			}
			// Reset the USART2 peripheral
			MX_USART2_UART_Init();
			printf("Le µCU entre en mode LPRun.\r\n\r\n");
			// Set the current mode to LPRun
			currentMode = MODE_LPRUN;
			// Enter LPRun mode
			Enter_LowPowerRunMode();
			// Reset the USART2 peripheral (again)
			MX_USART2_UART_Init();
		}
	}
	/* If user button 3 (UB3, on PC6) was pressed,
	 * switch mode to Run mode.
	 */
	else if (nextMode == MODE_RUN){
		// Launch red LED sequence (2 slow blinks)
		for (int _ = 0; _ < 4; _++){
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_11);
			delay(200);
		}
		// If we are in LPRun or STOP1 mode, switch to Run mode
		if (currentMode != MODE_RUN){
			if (currentMode == MODE_LPRUN){
				// Exit first from LPRun, working back at 16 MHz
				Exit_LowPowerRunMode(1);
			} if (currentMode == MODE_STOP1){
				// Resume the SysTick increment
				HAL_ResumeTick();
			}
			// (Re)Initialize the peripherals
			HAL_Init(); 			// all peripherals, Flash interface and SysTick
			MX_GPIO_Init();			// GPIO
			MX_USART2_UART_Init(); 	// USART2
			printf("Le µCU entre en mode Run.\r\n\r\n");
			// Set the current mode to Run
			currentMode = MODE_RUN;
		}
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
