// pulse.c
// Runs on LM4F120/TM4C123
// Pulses the white led at a certain interval
// 
// Sopheara Duong
// June 23, 2016


#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include "light.h" 
#include "SysTick.h"
#define OFF					0x00
#define ON					0x01
#define TOGGLE			0x10
extern void PLL_Init(void);

void cycle (uint8_t state){
	if(state>35){
				for (int i=0; i<150;i++){
					led(ON);
					SysTick_Wait1us(state);
					led(TOGGLE);
					SysTick_Wait1us(85-state);
				}
			}
	else if(state<35) {
		for (int i=0; i<300;i++){
					led(ON);
					SysTick_Wait1us(state);
					led(TOGGLE);
					SysTick_Wait1us(85-state);
				}
			}
}
	
int main(void){ 
	light_init();
	PLL_Init();
	SysTick_Init();
	while (1){  
		for(int i=85; i>0; i--){
			cycle(i);
		}
		for (int i=0; i<85; i++){
			cycle(i);
		}
	}
	
}
