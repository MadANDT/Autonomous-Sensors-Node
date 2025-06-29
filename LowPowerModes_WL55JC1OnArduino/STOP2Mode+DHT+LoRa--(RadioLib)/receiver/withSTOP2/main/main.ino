/**
   ******************************************************************************
   * @file           : main.ino
   * @brief          : Main program body.
   *                   The setup initializes the system : clock, pins, radio (as 
   *                   receiver), RTC alarm and sensors.  
   *                   The main loop consists of waiting for data sent by LoRa 
   *                   using the radio, entering STOP2 mode and waking up on RTC 
   *                   internal wake-up interrupt. 
   *                   When data are intercepted, they are stored in memory. 
   *                   Optionnaly, they may be sent on the LoRaWAN network towards
   *                   an online stack (e.g. The Things Network Stack). 
   ******************************************************************************
   */

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"
#include "STOP2Mode.h"
#include "STOP2_SystemClock_Config.h"
#include "STOP2_MX_Config.h"
#include "Radio.h"
#include <RadioLib.h>
#include "STM32WL_TxRx_RadioParameters.h"

// RF switch config for Nucleo WL55JC1
static const uint32_t rfswitch_pins[] = { PC3, PC4, PC5, RADIOLIB_NC, RADIOLIB_NC };
static const Module::RfSwitchMode_t rfswitch_table[] = {
  { STM32WLx::MODE_IDLE, { LOW, LOW, LOW } },
  { STM32WLx::MODE_RX, { HIGH, HIGH, LOW } },
  { STM32WLx::MODE_TX_LP, { HIGH, HIGH, HIGH } },
  { STM32WLx::MODE_TX_HP, { HIGH, LOW, HIGH } },
  END_OF_MODE_TABLE,
};

// Transmition/Reception flag and data storage
volatile bool transmittedFlag = false;
volatile bool receivedFlag = false;
float receivedData[7];  // Stores t_left, h_left, t_middle, h_middle, t_right, h_right, sleep_duration

// Timing parameters
const uint16_t LISTEN_INTERVAL = 8;  // Wake up every 8 seconds to listen
const uint16_t LISTEN_DURATION = 1;  // Listen for 1 second

// ISR for packet reception
void setFlag(void) {
  receivedFlag = true;
}

void setup() {

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("(#R) System initialized\r\n");

  RTC_Setup(LISTEN_INTERVAL);

  // Configure radio
  radio.setRfSwitchTable(rfswitch_pins, rfswitch_table);

  Serial.print("[STM32WL] Initializing... ");
  if (radio.begin(EU868_FREQUENCY) != RADIOLIB_ERR_NONE) {
    Serial.println("FAILED!");
    while (true)
      ;
  }
  Serial.println("SUCCESS!");

  // Match transmitter parameters
  radio.setSpreadingFactor(SPREADING_FACTOR);
  radio.setBandwidth(BANDWIDTH);
  radio.setCodingRate(CODING_RATE);
  radio.setSyncWord(SYNC_WORD);
  radio.setOutputPower(OUTPUT_POWER);
  radio.setTCXO(TCXO_VOLTAGE);

  // Configure interrupt
  radio.setDio1Action(setFlag);
  radio.standby();

  Serial.println("Listening for LoRa packets...");
}

void loop() {
  // Enter STOP2 mode (wakes on RTC alarm)
  Enter_STOP2Mode_WithRTCAlarm(LISTEN_INTERVAL, STOP2_MODE_RECEIVER);

  // After wakeup from RTC alarm
  Serial.println("Woke on RTC alarm - Starting listen");

  // Start listening for packets
  radio.startReceive();
  uint32_t listenStart = millis();

  // Listen for specified duration
  while ((millis() - listenStart) < (LISTEN_DURATION * 1000)) {
    if (receivedFlag) {
      receivedFlag = false;

      // Read received data
      uint8_t rxBuffer[28];  // 7 floats * 4 bytes
      int state = radio.readData(rxBuffer, 28);

      if (state == RADIOLIB_ERR_NONE) {
        memcpy(receivedData, rxBuffer, sizeof(receivedData));

        // Process received data
        Serial.println("\nReceived Sensor Data:");
        Serial.print("Left: ");
        Serial.print(receivedData[0]);
        Serial.print("°C, ");
        Serial.print(receivedData[1]);
        Serial.println("% RH");
        Serial.print("Middle: ");
        Serial.print(receivedData[2]);
        Serial.print("°C, ");
        Serial.print(receivedData[3]);
        Serial.println("% RH");
        Serial.print("Right: ");
        Serial.print(receivedData[4]);
        Serial.print("°C, ");
        Serial.print(receivedData[5]);
        Serial.println("% RH");
        Serial.print("Sleep Duration: ");
        Serial.print(receivedData[6]);
        Serial.println("s");

        // Exit listen loop early since we got data
        break;
      }
    }
  }

  // Return radio to standby
  radio.standby();
  Serial.println("Listen period ended - Returning to STOP2");
}