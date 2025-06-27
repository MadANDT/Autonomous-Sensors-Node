/**
  ******************************************************************************
  * @file           : Radio.cpp
  * @brief          : This file provides code for the radio sub-GHz module  
  *                   initialization and data sending, via LoRa. 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Radio.h"
#include "STOP2Mode.h"

STM32WLx radio = new STM32WLx_Module();
static const uint32_t rfswitch_pins[] = {PC3, PC4, PC5, RADIOLIB_NC, RADIOLIB_NC};
static const Module::RfSwitchMode_t rfswitch_table[] = {
  {STM32WLx::MODE_IDLE,  {LOW,  LOW,  LOW}},
  {STM32WLx::MODE_RX,    {HIGH, HIGH, LOW}},
  {STM32WLx::MODE_TX_LP, {HIGH, HIGH, HIGH}},
  {STM32WLx::MODE_TX_HP, {HIGH, LOW,  HIGH}},
  END_OF_MODE_TABLE,
};

/**
  * @brief Sends data via LoRaWAN.
  * @param dataTable Pointer to the data table containing sensor readings.
  * @param dataSize Size of the data table.
  * @note This function sends the data in a specific format.
  */
int sendLoRaData(float* dataTable, size_t dataSize){ 
  // Convert your dataTable to a byte array as needed
    uint8_t* txData = reinterpret_cast<uint8_t*>(dataTable);
    size_t dataLen = dataSize * sizeof(float);

    int state = radio.transmit(txData, dataLen);
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println("LoRa transmission successful!");
    } else {
        Serial.print("LoRa transmission failed, code: ");
        Serial.println(state);
    }
    radio.finishTransmit();
    return state;
}
