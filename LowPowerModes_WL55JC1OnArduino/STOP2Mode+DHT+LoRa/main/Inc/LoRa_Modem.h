/**
  ******************************************************************************
  * @file           : LoRa_Modem.h
  * @brief          : Header for LoRa_Modem.cpp file.
  *                   This file declares the necessary functions for the LoRaWAN
  *                   modem initialization and data sending.    
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LORA_MODEM_H
#define LORA_MODEM_H

#include <STM32LoRaWAN.h>
#include "My_Error_Handler.h"

// LoRaWAN Configuration
extern const char* SECRET_APP_EUI;
extern const char* SECRET_APP_KEY;

// LoRaWAN modem object
extern LoRaModem modem;

// Async LoRaWAN state machine for STOP2-safe operation
enum LoRaState {
  LORA_IDLE,
  LORA_JOINING,
  LORA_JOINED,
  LORA_SENDING,
  LORA_SENT,
  LORA_ERROR
};
extern LoRaState loraState;

// Functions
void setupLoRaWAN(); // (blocking, legacy)
void sendLoRaData(float* dataTable, size_t dataSize); // (blocking, legacy)

// Non-blocking/async functions for STOP2-safe operation:
// void tryLoRaJoinAsync();
// void tryLoRaSendAsync(const uint8_t* data, size_t len);

#endif // LORA_MODEM_H