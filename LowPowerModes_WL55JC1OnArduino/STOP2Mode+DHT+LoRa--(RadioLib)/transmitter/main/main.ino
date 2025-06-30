/**
  ******************************************************************************
  * @file           : main.ino
  * @brief          : Main program body.
  *                   The setup initializes the system : clock, pins, radio (as 
  *                   transmitter), RTC alarm and sensors.  
  *                   The main loop consists of reading data sensors, sending 
  *                   them via the LoRa Radio, entering STOP2 mode and waking up 
  *                   on RTC internal wake-up interrupt.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"
#include "STOP2Mode.h"
#include "STOP2_SystemClock_Config.h"
#include "STOP2_MX_Config.h"
#include "DHT_Sensors.h"
#include "Radio.h"
#include <RadioLib.h>
#include "STM32WL_TxRx_RadioParameters.h"

volatile uint16_t SLEEP_DURATION = 5;
volatile bool sleep_increment = true; // if se to True, sleep duration increments, if False, it decrements

volatile bool transmittedFlag = false;
int transmissionState = RADIOLIB_ERR_NONE;

static const uint32_t rfswitch_pins[] = {PC3, PC4, PC5, RADIOLIB_NC, RADIOLIB_NC};
static const Module::RfSwitchMode_t rfswitch_table[] = {
  {STM32WLx::MODE_IDLE,  {LOW,  LOW,  LOW}},
  {STM32WLx::MODE_RX,    {HIGH, HIGH, LOW}},
  {STM32WLx::MODE_TX_LP, {HIGH, HIGH, HIGH}},
  {STM32WLx::MODE_TX_HP, {HIGH, LOW,  HIGH}},
  END_OF_MODE_TABLE,
};

void setFlag(void) {
  transmittedFlag = true;
}

void setup() {

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  Serial.begin(9600);
  while(!Serial);

  // Serial.println("(#T) System initialized\r\n");

  RTC_Setup(SLEEP_DURATION);

  setupSensors();  

  // Serial.println("(#T) DHT sensors initialized\r\n");

  radio.setRfSwitchTable(rfswitch_pins, rfswitch_table);
  int state = radio.begin(EU868_FREQUENCY);
  if (state == RADIOLIB_ERR_NONE) {
  // Parameters must match with receiver
    radio.setSpreadingFactor(SF_LONG_RANGE);  // Spreading factor 12
    radio.setBandwidth(BW_LONG_RANGE);        // 125 kHz bandwidth
    radio.setCodingRate(CR_HIGH);             // Coding rate 4/8
    radio.setSyncWord(SYNC_WORD);             // Private network sync word
    radio.setOutputPower(OP_MAX_RANGE);       // 20 dBm output power
  }
  radio.setTCXO(TCXO_VOLTAGE); // for Nucleo WL55JC1
  // Serial.println("(#T) RTC alarm, LoRa modem and DHT sensors initialized\r\n");
}

void loop(){
  // Serial.println("(#T) Loop start / Wake-up");
  //Read DHT sensors
  float t_left, h_left, t_middle, h_middle, t_right, h_right;
  readDHTSensors(t_left, h_left, t_middle, h_middle, t_right, h_right);

  // Prepare data table (local, stack-allocated, minimal memory)
  float dataTable[] = { t_left, h_left, t_middle, h_middle, t_right, h_right, SLEEP_DURATION };
  size_t dataSize = sizeof(dataTable) / sizeof(dataTable[0]);
  uint8_t txData[sizeof(float) * dataSize];

  // Send data via LoRa
  sendLoRaData(dataTable, dataSize);

  // Serial.print("Left Sensor - Temp.: "); Serial.print(t_left); Serial.print("°C, Humidity: "); Serial.print(h_left); Serial.println("%");
  // Serial.print("Middle Sensor - Temp.: "); Serial.print(t_middle); Serial.print("°C, Humidity: "); Serial.print(h_middle); Serial.println("%");
  // Serial.print("Right Sensor - Temp.: "); Serial.print(t_right); Serial.print("°C, Humidity: "); Serial.print(h_right); Serial.println("%");

  // Serial.print("Current sleep duration required: "); Serial.print(SLEEP_DURATION); Serial.print(" seconds");
  // uint16_t wakeup_ticks = (uint16_t)(SLEEP_DURATION * 2048);
  // Serial.print(" → "); Serial.print(wakeup_ticks); Serial.println(" wakeup_ticks\r\n");
   
  // Serial.println("(#T) Entering STOP2 mode");
   
  Enter_STOP2Mode_WithRTCAlarm(SLEEP_DURATION, STOP2_MODE_TRANSMITTER);

  // Modify sleep duration for next iteration
  sleep_increment = (SLEEP_DURATION == 5) ? true : ((SLEEP_DURATION == 7) ? false : sleep_increment);
  SLEEP_DURATION = (sleep_increment) ? ++SLEEP_DURATION : --SLEEP_DURATION;
}
