// Sleep mode implementation using registers
void enter_sleep_mode_reg(void)
{
    // Clear SLEEPDEEP bit in System Control Register
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
    
    // Set main regulator mode
    PWR->CR1 &= ~PWR_CR1_LPR;
    
    // Wait For Interrupt instruction
    __WFI();
}

// Low power run mode implementation using registers  
void enter_low_power_run_mode_reg(void)
{
    // Set LPR bit to enable low power run mode
    PWR->CR1 |= PWR_CR1_LPR;
    
    // Wait until REGLPF flag is set
    while((PWR->SR2 & PWR_SR2_REGLPF) == 0);
}

// Low power sleep mode implementation using registers
void enter_low_power_sleep_mode_reg(void)
{
    // Clear SLEEPDEEP bit 
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
    
    // Set low power regulator mode
    PWR->CR1 |= PWR_CR1_LPR;
    
    // Wait For Interrupt
    __WFI();
}

// Stop 0 mode implementation using registers
void enter_stop0_mode_reg(void)
{
    // Set SLEEPDEEP bit
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    
    // Clear LPMS bits
    PWR->CR1 &= ~PWR_CR1_LPMS;
    
    // Set Stop 0 mode
    PWR->CR1 |= PWR_CR1_LPMS_STOP0;
    
    // Wait For Interrupt
    __WFI();
}

// Stop 1 mode implementation using registers
void enter_stop1_mode_reg(void)
{
    // Set SLEEPDEEP bit
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    
    // Clear LPMS bits
    PWR->CR1 &= ~PWR_CR1_LPMS;
    
    // Set Stop 1 mode 
    PWR->CR1 |= PWR_CR1_LPMS_STOP1;
    
    // Wait For Interrupt
    __WFI();
}

// Stop 2 mode implementation using registers
void enter_stop2_mode_reg(void)
{
    // Set SLEEPDEEP bit
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    
    // Clear LPMS bits
    PWR->CR1 &= ~PWR_CR1_LPMS;
    
    // Set Stop 2 mode
    PWR->CR1 |= PWR_CR1_LPMS_STOP2;
    
    // Wait For Interrupt  
    __WFI();
}

// Standby mode implementation using registers
void enter_standby_mode_reg(void)
{
    // Set SLEEPDEEP bit
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    
    // Clear LPMS bits
    PWR->CR1 &= ~PWR_CR1_LPMS;
    
    // Set Standby mode
    PWR->CR1 |= PWR_CR1_LPMS_STANDBY;
    
    // Wait For Interrupt
    __WFI();
}

// Shutdown mode implementation using registers
void enter_shutdown_mode_reg(void)
{
    // Set SLEEPDEEP bit
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    
    // Clear LPMS bits
    PWR->CR1 &= ~PWR_CR1_LPMS;
    
    // Set Shutdown mode
    PWR->CR1 |= PWR_CR1_LPMS_SHUTDOWN;
    
    // Wait For Interrupt
    __WFI();
}