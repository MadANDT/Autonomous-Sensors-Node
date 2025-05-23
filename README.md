# Autonomous-Sensors-Node
With the STM32WL55JC1 board, the goal is to acquire measures from sensors, <br> 
send them via LoRa and switch to sleep mode. <br>
The *autonomous* aspect is linked to the system being supplied with a solar panel and/or a battery, <br>
and with the possibility to easily change the sleep mode used.

Different configurations of low-power modes will be tested and implemented with simple examples (blinking LEDs, messages on USART2, etc.: 
* normal sleep modes: *Sleep*, *Low Power Run (LPR)*, *Low Power Sleep (LPS)*;
* **deep** sleep modes: _STOP0_, _STOP1_, _STOP2_, _STANDBY_ and _Shutdown_.

Real-time visualization of the board consumption (current, voltage, power and such) will be possible using the [Otii software](https://www.qoitech.com/software/) solution. 
A more economic approach will be to use the [Power Profiler Kit II tool from *NORDIC Semiconductors*](https://www.nordicsemi.com/Products/Development-hardware/Power-Profiler-Kit-2).

A first approach regarding the implementation of low-power modes will be to use Arduino IDE combined with STM32Cube[MX](https://www.st.com/en/development-tools/stm32cubemx.html) / [IDE](https://www.st.com/en/development-tools/stm32cubeide.html).<br>
*If no results are exploitable, we'll go with micro-Python / C.*
The solution making the choice of configuration mode easier will be retained.
<br><br>
I will indicate in which context I am using this system later.
All there is to know for now (list of **This must ...**):
- the board must consume the least possible;
- a wide variety of sensor types (temperature, humidity, light intensity, etc.) must be usable with our system.
<br><br>
The current structure of this project is as follows.<br>
Each low power mode:
* has its own STM32 project, exploitable on STM32Cube IDE, in the _STM32CubeIDE-project_ folder
(I should use the `🟦 Generate Report` function in order for anyone to get the necessary information to understand what is going on.);
- has its own Otii project, exploitable on Otii 3 software (⚠️**version 3.5.2**), to visualize the data recorded.
<br>
I tried to use the same configuration in every STM32 project: clock source, clock frequency, peripherals used and their configurations, etc. <br><br>

Progression: 3/8 
- ☑️ DONE: Sleep (WFI/WFE), Low Power Run, Low Power Sleep;
- ⏳ Currently working on: STOP0, STOP1;
- 🗒️ TODO: STOP2, STANDBY, Shutdown.

---

A **huge** thanks to Mr. Sylvain MONTAGNY ([@SylvainMontagny](https://github.com/SylvainMontagny)), of Savoie Mont Blanc University, for his amazing work to understand the low power modes on STM32 boards.<br>
Even though I am not using the same board, it was quite simple to grasp the subtleties of implementing the necessary code. <br>
**[Check his paper here!](https://scem-eset.univ-smb.fr/wp-content/uploads/2021/06/Mastering-Low-Power-in-STM32.pdf)** 