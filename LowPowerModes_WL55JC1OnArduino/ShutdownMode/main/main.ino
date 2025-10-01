 /**
  ******************************************************************************
  * @file           : main.ino
  * @brief          : Main program body, enters Shutdown mode for a specified time -
                      oscillating between 3 (default value), 4 and 5 seconds and 
                      wakes up using RTC alarm.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"
#include "ShutdownMode.h"
#include "Shutdown_SystemClock_Config.h"
#include "Shutdown_MX_Config.h"
#include <STM32RTC.h>

#define USE_HAL_RTC_REGISTER_CALLBACKS 1

/* RTC related variables -----------------------------------------------------*/
STM32RTC& rtc = STM32RTC::getInstance();
volatile bool rtcAlarmFlag = false;
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
  * @brief Sets up the RTC through STM32duino RTC library API and functions, 
  * and the initial sleep duration DEALY_S.
  * @note This function should be called in the setup loop.
  * @param delay_s: sleep duration offset in seconds for the first set up.
  * @retval None
 */
void RTC_Setup(uint16_t delay_s){
  rtc.setClockSource(STM32RTC::LSE_CLOCK, 127, 255);
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
  * @brief Enter Shutdown mode for DELAY_S seconds,
  * wake up on RTC alarm interrupt.
  * @param delay_s: sleep duration offset in seconds
  * @retval None
 */
 void Enter_ShutdownMode_WithRTCAlarm(uint16_t delay_s){
  Shutdown_Entry_LEDSequence();  // Indicate the entry in Shutdown mode with the LED sequence

  rtcAlarmFlag = false;
  
  RTC_Setup(delay_s);  

  // No need to suspend the SysTick Increment, as Shutdown is similar
  // to a full board-reset, meaning it must be the last function called.

  HAL_PWREx_EnterSHUTDOWNMode();
  /* ####----####----####----####
	 * MCU is asleep in Shutdown mode
	 * ####----####----####----####
	 */
}

void setup() {
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  RTC_Setup(SLEEP_DURATION);

}

void loop(){
  // #### #### #### ####
  /* Insert other actions here. */
  
  // #### #### #### ####
  
  Enter_ShutdownMode_WithRTCAlarm(SLEEP_DURATION);  
  // Don't call anything past here: it will never execute.
}
