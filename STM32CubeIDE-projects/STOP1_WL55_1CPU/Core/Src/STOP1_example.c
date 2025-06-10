/**
  ******************************************************************************
  * @file    STOP1_example.c
  * @author  Andriatsilavo Matteo
  * @brief   Source file of the functions used in STOP1 mode project.
  ******************************************************************************
  */

#include "./../Inc/STOP1_example.h"

/** @brief  Redirect the output of printf to USART2
  * @retval int
  */
int __io_putchar(int ch){
    HAL_UART_Transmit(&huart2, &ch, 1, 1000);
}

/** @brief Simple software delay (blocking)
  * @param ms: approximate milliseconds to delay (not precise, depends on CPU speed)
  * @retval None
  */
void delay(uint32_t ms){
    uint32_t freq = HAL_RCC_GetHCLKFreq();
    uint32_t count = (freq / 4000) * ms; // 4 cycles per loop (approx.)
    while(count--) {
        __NOP();
    }
}

/** @brief Easier way to reduce / increase HCLK1 frequency by
  * changing the AHB divider, without reconfiguring oscillators or PLL.
  * Assumes SYSCLK is initially set at 16 MHz, through HSI16 clock.
  * Use register-access instructions (according to datasheet guidance),
  * on RCC clock configuration register (RCC_CFGR).
  * @param dividerX: accepts the following values:
  * 2, 3, 4, 5, 6, 8, 10, 16, 32, 64, 128, 256, 512.
  * Any other value does not divide SYSCLK (1 does not divide, >512 divides too much,
  * and by default, any unusual value will not divide).
  * @retval None
  */
void Divide_HCLK1_By(uint16_t dividerX){
    uint8_t HCLK1_prescaler;
    if (dividerX >= 2 && dividerX <= 512){
      switch(dividerX){
        case 2:   HCLK1_prescaler = 8;    break; // "1000"
        case 3:   HCLK1_prescaler = 1;    break; // "0001"
        case 4:   HCLK1_prescaler = 9;    break; // "1001"
        case 5:   HCLK1_prescaler = 2;    break; // "0010"
        case 6:   HCLK1_prescaler = 5;    break; // "0101"
        case 8:   HCLK1_prescaler = 10;   break; // "1010"
        case 10:  HCLK1_prescaler = 6;    break; // "0110"
        case 16:  HCLK1_prescaler = 11;   break; // "1011"
        case 32:  HCLK1_prescaler = 7;    break; // "0111"
        case 64:  HCLK1_prescaler = 12;   break; // "1100"
        case 128: HCLK1_prescaler = 13;   break; // "1101"
        case 256: HCLK1_prescaler = 14;   break; // "1110"
        case 512: HCLK1_prescaler = 15;   break; // "1111"
        default:  HCLK1_prescaler = 0; break; // "0000", no division
      }
    } else HCLK1_prescaler = 0; // no division

    // Reset the current prescaler value into bits 7 to 4 (HPRE)
    RCC -> CFGR &= ~(0xF << 4);
    // Insert the new prescaler value
    RCC -> CFGR |= (HCLK1_prescaler << 4);
    /* Wait for the hardware to aknowledge the new prescaler value
     * using HCLK1 prescaler flag (HPREF) on bit 16:
     * 0 → HCLK1 prescaler value not yet applied
     * 1 → HCLK1 prescaler value applied (RCC -> CFGR & (1 << 16))
     * OR using LL function `LL_RCC_IsActiveFlag_HPRE(void)`
     */ while(!(LL_RCC_IsActiveFlag_HPRE())){}
}

/** @brief Makes the CPU goes into low power run mode (LPRun),
  * according to the datasheet guidance. HSE32 is not used,
  * HSI16 is.
  * @retval None
  */
void Enter_LowPowerRunMode(void){
    // Set the current mode to LPRUN
    currentMode = MODE_LPRUN;

	// 1. set the FPDR bit in PWR_CR1.
	HAL_PWREx_EnableFlashPowerDown(PWR_FLASHPD_LPRUN);

	// 2. Disable HSE32 clock (optionnal if not used, which is our case here).

	/* 3. Decrease the HCLK clock frequencies below 2 MHz.
	 * 16 MHz / X ≤ 2 MHz → X ≥ 8; and 8 is a usual value of divider
	 * among 1, 2, 3, 4, 5, 6, 8, 10, 16, 32, 64, 128, 256 and 512.
	 * But we will take the next higher value, 10, to ensure we are below 2 MHz.
	 */
    Divide_HCLK1_By(10);

	// 4. Set the LPR bit in PWR_CR1.
	HAL_PWREx_EnableLowPowerRunMode();

	// 5. Check if the low-power regulator is ready,
	// by checking the REGLPS bit (PWR_SR2, bit 8)
	while(!(LL_PWR_IsActiveFlag_REGLPS())){}
}

/** @brief Wakes up the CPU from low power run mode (LPRun),
  * according to the datasheet guidance. HSI16 is used.
  * @param newHCLKFreq: divider applied to SYSCLK (16 MHz here) to increase back
  * HCLK1 frequency over 2 MHz, can take the following values:
  * 1, 2, 3, 4, 5, 6, 8, 10, 16, 32, 64, 128, 256, 512, but MUST BE lower than 10;
  * e.g. among the nine first values.
  * @retval None
  */
