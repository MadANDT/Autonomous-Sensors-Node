/**
  ******************************************************************************
  * @file           : STOP2_MX_Config.h
  * @brief          : Header for STOP2_MX_Config.cpp file.
  *                   This file declares the necessary functions for the peripherals 
  *                   configuration (RTC, GPIO here) made by CubeMX.
  ******************************************************************************
  */

#pragma once

/* Define to prevent recursive inclusion -------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"

//extern RTC_HandleTypeDef hrtc;

void MX_GPIO_Init(void);
void MX_RTC_Init(void);

#ifdef __cplusplus
}
#endif
