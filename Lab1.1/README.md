# Lab1

The objective of this system is to implement a car door signal system.

Hardware connections: Inputs are negative logic; output is positive logic

* PF0 is left-door input sensor (1 means door is open, 0 means door is closed)
* PF4 is right-door input sensor (1 means door is open, 0 means door is closed)
* PF3 is Safe (Green) LED signal - ON when both doors are closed, otherwise OFF
* PF1 is Unsafe (Red) LED signal - ON when either (or both) doors are open, otherwise OFF

The specific operation of this system 

* Turn Unsafe LED signal ON if any or both doors are open, otherwise turn the Safe LED signal ON
* Only one of the LEDs should be on at any given time.
