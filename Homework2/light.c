

#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#define red   0x02
#define blue  0x04
#define green 0x08


volatile uint8_t wait;
//choose color
uint8_t light = blue;

void light_init (void)
{
	SYSCTL_RCGCGPIO_R |= 0x20;  // 1) activate clock for Port F
  int delay = SYSCTL_RCGCGPIO_R;        // allow time for clock to start
  GPIO_PORTF_DIR_R = light;          // PF3 out E==white
  GPIO_PORTF_AFSEL_R = 0x00;        // disable alt funct
  GPIO_PORTF_DEN_R = light;          // enable digital I/O on PF3
}

void led (uint8_t state)
{
	//Turn the light on or off
	switch(state){
		case 0x00:GPIO_PORTF_DATA_R = 0x00; break; 
		case 0x01:GPIO_PORTF_DATA_R = light; break;
		case 0x10:GPIO_PORTF_DATA_R ^= light; break;
	}
}
