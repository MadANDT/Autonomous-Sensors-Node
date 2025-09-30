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

volatile uint16_t SLEEP_DURATION = 3;
volatile bool sleep_increment = true; // if se to True, sleep duration increments, if False, it decrements

/**
  * @brief RTC interrupt callback function, updates RTC alarm flag by rising it.
  * @note This function is called after RTC fires its itnerrupt.
  * @param data: data that may have been stored in RTC, unused by default.
  * @retval None
 */
void RTCAlarmCallback(void *data) {
  UNUSED(data);
  rtcAlarmFlag = true;
}

/**
  * @brief Enter STOP2 mode for DELAY_S seconds,
  * wake up on RTC alarm interrupt.
  * @param delay_s: sleep duration offset in seconds
  * @retval None
 */
 void Enter_STOP2Mode_WithRTCAlarm(uint16_t delay_s){
  STOP2_Entry_LEDSequence();  // Indicate the entry in STOP2 mode with the LED sequence

  rtcAlarmFlag = false;

  RTC_Setup(delay_s);  

  HAL_SuspendTick();          // Suspend the SysTick Increment

  HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
  /* ####----####----####----####
	 * MCU is asleep in STOP2 mode
	 * ####----####----####----####
	 */

  HAL_ResumeTick();           // Resume the SysTick Increment

  STOP2_Exit_LEDSequence();   // Indicate the exit in STOP2 mode with the LED sequence

  SystemClock_Config();

  // Wait for alarm flag (should be set by callback)
  while (!rtcAlarmFlag) {
      // Optionally, sleep or do nothing
  }

  // De-initialize the RTC
  rtc.end();
}

void setup() {

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  Serial.begin(9600);
  while(!Serial);

  Serial.println("System initialized\r\n");

  RTC_Setup(SLEEP_DURATION);

  setupSensors();  // Initialize DHT sensors if needed
  Serial.println("RTC alarm and DHT sensors initialized\r\n");
}

void loop(){
  Serial.println("Loop start / Wake-up");
  //Read DHT sensors
  float t_left, h_left, t_middle, h_middle, t_right, h_right;
  readDHTSensors(t_left, h_left, t_middle, h_middle, t_right, h_right);

  Serial.print("Left Sensor - Temp.: "); Serial.print(t_left); Serial.print("°C, Humidity: "); Serial.print(h_left); Serial.println("%");
  Serial.print("Middle Sensor - Temp.: "); Serial.print(t_middle); Serial.print("°C, Humidity: "); Serial.print(h_middle); Serial.println("%");
  Serial.print("Right Sensor - Temp.: "); Serial.print(t_right); Serial.print("°C, Humidity: "); Serial.print(h_right); Serial.println("%");

  Serial.print("Current sleep duration required: "); Serial.print(SLEEP_DURATION); Serial.print(" seconds");
  uint16_t wakeup_ticks = (uint16_t)(SLEEP_DURATION * 2048);
  Serial.print(" → "); Serial.print(wakeup_ticks); Serial.println(" wakeup_ticks\r\n");

   
  Serial.println("Entering STOP2 mode");
  Enter_STOP2Mode_WithRTCAlarm(SLEEP_DURATION);
  
  // Modify sleep duration for next iteration
  sleep_increment = (SLEEP_DURATION == 3) ? true : ((SLEEP_DURATION == 5) ? false : sleep_increment);
  SLEEP_DURATION = (sleep_increment) ? ++SLEEP_DURATION : --SLEEP_DURATION;
}
