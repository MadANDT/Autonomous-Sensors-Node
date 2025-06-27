/**
  ******************************************************************************
  * @file           : LoRa_Modem.cpp
  * @brief          : This file provides code for the LoRaWAN modem 
  *                   initialization and data sending. 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Inc/LoRa_Modem.h"
#include "Inc/arduino_secrets.h"
#include "Inc/STOP2Mode.h"

// LoRaWAN modem object
LoRaModem modem;

LoRaState loraState = LORA_IDLE;
unsigned long loraActionStart = 0;
const unsigned long LORA_JOIN_TIMEOUT = 20000; // 20s
const unsigned long LORA_SEND_TIMEOUT = 10000; // 10s

/**
  * @brief Setup function for LoRaWAN modem. Initializes 
  *        the LoRaWAN modem and attempts to join the network 
  *        using OTAA (Over-The-Air Activation).
  * @note This function must be called in `setup`, before sending any data.
  * @param None
  * @retval None
  */
void setupLoRaWAN() {
  if (!modem.begin(EU868)) {
    Serial.println("LoRaWAN modem initialization failed!");
    My_Error_Handler();
  }

  bool connected = modem.joinOTAA(SECRET_APP_EUI, SECRET_APP_KEY, SECRET_DEV_EUI);
  if (!connected) {
    Serial.println("OTAA join failed!");
    while (1);
  }
  Serial.println("LoRaWAN connected!");
}

/**
  * @brief Sends data via LoRaWAN.
  * @param dataTable Pointer to the data table containing sensor readings.
  * @param dataSize Size of the data table.
  * @note This function sends the data in a specific format.
  */
void sendLoRaData(float* dataTable, size_t dataSize){ 
  // Prepare data for transmission
  uint8_t txData[sizeof(float) * dataSize];
  for (size_t i = 0; i < dataSize; i++) {
    memcpy(&txData[i * sizeof(float)], &dataTable[i], sizeof(float));
  }
  
  if (!modem.connected()) {
  // Re-join or re-initialize here
    modem.begin(EU868);   // if needed
    modem.joinOTAA(SECRET_APP_EUI, SECRET_APP_KEY, SECRET_DEV_EUI); 
  }

  modem.beginPacket();

  modem.write(txData, sizeof(txData));
  while (modem.busy()) {
  delay(10);
  }
  Serial.println("About to call endPacket");
int err = modem.endPacket(true);
Serial.println("endPacket returned");

  Serial.println(err > 0 ? "Data sent!" : "Send failed");
}


// void tryLoRaJoinAsync() {
//   if (loraState == LORA_IDLE) {
//     if (!modem.connected()) {
//       modem.begin(EU868);
//       if (modem.joinOTAAAsync(/*SECRET_APP_EUI, SECRET_APP_KEY, SECRET_DEV_EUI*/)) {
//         loraState = LORA_JOINING;
//         loraActionStart = millis();
//         Serial.println("Started async join...");
//       } else {
//         loraState = LORA_ERROR;
//         Serial.println("Failed to start async join");
//       }
//     } else {
//       loraState = LORA_JOINED;
//       Serial.println("Already joined");
//     }
//   }

//   if (loraState == LORA_JOINING) {
//     modem.maintain(); // Progress join
//     if (!modem.busy()) {
//       if (modem.connected()) {
//         loraState = LORA_JOINED;
//         Serial.println("Join successful!");
//       } else {
//         loraState = LORA_ERROR;
//         Serial.println("Join failed!");
//       }
//     } else if (millis() - loraActionStart > LORA_JOIN_TIMEOUT) {
//       loraState = LORA_ERROR;
//       Serial.println("Join timeout!");
//     }
//   }
// }

// void tryLoRaSendAsync(const uint8_t* data, size_t len) {
//   if (loraState == LORA_JOINED) {
//     modem.beginPacket(); // No if, just call it
//     modem.write(data, len);
//     if (modem.endPacketAsync(true) > 0) {
//       loraState = LORA_SENDING;
//       loraActionStart = millis();
//       Serial.println("Started async send...");
//     } else {
//       loraState = LORA_ERROR;
//       Serial.println("Failed to queue packet");
//     }
//   }

//   if (loraState == LORA_SENDING) {
//     modem.maintain(); // Progress send
//     if (!modem.busy()) {
//       loraState = LORA_SENT;
//       Serial.println("Send complete!");
//     } else if (millis() - loraActionStart > LORA_SEND_TIMEOUT) {
//       loraState = LORA_ERROR;
//       Serial.println("Send timeout!");
//     }
//   }
// }