// IO.c
// This software configures the switch and LED
// You are allowed to use any switch and any LED, 
// although the Lab suggests the SW1 switch PF4 and Red LED PF1
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 
// Last Modified:  
// Lab number: 6


#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>

//------------IO_Init------------
// Initialize GPIO Port for a switch and an LED
// Input: none
// Output: none
void IO_Init(void) { volatile uint32_t delay;
 // --UUU-- Code to initialize PF4 and PF2
	SYSCTL_RCGCGPIO_R|=0x20;
	delay=5;
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	GPIO_PORTF_DIR_R|=0x04;
	GPIO_PORTF_DIR_R&=0xEF;
	GPIO_PORTF_PUR_R|=0x10;
	GPIO_PORTF_AFSEL_R&=0xEB;
	GPIO_PORTF_DEN_R|=0x14;
}

//------------IO_HeartBeat------------
// Toggle the output state of the  LED.
// Input: none
// Output: none
void IO_HeartBeat(void) {
 // --UUU-- PF2 is heartbeat
	GPIO_PORTF_DATA_R^=0x04;
}


//------------IO_Touch------------
// wait for release and press of the switch
// Input: none
// Output: none
void IO_Touch(void) {
 // --UUU-- wait for release delay and then wait for press
	if((GPIO_PORTF_DATA_R&0x10)==0)
		while((GPIO_PORTF_DATA_R&0x10)==0){}
	else{
		while((GPIO_PORTF_DATA_R&0x10)==0x10){}
		while((GPIO_PORTF_DATA_R&0x10)==0){}
		}
	return;
}  
