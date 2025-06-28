/**
  ******************************************************************************
  * @file           : STOP2Mode.h
  * @brief          : Header for STOP2Mode.cpp file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STOP2MODE_H
#define __STOP2MODE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void My_Error_Handler(void);

/* USER CODE BEGIN EFP */
void System_Setup(void);
void STOP2_Entry_LEDSequence(void);
void STOP2_Exit_LEDSequence(void);

//
/* USER CODE END EFP *
/* Private defines -----------------------------------------------------------*/
#define RCC_OSC32_IN_Pin GPIO_PIN_14
#define RCC_OSC32_IN_GPIO_Port GPIOC
#define GLED_Pin GPIO_PIN_9
#define GLED_GPIO_Port GPIOB
#define RCC_OSC32_OUT_Pin GPIO_PIN_15
#define RCC_OSC32_OUT_GPIO_Port GPIOC
#define FE_CTRL3_Pin GPIO_PIN_3
#define FE_CTRL3_GPIO_Port GPIOC
#define FE_CTRL2_Pin GPIO_PIN_5
#define FE_CTRL2_GPIO_Port GPIOC
#define FE_CTRL1_Pin GPIO_PIN_4
#define FE_CTRL1_GPIO_Port GPIOC
#define RLED_Pin GPIO_PIN_11
#define RLED_GPIO_Port GPIOB
#define T_VCP_RX_Pin GPIO_PIN_3
#define T_VCP_RX_GPIO_Port GPIOA
#define T_VCP_RXA2_Pin GPIO_PIN_2
#define T_VCP_RXA2_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */
// RTC WakeUpCounter values corresponding to 5 seconds
#define FIVE_SECONDS_LSI 0x2710	// Supposing RTC clock source is LSI 32 kHz
#define FIVE_SECONDS_LSE 0x2800 // Supposing RTC clock source is LSE 32.768 kHz
#define SUBSECONDS_OFFSET 120   // Necessary subseconds offset to set up the RTC alarm precisely (experimentally determined)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#include "Radio.h"
extern STM32WLx_Module radio; // Radio module instance

void Enter_STOP2Mode_WithRTCAlarm(uint16_t delay_s);

#endif /* __STOP2MODE_H */
