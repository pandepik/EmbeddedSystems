// ****************** Lab1.c ***************
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
int delay;
uint8_t sw0, sw4;

int main(void){
// one time initialization goes here
	SYSCTL_RCGCGPIO_R = 0x20;	    	//Turn on clock for Port F
	delay = 10;
	GPIO_PORTF_DIR_R |= 0x0A;      //Port 1 and 3 as Outputs
	GPIO_PORTF_DIR_R &= ~0x11;     //Port 0 and 4 as Inputs
	GPIO_PORTF_PUR_R |= 0x11;			 //Pull Up Enable Port 4
	GPIO_PORTF_AFSEL_R &= ~0x1B;   //Turned off alternate funtion
//	GPIO_PORTF_PUR_R |= 0x01;		   //Pull Up Enable Port 0
	GPIO_PORTF_DEN_R |= 0x1B;      //Turned on Digital Enable
	
	
	while(1){
// operations to be executed over and over go here - Using bit-wise logic operations is efficient
	sw4 = GPIO_PORTF_DATA_R & 0x10;
	sw0 = GPIO_PORTF_DATA_R & 0x01;
	if ((sw4 == 0) && (sw0 == 0))
		{
		GPIO_PORTF_DATA_R = 0x08;
		}
	else
		{
		GPIO_PORTF_DATA_R = 0x02;
		}
  }
}
