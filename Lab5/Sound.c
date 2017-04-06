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
#include "piano.h"

//--UUU--
// Declare 4-bit 32-element sine wave table as an array (see dac.xls)
// Also declare other globals like index into the wave table
uint32_t piano[61] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 
	12, 13, 14, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 
	1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 14, 
	13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

uint32_t trumpet[61] = {10, 11, 11, 12, 10, 8, 3, 1, 8, 15, 15,
	11, 10, 10, 11, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11,
	11, 11, 11, 10, 10, 10, 10, 11, 11, 12, 10, 8, 3, 1, 8, 15,
	15, 11, 10, 10, 11, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11,
	11, 11, 11, 10};

uint32_t horn[61] = {7, 8, 8, 8, 9, 10, 12, 15, 15, 15, 13, 10,
	7, 4, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 6, 7, 8, 8, 9, 9,
	10, 11, 11, 12, 13, 13, 12, 12, 13, 14, 12, 11, 9, 8, 6, 3,
	2, 1, 1, 0, 1, 1, 2, 2, 3, 4, 4, 6, 7, 7};

uint32_t guitar[61] = {5, 5, 4, 1, 1, 3, 8, 11, 11, 9, 4, 2, 5, 11,
	15, 13, 9, 7, 5, 5, 6, 8, 8, 7, 4, 3, 3, 3, 3, 4, 5, 5, 5, 5,
	4, 1, 1, 3, 8, 11, 11, 9, 4, 2, 5, 11, 15, 13, 9, 7, 5, 5, 6, 
	8, 8, 7, 4, 3, 3, 3, 4};

uint32_t flute[61] = {6, 7, 9, 9, 10, 11, 12, 13, 13, 14, 15, 15, 
	15, 15, 15, 14, 13, 13, 12, 11, 10, 9, 8, 7, 7, 6, 6, 5, 5,
	5, 5, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 1, 1, 1, 0,
	0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5};

uint8_t idx = 0;	
extern uint8_t input;
extern uint8_t b_input;
extern uint8_t p_input;	
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
		GPIO_PORTF_DIR_R |= 0x0E;
//		GPIO_PORTF_PUR_R |= 0x11;
		GPIO_PORTF_AFSEL_R |= 0x00;
		GPIO_PORTF_DEN_R |= 0x1F;
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
void Sound_Play(uint32_t period){
  //--UUU--
	//  change the note by changing when systick interrupts
	NVIC_ST_RELOAD_R = period;
}

// Interrupt service routine
// Executed periodically, the actual period
// determined by the current Reload.
void SysTick_Handler(void){
	//--UUU--
	//  Write one sample out to the DAC 
		GPIO_PORTF_DATA_R ^= 0x08;
			if(input > 0x00){
				if(p_input==0x00){
					DAC_Out(piano[idx]);
					idx = (idx + 1)%61;
				}
				else if(p_input==0x01){
					DAC_Out(guitar[idx]);
					idx = (idx + 1)%61;
				}
				else if(p_input==0x02){
					DAC_Out(trumpet[idx]);
					idx = (idx + 1)%61;
				}
				else if(p_input==0x03){
					DAC_Out(flute[idx]);
					idx = (idx + 1)%61;
				}
			}
			else{
				Sound_Play(0);
			}
}
