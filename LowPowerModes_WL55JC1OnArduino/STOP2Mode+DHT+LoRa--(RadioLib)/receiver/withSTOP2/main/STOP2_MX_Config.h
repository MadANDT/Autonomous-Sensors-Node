/**
  ******************************************************************************
  * @file           : STOP2_MX_Config.h
  * @brief          : Header for STOP2_MX_Config.cpp file.
  *                   This file declares the necessary functions for the peripherals 
  *                   configuration (GPIO here) made by CubeMX.
  ******************************************************************************
  */

#pragma once

/* Define to prevent recursive inclusion -------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"

void MX_GPIO_Init(void);

#ifdef __cplusplus
}
#endif

#include <STM32RTC.h>
/* RTC related variables -----------------------------------------------------*/
extern STM32RTC& rtc; // = STM32RTC::getInstance();
extern volatile bool rtcAlarmFlag; // = false;
extern volatile bool wakeFromRTC;

void RTCAlarmCallback(void *data);
void RTC_Setup(uint16_t delay_s);


