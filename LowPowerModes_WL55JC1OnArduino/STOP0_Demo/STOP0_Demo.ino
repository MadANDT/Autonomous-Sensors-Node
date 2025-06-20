// File: STOP0_Demo.ino

#include "stm32wlxx_hal.h"
#include "STOP0_Demo.h"
#include "STOP0_clock_config.h"
#include "MX_Config.h"

// Function declarations
void Enter_STOP0Mode_OnWFE();
void Exit_STOP0Mode_OnWFE();

// === Arduino Setup ===
void setup() {
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  Serial.begin(9600);
  delay(1000);
  Serial.println("\r\n\r\nTest d'une appli mode veille profonde sur STM32 (Arduino)");
}

// === Arduino Loop ===
void loop() {
  Serial.println("Le µCU entre en mode STOP0 via WFE - type event\r\n");

  __HAL_RCC_USART2_CLK_SLEEP_DISABLE(); // Optional: disable UART2 clock in sleep

  Enter_STOP0Mode_OnWFE();  // MCU enters STOP0 mode
  Exit_STOP0Mode_OnWFE();   // Upon wake-up
}

/** @brief Makes the CPU go into STOP0 mode,
  * according to the datasheet guidance.
  * We are waiting on Event
  * @retval None
  */
void Enter_STOP0Mode_OnWFE(void){

    /* All EXTI line pending bits (in EXTI pending register
	 * (EXTI_PR1), and EXTI pending register (EXTI_PR2)),
	 * and the peripheral flags generating wake-up interrupts must be cleared.
	 * Otherwise, the Stop 0 mode entry procedure is ignored and program execution continues.
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

	// Wait a bit before entering STOP0 mode
	HAL_Delay(1000);

	// Suspend the tick to prevent the SysTick interrupt every 1 ms
	HAL_SuspendTick();

	// Enter the STOP0 mode, waiting for an event
	HAL_PWREx_EnterSTOP0Mode(PWR_SLEEPENTRY_WFE);
	/* ####----####----####----####
	 * MCU is asleep in STOP0 mode
	 * ####----####----####----####
	 */
}

/** @brief Executes a user-defined sub-routine after the CPU
  * wakes up from STOP0 mode on an event.
  * Some peripherals are (re)initialized, we display text through
  * USART2 and the three LEDs blink.
  * @retval None
  */
void Exit_STOP0Mode_OnWFE(void){
	// At wake-up, resume the SysTick
	HAL_ResumeTick(); // at wake up, we resume the SysTick

	// (Re)Initialize the peripherals
	HAL_Init(); 			// all peripherals, Flash interface and SysTick
	MX_GPIO_Init();			// GPIO
	MX_USART2_UART_Init(); 	// USART2

	delay(1000);

	printf("Réveil par pression du Bouton 3 et son évènement en attente.\r\n");
	printf("Séquence de clignotement de LED lancée.\r\n");

	// Toggle the red, green and blue LEDs one by one, twice
	for (int _ = 0; _ < 2; _++){
		for (int l = 0; l < 2; l++){
			HAL_GPIO_TogglePin(GPIOB, RLED_Pin);
			delay(100);
		}
		for (int l = 0; l < 2; l++){
			HAL_GPIO_TogglePin(GPIOB, GLED_Pin);
			delay(100);
		}
		for (int l = 0; l < 2; l++){
			HAL_GPIO_TogglePin(GPIOB, BLED_Pin);
			delay(100);
		}
	}
}

/** @brief Redirect the output of printf to USART2
  * @retval int
  */
int __io_putchar(int ch){
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}