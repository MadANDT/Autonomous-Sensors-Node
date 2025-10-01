/**
  ******************************************************************************
  * @file           : Standby_MX_Config.h
  * @brief          : Header for Standby_MX_Config.cpp file.
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
