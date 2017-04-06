// LCD.c
// This software is the low-lower LCD module which involves
// sending commands and data to the LCD. Will be called by 
// Nokia5110 module for init/configuring/manipulating LCD
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 
// Last Modified:  
// Lab number: 6
/*
; The Data/Command pin must be valid when the eighth bit is
; sent.  The SSI module has hardware input and output FIFOs
; that are 8 locations deep.  Based on the observation that
; the LCD interface tends to send a few commands and then a
; lot of data, the FIFOs are not used when writing
; commands, and they are used when writing data.  This
; ensures that the Data/Command pin status matches the byte
; that is actually being transmitted.
; The write command operation waits until all data has been
; sent, configures the Data/Command pin for commands, sends
; the command, and then waits for the transmission to
; finish.
; The write data operation waits until there is room in the
; transmit FIFO, configures the Data/Command pin for data,
; and then adds the data to the transmit FIFO.
; NOTE: These functions will crash or stall indefinitely if
; the SSI0 module is not initialized and enabled.
*/

#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>


// ************** LCD writecommand ************************
// This is a helper function that sends an 8-bit command to the LCD.
// inputs: R0  8-bit command to transmit
// outputs: none
// assumes: SSI0 and port A have already been initialized and enabled
/*Steps:
1) Read SSI0_SR_R and check bit 4, 
2) If bit 4 is high, loop back to step 1 (wait for BUSY bit to be low)
3) Clear D/C=PA6 to zero
4) Write the command to SSI0_DR_R
5) Read SSI0_SR_R and check bit 4, 
6) If bit 4 is high, loop back to step 5 (wait for BUSY bit to be low)
*/
void writecommand(uint8_t cmd) {
	 // --UUU-- Code to write data to the LCD
	while((SSI0_SR_R&0x10)==0x10){}
	GPIO_PORTA_DATA_R &= ~0x40;
	SSI0_DR_R = cmd;
	while((SSI0_SR_R&0x10)==0x10){}
}

// ************** LCD writedata ************************
// This is a helper function that sends an 8-bit data to the LCD.
// inputs: R0  8-bit data to transmit
// outputs: none
// assumes: SSI0 and port A have already been initialized and enabled
/*Steps:
1) Read SSI0_SR_R and check bit 1, 
2) If bit 1 is low loop back to step 1 (wait for TNF bit to be high)
3) Set D/C=PA6 to one
4) Write the 8-bit data to SSI0_DR_R
*/
void writedata(uint8_t data) {
	 // --UUU-- Code to write a data to the LCD
	while((SSI0_SR_R&0x02)==0x00){}
	GPIO_PORTA_DATA_R |= 0x40;
	SSI0_DR_R = data;
}
