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

/* Define of parameters -------------------------------------*/
#define EU868_FREQUENCY     868.0   // Frequency in MHz for EU868 band
#define SPREADING_FACTOR    7       // Spreading factor (SF) for LoRa modulation
#define BANDWIDTH           125.0   // Bandwidth in kHz for LoRa modulation
#define CODING_RATE         7       // Coding rate for LoRa modulation (CR = 4/7)
#define SYNC_WORD           0x12    // Synchronization word for LoRa communication
#define OUTPUT_POWER        10      // Output power in dBm for transmission
#define TCXO_VOLTAGE        1.7     // TCXO voltage in volts (for Nucleo WL55JC1)