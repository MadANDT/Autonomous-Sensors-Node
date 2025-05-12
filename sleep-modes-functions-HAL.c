// Sleep mode implementation using HAL
void enter_sleep_mode(void)
{
    // Configure sleep mode entry with main regulator
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

// Low power run mode implementation using HAL
void enter_low_power_run_mode(void)
{
    // Enable low power run mode
    HAL_PWREx_EnableLowPowerRunMode();
}

// Low power sleep mode implementation using HAL
void enter_low_power_sleep_mode(void)
{
    // Configure sleep mode entry with low power regulator
    HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

// Stop 0 mode implementation using HAL  
void enter_stop0_mode(void)
{
    // Enter Stop 0 mode
    HAL_PWREx_EnterSTOP0Mode(PWR_STOPENTRY_WFI);
}

// Stop 1 mode implementation using HAL
void enter_stop1_mode(void) 
{
    // Enter Stop 1 mode
    HAL_PWREx_EnterSTOP1Mode(PWR_STOPENTRY_WFI);
}

// Stop 2 mode implementation using HAL
void enter_stop2_mode(void)
{
    // Enter Stop 2 mode  
    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
}

// Standby mode implementation using HAL
void enter_standby_mode(void)
{
    // Enter Standby mode
    HAL_PWR_EnterSTANDBYMode();
}

// Shutdown mode implementation using HAL 
void enter_shutdown_mode(void)
{
    // Enter Shutdown mode
    HAL_PWREx_EnterSHUTDOWNMode();
}