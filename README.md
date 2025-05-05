# Autonomous-Sensors-Node
With the STM32WL55JC1 board, the goal is to acquire measures from sensors, <br> 
send them via LoRa and switch to sleep mode. <br>
The *autonomous* aspect is linked to the system being supplied with a solar panel and/or a battery.

Different configurations of low-power modes will be tested: 
*common sleep mode (Sleep mode, Low-power run mode, Low-power sleep mode), Stop 0, Stop 1, Stop 2, Standby and Shutdown*.

Real-time visualization of the board consumption (current, voltage, power and such) will be possible using the [Otii software](https://www.qoitech.com/software/) solution. 

A first approach regarding the implementation of low-power modes will be to use Arduino IDE combined with STM32Cube[MX](https://www.st.com/en/development-tools/stm32cubemx.html) / [IDE](https://www.st.com/en/development-tools/stm32cubeide.html).
*If no results are exploitable, we'll go with micro-Python / C.*
The solution making the choice of configuration mode easier will be retained.

I will indicate in which context I am using this system later.
All there is to know for now (list of **This must ...**):
- the board must consume the least possible;
- a wide variety of sensor types (temperature, humidity, light intensity, etc.) must be usable with our system.
