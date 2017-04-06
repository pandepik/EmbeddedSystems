# Lab2

**Brief description of the program**

If the switch is pressed, the LED toggles at 8 Hz

8 Hz implies 8 cycles per second. A cycle is "one ON and one OFF"

      - - - - - - - - 
      1010101010101010
      <-    1sec    ->
      
      
**Hardware connections**

* PE0 is switch input  (1 means pressed, 0 means not pressed --- Positive Logic)
* PE1 is LED output    (1 turns ON external LED   --- Positive Logic) 

**Operation:**

1. Make PE1 an output and make PE0 an input. 
2. The system starts with the LED on (make PE1 = 1). 
3. Wait about 1/16th of a second
4. If the switch is pressed (PE0 is 1), then toggle the LED once, else turn the LED on. 
5. Steps 3 and 4 are repeated over and over

**Notes:**

* Switch is interfaced using positive logic with external pull-down resistor, so there is no need to engage the internal pull-down.
* LED current draw is >10mA, so it needs an external driver (7406) 
