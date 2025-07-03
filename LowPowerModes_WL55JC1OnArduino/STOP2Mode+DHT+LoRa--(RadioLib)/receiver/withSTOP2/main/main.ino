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
volatile bool wakeFromRTC = false;
volatile bool wakeFromRadio = false;
float receivedData[7];  // Stores t_left, h_left, t_middle, h_middle, t_right, h_right, sleep_duration

// Timing parameters, ⚠️ Receiver must avoid listening when Transmitter is sleeping !
const uint16_t LISTEN_INTERVAL = 10;  // Wake up every 4 seconds to listen
const uint16_t LISTEN_DURATION = 3;  // Listen for 10 second

// ISR for packet reception
void setFlag(void) {
  receivedFlag = true;
  wakeFromRadio = true;
}

void setup() {
  // System initialization
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  // Serial initialization
  Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("(#R) System initialized\r\n");

  // RTC peripheral initialization
  RTC_Setup(LISTEN_INTERVAL);

  // Configure radio
  radio.setRfSwitchTable(rfswitch_pins, rfswitch_table);

  Serial.print("[STM32WL] Initializing... ");
  
  if (radio.begin(EU868_FREQUENCY) != RADIOLIB_ERR_NONE) {
    Serial.println("FAILED!");
    while (true)
      ;
  }
  // Match transmitter parameters
  radio.setSpreadingFactor(SF_LONG_RANGE);
  radio.setBandwidth(BW_LONG_RANGE);
  radio.setCodingRate(CR_HIGH);
  radio.setSyncWord(SYNC_WORD);
  radio.setOutputPower(OP_MAX_RANGE);
  radio.setTCXO(TCXO_VOLTAGE);

  Serial.println("SUCCESS!");
  radio.setTCXO(TCXO_VOLTAGE);

  // Configure interrupt
  radio.setDio1Action(setFlag);
  radio.setPacketReceivedAction(setFlag);
  // radio.standby();

  Serial.println("Listening for LoRa packets...");
  radio.startReceive();
}

void loop() {
  // Start listening for packets before STOP2
  radio.startReceive();

  // Enter STOP2 mode (can wake on RTC alarm or radio interrupt)
  Enter_STOP2Mode_WithRTCAlarm(LISTEN_INTERVAL, STOP2_MODE_RECEIVER);
  // Determine wake-up source and handle accordingly
  if (wakeFromRadio) {
    wakeFromRadio = false;
    handleRadioPacket();
  }
  else if (wakeFromRTC) {
    wakeFromRTC = false;
    handlePeriodicListen();
  }
}

void handleRadioPacket() {
  // Packet received while in STOP2 mode
  Serial.println("Woke on LoRa packet reception!");
  
  // Read received data
  uint8_t rxBuffer[28];  // 7 floats * 4 bytes
  int state = radio.readData(rxBuffer, 28);

  if (state == RADIOLIB_ERR_NONE) {
    memcpy(receivedData, rxBuffer, sizeof(receivedData));
    processReceivedData();
  } else {
    Serial.print("Packet receive failed, code: ");
    Serial.println(state);
  }

  // Restart listening
  radio.startReceive();
}

void handlePeriodicListen() {
  // Woke from RTC alarm (periodic listen)
  Serial.println("Woke on RTC alarm - Listening for LoRa packets");
  
  uint32_t listenStart = millis();
  bool packetReceived = false;

  // Active listening for specified duration
  while ((millis() - listenStart) < (LISTEN_DURATION * 1000)) {
    if (receivedFlag) {
      receivedFlag = false;
      packetReceived = true;
      
      uint8_t rxBuffer[28];
      int state = radio.readData(rxBuffer, 28);

      if (state == RADIOLIB_ERR_NONE) {
        memcpy(receivedData, rxBuffer, sizeof(receivedData));
        processReceivedData();
        break;  // Exit early if packet received
      }
    }
  }

  if (!packetReceived) {
    Serial.println("No packets received during listen window");
  }

  // Return to standby and prepare for STOP2
  radio.standby();
}


void processReceivedData() {
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
}