/**
  ******************************************************************************
  * @file           : main.ino
  * @brief          : Main program body, enters STOP1 mode for a specified time
                      and wakes up using RTC alarm set (every 5 seconds by default).
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"
#include "STOP1Mode.h"
#include "STOP1_SystemClock_Config.h"
#include "STOP1_MX_Config.h"
#include <STM32RTC.h>

/* RTC related variables -----------------------------------------------------*/
STM32RTC& rtc = STM32RTC::getInstance();
volatile bool rtcAlarmFlag = false;
volatile uint16_t SLEEP_DURATION = 7;
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
  * @brief Enter STOP1 mode for DELAY_S seconds,
  * wake up on RTC alarm interrupt.
  * @param delay_s: sleep duration offset in seconds
  * @retval None
 */
 void Enter_STOP1Mode_WithRTCAlarm(uint16_t delay_s){
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

  STOP1_Entry_LEDSequence();  // Indicate the entry in STOP2 mode with the LED sequence

  rtcAlarmFlag = false;

  /* All EXTI line pending bits (in EXTI pending register
	 * (EXTI_PR1), and EXTI pending register (EXTI_PR2)),
	 * and the peripheral flags generating wake-up interrupts must be cleared.
	 * Otherwise, the Stop 1 mode entry procedure is ignored and program execution continues.
	 * These bits are cleared when writing '1' to them.
	 */
	// EXTI_PR1 - 19 bits: 22, 21, 16 to 0.
	EXTI -> PR1 |= (3 << 21); // bits 22 & 21
	EXTI -> PR1 |= (1 << 16); // bit 16
	EXTI -> PR1 |= (0xFFFF << 0); // bits 15 to 0
	// EXTI_PR2 - 4 bits: 13, 9, 8, 2.
	EXTI -> PR1 |= (1 << 13); // bit 13
	EXTI -> PR1 |= (3 << 8); // bits 9 & 8
	EXTI -> PR1 |= (1 << 2); // bit 2

  HAL_SuspendTick();          // Suspend the SysTick Increment

  HAL_PWREx_EnterSTOP1Mode(PWR_STOPENTRY_WFI);
  /* ####----####----####----####
	 * MCU is asleep in STOP1 mode
	 * ####----####----####----####
	 */

  HAL_ResumeTick();           // Resume the SysTick Increment
  SystemClock_Config();       // Reset System clock configuration

  STOP1_Exit_LEDSequence();   // Indicate the exit in STOP2 mode with the LED sequence

  // Wait for alarm flag (should be set by callback)
  while (!rtcAlarmFlag) {
      // Optionally, sleep or do nothing
  }
}

void setup() {
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  RTC_setup(SLEEP_DURATION);

}

void loop(){
  Enter_STOP1Mode_WithRTCAlarm(SLEEP_DURATION);
}
