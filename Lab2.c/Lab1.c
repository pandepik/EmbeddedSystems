// ****************** Homework.c ***************
// Program written by: Sopheara Duong
// Date Created: 6/5/2016 
// Last Modified: 6/14/2016 
// Brief description of the program
///The objective of this system is to implement a
// Car door signal system
// Hardware connections: Inputs are negative logic; output is positive logic
//  PF0 is left-door input sensor (1 means door is open, 0 means door is closed)
//  PF4 is right-door input sensor (1 means door is open, 0 means door is closed)
//  PF3 is Safe (Green) LED signal - ON when both doors are closed, otherwise OFF
//  PF1 is Unsafe (Red) LED signal - ON when either (or both) doors are open, otherwise OFF
// The specific operation of this system 
//   Turn Unsafe LED signal ON if any or both doors are open, otherwise turn the Safe LED signal ON
//   Only one of the LEDs should be on at any given time.

#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
int delay1, count, i;
uint8_t sw1, led;

void init (void)
	{
	// one time initialization goes here
	SYSCTL_RCGC2_R = 0x20;	    	//Turn on clock for Port F
	delay1 = 10;
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	GPIO_PORTF_CR_R = 0x1F;
	GPIO_PORTF_DIR_R |= 0x04;      //Pin 2 as Outputs
	GPIO_PORTF_DIR_R &= ~0x01;     //Port 0 as Input
	GPIO_PORTF_AFSEL_R &= ~0x05;   //Turned off alternate funtion
	GPIO_PORTF_DEN_R |= 0x05;      //Turned on Digital Enable
	}

void ledon (void)
	{
	led = 0x04;
	GPIO_PORTF_DATA_R = led;
	}

void	toggle (void)
	{
	led ^= (sw1 << 2);
	GPIO_PORTF_DATA_R = led;
	}
	
void	delay (void)
	{
		count = 0x28A7C;
		for(i = 0; i < count; i++){}
	}

int main(void){
	
	init();
	ledon();
	while(1)
		{
		delay();	
		switch(sw1){
			case 0x00:
				ledon();		
				break;
			case 0x01:
				toggle();	
				break;
			default: ledon();
			}
		}
}	