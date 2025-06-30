/**
  ******************************************************************************
  * @file           : STM32WL_TxRx_RadioParameters.h
  * @brief          : Header for STM32WL radio parameters.
  *                   This file contains the definitions of the parameters used
  *                   to configure the STM32WL radio module for LoRa communication.
  *                   It includes : frequency (MHz), bandwidth (kHz), spreading factor, coding rate, 
  *                   synchronisation word and output power (dBm).    
  *                   Both receiver and transmitter need to match these 
  *                   parameters for successful communication.
  ******************************************************************************
  */

#include <RadioLib.h>

/* Defines of parameters for LoRa modulation ---------------------------------*/
#define EU868_FREQUENCY       868.0                           // Frequency in MHz for EU868 band
//// Spreading factor (SF) 
/**/ #define SF_MEDIUM_RANGE  7
/**/ #define SF_LONG_RANGE    12
//// Bandwidth (BW)
/**/ #define BW_SHORT_RANGE   RADIOLIB_SX126X_LORA_BW_500_0   // 500.0 kHz, slower data rate.
/**/ #define BW_LONG_RANGE    RADIOLIB_SX126X_LORA_BW_125_0   // 125.0 kHz, sweet spot for range vs. speed.
//// Coding rate (CR) 
/**/ #define CR_HIGH          RADIOLIB_SX126X_LORA_CR_4_8     // 4/8, more reliable at long range, in noisy environments
/**/ #define CR_LOW           RADIOLIB_SX126X_LORA_CR_4_5     // 4/5, for short range, high-speed links
//// Output power (OP)
/**/ #define OP_MAX_RANGE     20                              // 20-22 dBm : maximum range
/**/ #define OP_BAT_OPT       10                              // 10-14 dBm : battery optimization
//// Synchronisation word, for LoRa communication 
#define SYNC_WORD             0x12    
//// TCXO voltage in volts (for Nucleo WL55JC1)
#define TCXO_VOLTAGE          1.7