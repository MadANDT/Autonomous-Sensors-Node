 /**
  ******************************************************************************
  * @file           : main.ino
  * @brief          : Main program body, enters STOP2 mode for a specified time -
                      oscillating between 3 (default value), 4 and 5 seconds and 
                      wakes up using RTC alarm.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"
#include "STOP2Mode.h"
#include "STOP2_SystemClock_Config.h"
#include "STOP2_MX_Config.h"
#include "DHT_Sensors.h"
#include "Radio.h"

volatile uint16_t SLEEP_DURATION = 10;
volatile bool sleep_increment = true; // if se to True, sleep duration increments, if False, it decrements

volatile bool transmittedFlag = false;
int transmissionState = RADIOLIB_ERR_NONE;

void setFlag(void) {
  transmittedFlag = true;
}


void setup() {

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  Serial.begin(9600);
  while(!Serial);

  Serial.println("System initialized\r\n");

  RTC_Setup(SLEEP_DURATION);

  setupSensors();  

  Serial.println("DHT sensors initialized\r\n");

  radio.setRfSwitchTable(rfswitch_pins, rfswitch_table);
  int state = radio.begin(868.0); // Corresponds to Europe 868 MHz bandwitdh
  radio.setTCXO(1.7); // for Nucleo WL55JC1

  Serial.println("RTC alarm, LoRa modem and DHT sensors initialized\r\n");
}

void loop(){
  Serial.println("Loop start / Wake-up");
  //Read DHT sensors
  float t_left, h_left, t_middle, h_middle, t_right, h_right;
  readDHTSensors(t_left, h_left, t_middle, h_middle, t_right, h_right);

  // Prepare data table (local, stack-allocated, minimal memory)
  float dataTable[] = { t_left, h_left, t_middle, h_middle, t_right, h_right, SLEEP_DURATION };
  size_t dataSize = sizeof(dataTable) / sizeof(dataTable[0]);
  uint8_t txData[sizeof(float) * dataSize];

  // Send data via LoRa
  sendLoRaData(dataTable, dataSize);

  Serial.print("Left Sensor - Temp.: "); Serial.print(t_left); Serial.print("°C, Humidity: "); Serial.print(h_left); Serial.println("%");
  Serial.print("Middle Sensor - Temp.: "); Serial.print(t_middle); Serial.print("°C, Humidity: "); Serial.print(h_middle); Serial.println("%");
  Serial.print("Right Sensor - Temp.: "); Serial.print(t_right); Serial.print("°C, Humidity: "); Serial.print(h_right); Serial.println("%");


  Serial.print("Current sleep duration required: "); Serial.print(SLEEP_DURATION); Serial.print(" seconds");
  uint16_t wakeup_ticks = (uint16_t)(SLEEP_DURATION * 2048);
  Serial.print(" → "); Serial.print(wakeup_ticks); Serial.println(" wakeup_ticks\r\n");

   
  Serial.println("Entering STOP2 mode");
   
  Enter_STOP2Mode_WithRTCAlarm(SLEEP_DURATION);

  // Modify sleep duration for next iteration
  sleep_increment = (SLEEP_DURATION == 10) ? true : ((SLEEP_DURATION == 12) ? false : sleep_increment);
  SLEEP_DURATION = (sleep_increment) ? ++SLEEP_DURATION : --SLEEP_DURATION;
}
