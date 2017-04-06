# Lab3

**Brief description of the lab**

If the switch is pressed, the LED toggles at 8 Hz

8 Hz implies 8 cycles per second. A cycle is "one ON and one OFF"

      - - - - - - - - 
      1010101010101010
      <-    1sec    -> 

**Hardware connections**

* PE0 is switch input  (1 means pressed, 0 means not pressed)
* PE1 is LED output (1 activates external LED on protoboard) 

**Overall functionality of this system is similar to Lab 2, with four changes:**

1. activate the PLL to run at 80 MHz (12.5ns bus cycle time) 
2. initialize SysTick with RELOAD 0x00FFFFFF 
3. add a heartbeat to PF2 that toggles every time through loop, every 62ms 
4. add debugging dump of input, output, and time

**Operation**

1. Make PE1 an output and make PE0 an input. 
2. The system starts with the LED on (make PE1 =1). 
3. Wait about 62 ms
4. If the switch is pressed (PE0 is 1), then toggle the LED once, else turn the LED on. 
5. Steps 3 and 4 are repeated over and over

**Notes:**

* Switch is interfaced using positive logic with external pull-down resistor, so there is no need to engage the internal pull-down.
* LED current draw is > 10mA, so it needs an external driver(7406) 

**Deliverables:**

* Provide proof that your LED is indeed blinking at 8Hz
