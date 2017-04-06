// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 3/6/2015 
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void){ 
	SYSCTL_RCGCGPIO_R |= 0x10;
	while((SYSCTL_RCGCGPIO_R&0x10)==0x00){}
	GPIO_PORTE_DIR_R &= ~0x04;
	GPIO_PORTE_AFSEL_R |= 0x04;
	GPIO_PORTE_DEN_R &= ~0x04;
	GPIO_PORTE_AMSEL_R |= 0x04;
	SYSCTL_RCGC0_R |= 0x00010000;		//Activate ADC
	SYSCTL_RCGC0_R &= 0x00000300;		//Configure for 125k
	ADC0_SSPRI_R = 0x3210;					//Sequencer 3 is lowest priority
	ADC0_ACTSS_R &= ~0x08;					//Disable sample sequencer 3
	ADC0_EMUX_R &= ~0xF000;					//Sample Sequencer 3 is software triggered
	ADC0_SSMUX3_R &= ~0x07;					//Clear SS3 field
	ADC0_SSMUX3_R += 1;							//Set to channel 1
	ADC0_SSCTL3_R = 0x06;						//
	ADC0_IM_R &= ~0x08;							//Disable SS3 Interrupts
	ADC0_ACTSS_R |= 0x08;						//Enable sample sequencer 3
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void){  
	ADC0_PSSI_R = 0x08;
	while((ADC0_RIS_R&0x08)==0){}
  uint32_t result = ADC0_SSFIFO3_R &0xFFF;
	ADC0_ISC_R |= 0x08;
	return result;
}


