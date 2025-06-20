/**
  ******************************************************************************
  * @file           : STOP2_SystemClock_Config.h
  * @brief          : Header for STOP2_SystemClock_Config.cpp file.
  *                   This file declares the necessary functions for the system 
  *                   clock configuration.
  ******************************************************************************
  */

#pragma once

/* Define to prevent recursive inclusion -------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

void SystemClock_Config(void);
void My_Error_Handler(void);

#ifdef __cplusplus
}
#endif