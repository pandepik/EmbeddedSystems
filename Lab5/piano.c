// Piano.c
// This software configures the off-board piano keys
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 7/7/2014 
// Last Modified:  
// Lab number: 5
// Hardware connections

#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
// put code definitions for the software (actual C code)
// this file explains how the module works

// **************Piano_Init*********************
// Initialize piano key inputs, called once
// Input: none 
// Output: none
void Piano_Init(void){ 
	SYSCTL_RCGCGPIO_R |= 0x10;
	while((SYSCTL_RCGCGPIO_R&0x10) == 0) {};
	GPIO_PORTE_DIR_R = 0x00;
	GPIO_PORTE_AFSEL_R = 0x00;
	GPIO_PORTE_DEN_R = 0x0F;
}
// **************Piano_In*********************
// Returns the current key combination noted
// by the ISR
// Input: none 
// Output: 0 to 2^n - 1 depending on n keys
uint32_t Piano_In(void){ 
	uint32_t result = GPIO_PORTE_DATA_R & 0x0F;
	return result;
}
