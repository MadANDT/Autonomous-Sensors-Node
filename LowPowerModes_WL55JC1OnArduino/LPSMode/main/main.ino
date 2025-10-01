/**
  ******************************************************************************
  * @file           : main.ino
  * @brief          : Main program body, enters LPS mode for a specified time
                      and wakes up using RTC alarm set (every 7 seconds by default).
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"
#include "LPSMode.h"
#include "LPS_SystemClock_Config.h"
#include "LPS_MX_Config.h"
#include <STM32RTC.h>

/* RTC related variables -----------------------------------------------------*/
STM32RTC& rtc = STM32RTC::getInstance();
volatile bool rtcAlarmFlag = false;
volatile uint16_t SLEEP_DURATION = 7;
/**
  * @brief RTC interrupt callback function, updates RTC alarm flag by rising it.
  * @note This function is called after RTC fires its interrupt.
  * @param data: data that may have been stored in RTC, unused by default.
  * @retval None
 */
void RTCAlarmCallback(void *data) {
  UNUSED(data);
  rtcAlarmFlag = true;
}

/**
  * @brief Sets up the RTC through STM32duino RTC library API and functions, 
  * and the initial sleep duration DEALY_S.
  * @note This function should be called in the setup loop.
  * @param delay_s: sleep duration offset in seconds for the first set up.
  * @retval None
 */
void RTC_setup(uint16_t delay_s){
  rtc.begin();
  rtc.attachInterrupt(RTCAlarmCallback);
  // Set initial time and date (optional, for demo)
  rtc.setTime(0, 0, 0);
  rtc.setDate(1, 1, 25);

  uint8_t hours = rtc.getHours();
  uint8_t minutes = rtc.getMinutes();
  uint8_t seconds = rtc.getSeconds();
  uint32_t subSeconds = rtc.getSubSeconds();

  uint32_t newSubSeconds = subSeconds + SUBSECONDS_OFFSET;
  uint8_t newSeconds = seconds + delay_s;

  if (newSubSeconds >= 1000) { // assuming 1000 ms per second
    newSubSeconds -= 1000;
    newSeconds += 1;
  }
  if (newSeconds >= 60) {
    newSeconds -= 60;
    minutes += 1;
    if (minutes >= 60) {
        minutes = 0;
        hours = (hours + 1) % 24;
    }
  }
  rtc.setAlarmTime(hours, minutes, newSeconds, newSubSeconds);
  rtc.enableAlarm(rtc.MATCH_HHMMSS);
}

/**
  * @brief Enter LPS mode for DELAY_S seconds,
  * wake up on RTC alarm interrupt.
  * @param delay_s: sleep duration offset in seconds
  * @retval None
 */
 void Enter_LPSMode_WithRTCAlarm(uint16_t delay_s){
  // Get current time
  uint8_t hours = rtc.getHours();
  uint8_t minutes = rtc.getMinutes();
  uint8_t seconds = rtc.getSeconds();
  uint32_t subSeconds = rtc.getSubSeconds();

  // Calculate new alarm time
  // Subseconds offset to consider the time between setting the alarm and actually enter the low-power mode
  uint32_t newSubSeconds = subSeconds + SUBSECONDS_OFFSET;
  uint8_t newSeconds = seconds + delay_s;

  if (newSubSeconds >= 1000) { // assuming 1000 ms per second
      newSubSeconds -= 1000;
      newSeconds += 1;
  }
  if (newSeconds >= 60) {
      newSeconds -= 60;
      minutes += 1;
      if (minutes >= 60) {
          minutes = 0;
          hours = (hours + 1) % 24;
      }
  }

  rtc.setAlarmTime(hours, minutes, newSeconds, newSubSeconds);
  rtc.enableAlarm(rtc.MATCH_HHMMSS);

  Enter_LowPowerSleepMode();  // Enter LPS mode - SysTick has been suspended! 

  rtcAlarmFlag = false;

  /* ####----####----####----####
	 * MCU is asleep in LPS mode
	 * ####----####----####----####
	 */

  HAL_ResumeTick();           // Resume the SysTick Increment
  
  // SystemClock_Config();    // Uncomment to reset System clock configuration -- IF NECESSARY

  // Wait for alarm flag (should be set by callback)
  while (!rtcAlarmFlag) {
      // Optionally, sleep or do nothing
  }

  Exit_LowPowerSleepMode(); // Exit LPS mode
  // SystemClock_Config();    // Uncomment to reset System clock configuration -- IF NECESSARY

  // De-initialize the RTC -- if NECESSARY
  rtc.end();
}

void setup() {
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  RTC_setup(SLEEP_DURATION);

}

void loop(){
  Enter_LPSMode_WithRTCAlarm(SLEEP_DURATION);
}
