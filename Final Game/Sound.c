// Sound.c, 
// This module contains the SysTick ISR that plays sound
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 7/7/2014 
// Last Modified:  
// Lab number: 5
// put code definitions for the software (actual C code)
// this file explains how the module works

#include "../inc/tm4c123gh6pm.h"
#include "dac.h"   // will call DAC module functions from this module so include
#include <stdint.h>

//--UUU--
// Declare 4-bit 32-element sine wave table as an array (see dac.xls)
// Also declare other globals like index into the wave table
	uint32_t piano[61] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

	unsigned char pong[225] = {31,58,8,14,11,11,12,12,11,12,12,12,12,12,12,13,14,58,46,54,57,52,50,54,55,51,51,53,53,51,51,50,3,17,12,7,11,14,11,10,12,13,11,11,13,14,16,63,49,50,56,55,50,52,55,53,51,52,53,52,50,45,2,13,15,9,8,13,13,10,11,13,12,11,12,14,24,60,54,49,53,56,52,50,53,54,52,51,53,53,50,35,7,7,15,13,8,10,14,12,10,12,13,12,11,13,35,54,58,51,50,55,55,51,51,54,53,51,52,53,51,23,13,5,11,15,10,9,12,13,11,11,13,13,12,12,48,48,58,54,50,52,55,53,51,52,53,51,50,51,50,13,19,10,11,16,15,12,14,16,16,15,16,17,17,17,51,42,48,50,46,44,47,47,45,44,45,45,44,43,42,15,23,21,18,20,23,21,20,22,23,22,22,23,24,25,44,38,39,41,40,38,38,39,38,38,38,38,37,36,35,25,28,28,27,27,28,29,28,29,29,29,29,30,30};

uint32_t sound = 0;	
uint32_t idx = 0;	
// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Input: Initial interrupt period
//           Units to be determined by YOU
//           Maximum to be determined by YOU
//           Minimum to be determined by YOU
// Output: none
void Sound_Init(uint32_t period){
  //--UUU--
	//  Call DAC_Init and  other sound related initalization 
		DAC_Init();
		SYSCTL_RCGCGPIO_R |= 0x20;
		while((SYSCTL_RCGCGPIO_R&0x20) == 0) {};
		GPIO_PORTF_LOCK_R |= 0x4C4F434B;
		GPIO_PORTF_CR_R |= 0x1F;
		GPIO_PORTF_DIR_R |= 0x0C;
//		GPIO_PORTF_PUR_R |= 0x11;
		GPIO_PORTF_AFSEL_R &= ~0x1F;
		GPIO_PORTF_DEN_R |= 0x1D;
		NVIC_ST_CTRL_R = 0;
		NVIC_ST_RELOAD_R = period;	//Set the period length
		NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000;
		NVIC_ST_CTRL_R = 0x07;					//Enable SysTick with Interrupts
}

// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Input: interrupt period
//           Units to be determined by YOU
//           Maximum to be determined by YOU
//           Minimum to be determined by YOU
//         input of zero disables sound output
// Output: none
void Sound_Play(void){
  //--UUU--
	sound = 1;
	return;
}
// Interrupt service routine
// Executed periodically, the actual period
// determined by the current Reload.
void SysTick_Handler(void){
	//--UUU--
	//  Write one sample out to the DAC 
		GPIO_PORTF_DATA_R ^= 0x08;
		if(sound == 1){
			while(idx != 224){
					DAC_Out(pong[idx]);
					idx++;
			}
			idx = 0;
			sound = 0;
		}
}