void Exit_LowPowerRunMode(uint8_t newHCLKFreq){
	// 1. Clear the LPR bit in PWR_CR1, and,
	// 2. wait until REGLPF bit is cleared in PWR_SR2.
	HAL_PWREx_DisableLowPowerRunMode();

	// 3. Increase the HCLK clock frequencies over 2 MHz.
	// 48 MHz / X ≥ 2 MHz → X ≤ 24; but 24 is not
	// a usual value of divider (1, 2, 3, 4, 5, 6, 8, 10, 16, 32, 64, 128, 256, 512).
	// So we can take one of the nine lower values, 1 to 16.
	Divide_HCLK1_By(newHCLKFreq);
	// (HSE32 is not used here, so we skip this optionnal step.)

	// 4. Reinitialize the USART2 peripheral (since changing the HCLK frequency
	// impacts the related peripherals, such as USART2).
	MX_USART2_UART_Init();
}



/** @brief Makes the CPU go into STOP1 mode,
  * according to the datasheet guidance.
  * We are waiting on an Interrupt (WFI).
  * @retval None
  */
void Enter_STOP1Mode_OnWFI(void){
    // Set the current mode to STOP1
    currentMode = MODE_STOP1;
    
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

	// Wait a bit before entering STOP1 mode
	HAL_Delay(500);

	// Suspend the tick to prevent the SysTick interrupt every 1 ms
	HAL_SuspendTick();

	// Enter the STOP1 mode, waiting for an interrupt
	HAL_PWREx_EnterSTOP1Mode(PWR_SLEEPENTRY_WFI);
	/* ####----####----####----####
	 * MCU is asleep in STOP1 mode
	 * ####----####----####----####
	 */
}

/* No need of an exit function from STOP1 mode since we're waiting on interrupt.
    The code execution resumes from the corresponding interrupt callback and then 
    back at the start of the main while loop.
*/

/** @brief User-defined ISR when pressing any of the buttons,
  * text is displayed and LEDs are blinking.
  * Power mode is switched according to the button pressed and the current mode `currentMode`.
  * @param GPIO_Pin: Pin number that triggered the interrupt
  * @note  This function is called by the HAL when an external interrupt occurs.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    // Cases differ depending on the button pressed

    /* If user button 1 (UB1, on PA0) is pressed,
     * switch mode to LPRun occurs only if the current mode is STOP1.
     */ 
	if (GPIO_Pin == UB1_Pin){ 
        printf("Pression du Bouton 1 détectée.\r\n");
        // If we are in STOP1 mode, switch to LPRun mode
        if (currentMode == MODE_STOP1){
            /* As we wake up from STOP1 mode, we need
             * to resume the tick to allow SysTick interrupt.
             */ HAL_ResumeTick();
            
            printf("Passage en mode LPRun.\r\n");
            // Launch blue LED sequence (3 quick blinks)
            for (int _ = 0; _ < 6; _++){
                HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15); 
                delay(100);
            }
            // Switch to LPRun mode
            currentMode = MODE_LPRUN;
            Enter_LowPowerRunMode();
        } else {
            printf("\tLe mode actuel n'est pas STOP1, pas de changement de mode.\r\n");
        }
	}

    /* If user button 2 (UB2, on PA1) is pressed,
     * switch mode to STOP1 occurs only if the current mode is LPRun.
     */ 
	if (GPIO_Pin == UB2_Pin){ 
        printf("Pression du Bouton 2 détectée.\r\n");
        // If we are in LPRun mode, switch to STOP1 mode
        if (currentMode == MODE_LPRUN){
            printf("Passage en mode STOP1.\r\n");
            // Exit LPRun mode, set HCLK1 to 16 MHz
            Exit_LowPowerRunMode(1); 
            // Launch green LED sequence (3 quick blinks)
            for (int _ = 0; _ < 6; _++){
                HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9); 
                delay(100);
            }
            // Switch to STOP1 mode
            currentMode = MODE_STOP1;
            Enter_STOP1Mode_OnWFI();
        } else {
            printf("\tLe mode actuel n'est pas LPRun, pas de changement de mode.\r\n");
        }
	}
    
    /* If user button 3 (UB3, on PC6) is pressed,
     * switch mode to Run occurs only if the current mode is LPRun or STOP1.
     */ 
	if (GPIO_Pin == UB3_Pin){ 
        printf("Pression du Bouton 3 détectée.\r\n");
        // If we are in LPRun/STOP1 mode, switch to Run mode
        if (currentMode != MODE_RUN){
            printf("Passage en mode Run.\r\n");
            // If we are in LPRun mode, exit it and set HCLK1 to 16 MHz
            if (currentMode == MODE_LPRUN){
                Exit_LowPowerRunMode(1); 
            }
            // If we are in STOP1 mode, resume the tick
            if (currentMode == MODE_STOP1){
                HAL_ResumeTick();
            }
            // Launch red LED sequence (2 long blinks)
            for (int _ = 0; _ < 4; _++){
                HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_11); 
                delay(1000);
            }
            // Switch to Run mode
            currentMode = MODE_RUN;
        } else {
            printf("\tLe mode actuel est Run, pas de changement de mode.\r\n");
        }
	}
}
