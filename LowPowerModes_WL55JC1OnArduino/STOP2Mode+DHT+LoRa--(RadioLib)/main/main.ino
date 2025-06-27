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

/* STM32WLx radio = new STM32WLx_Module();
static const uint32_t rfswitch_pins[] = {PC3, PC4, PC5, RADIOLIB_NC, RADIOLIB_NC};
static const Module::RfSwitchMode_t rfswitch_table[] = {
  {STM32WLx::MODE_IDLE,  {LOW,  LOW,  LOW}},
  {STM32WLx::MODE_RX,    {HIGH, HIGH, LOW}},
  {STM32WLx::MODE_TX_LP, {HIGH, HIGH, HIGH}},
  {STM32WLx::MODE_TX_HP, {HIGH, LOW,  HIGH}},
  END_OF_MODE_TABLE,
}; */

volatile bool transmittedFlag = false;
int transmissionState = RADIOLIB_ERR_NONE;

void setFlag(void) {
  transmittedFlag = true;
}

/**
  * @brief RTC interrupt callback function, updates RTC alarm flag by rising it.
  * @note This function is called after RTC fires its itnerrupt.
  * @param data: data that may have been stored in RTC, unused by default.
  * @retval None
 */
/*void RTCAlarmCallback(void *data) {
  UNUSED(data);
  rtcAlarmFlag = true;
}*/

/**
  * @brief Enter STOP2 mode for DELAY_S seconds,
  * wake up on RTC alarm interrupt.
  * @param delay_s: sleep duration offset in seconds
  * @retval None
 */
/* void Enter_STOP2Mode_WithRTCAlarm(uint16_t delay_s){
  STOP2_Entry_LEDSequence();  // Indicate the entry in STOP2 mode with the LED sequence
  rtcAlarmFlag = false;
  
  // Put radio in sleep mode while retaining configuration to minimize current 
  radio.sleep(true);

  RTC_Setup(delay_s);  
  
  HAL_SuspendTick();          // Suspend the SysTick Increment

  HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
  /* ####----####----####----####
	 * MCU is asleep in STOP2 mode
	 * ####----####----####----####
	 */

/*  HAL_ResumeTick();           // Resume the SysTick Increment

  // Wake radio up (standby) after STOP2
  radio.standby();

  STOP2_Exit_LEDSequence();   // Indicate the exit in STOP2 mode with the LED sequence

  // Wait for alarm flag (should be set by callback)
  while (!rtcAlarmFlag) {
      // Optionally, sleep or do nothing
  }

  // After wake-up
  transmittedFlag = false;
  radio.finishTransmit();
  // De-initialize the RTC
  rtc.end();

  // Reset system part
  SystemClock_Config();
}*/

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
  int state = radio.begin(868.0); // or your frequency
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
