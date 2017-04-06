// LCD.h
// This is the interface for low-level LCD module
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 
// Last Modified:  
// Lab number: 6

#include <stdint.h>

// ************** LCD_WriteCommand ************************
// This is a helper function that sends an 8-bit command to the LCD.
// inputs:  8-bit command to transmit
// outputs: none
// assumes: SSI0 and port A have already been initialized and enabled
void writecommand(uint8_t);

// ************** LCD_WriteData ************************
// This is a helper function that sends an 8-bit data to the LCD.
// inputs:  8-bit data to transmit
// outputs: none
// assumes: SSI0 and port A have already been initialized and enabled
void writedata(uint8_t);



