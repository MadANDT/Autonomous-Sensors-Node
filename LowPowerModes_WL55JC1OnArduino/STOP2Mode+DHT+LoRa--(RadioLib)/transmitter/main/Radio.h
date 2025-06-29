/**
  ******************************************************************************
  * @file           : Radio.h
  * @brief          : Header for Radio.cpp file.
  *                   This file provides code for the radio sub-GHz module  
  *                   initialization and data sending, via LoRa.    
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef RADIO_H
#define RADIO_H

#include <RadioLib.h>
#include "My_Error_Handler.h"

// Functions
int sendLoRaData(float* dataTable, size_t dataSize);

extern STM32WLx radio;
extern volatile bool transmittedFlag;

#endif // RADIO_H