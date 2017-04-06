// dac.c
// This software configures DAC output
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 7/7/2014 
// Last Modified:  
// Lab number: 5
// put code definitions for the software (actual C code)
// this file explains how the module works

#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){ 
  //--UUU--
	SYSCTL_RCGCGPIO_R |= 0x02;
	while((SYSCTL_RCGCGPIO_R&0x02) == 0) {};
	GPIO_PORTB_DIR_R = 0x3F;
	GPIO_PORTB_AFSEL_R = 0x00;
	GPIO_PORTB_DEN_R = 0x3F;
}

// **************DAC_Out*********************
// output to DAC
// Input: 6-bit data, 0 to 63
// Output: none
void DAC_Out(uint32_t data){
  //--UUU--
	GPIO_PORTB_DATA_R = data;
}
