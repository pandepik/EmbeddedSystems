// Lab3.c
//****************** lab3.c ***************
// Program written by: Soph
// Date Created: 6/16/2016 
// Last Modified:  
// Brief description of the program
//   If the switch is pressed, the LED toggles at 8 Hz
//   8 Hz implies 8 cycles per second. A cycle is "one ON and one OFF"
//      - - - - - - - - 
//      1010101010101010
//      <-    1sec    -> 
//   Hardware connections
//   PE0 is switch input  (1 means pressed, 0 means not pressed)
//   PE1 is LED output (1 activates external LED on protoboard) 
// Overall functionality of this system is the similar to Lab 2, with four changes:
//  1-  activate the PLL to run at 80 MHz (12.5ns bus cycle time) 
//  2-  initialize SysTick with RELOAD 0x00FFFFFF 
//  3-  add a heartbeat to PF2 that toggles every time through loop, every 62ms 
//  4-  add debugging dump of input, output, and time
// Operation
// 	1) Make PE1 an output and make PE0 an input. 
// 	2) The system starts with the LED on (make PE1 =1). 
//  3) Wait about 62 ms
//  4) If the switch is pressed (PE0 is 1), then toggle the LED once, else turn the LED on. 
//  5) Steps 3 and 4 are repeated over and over
// Notes:
//   * Switch is interfaced using positive logic with external pull-down resistor, so
//     there is no need to engage the internal pull-down.
//   * LED current draw is > 10mA, so it needs an external driver(7406) 
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include "SysTick.h"
#include "inc/PLL.h"


// Function Prototypes
extern void PLL_Init(void);
extern void SysTick_Init (void);
void Debug_Init(void);
void Debug_Capture(void);
// --UUU-- Put appropriate function prototype to be able to call Delay 
extern void Delay (void);



//--UUU-- Port F Initialization for PF2
void PortF_Init(void){ volatile uint32_t delay;
		SYSCTL_RCGCGPIO_R |= 0x20;
		while((SYSCTL_RCGCGPIO_R&0x20) == 0) {};
		GPIO_PORTF_DIR_R = 0x04;
		GPIO_PORTF_AFSEL_R &= ~(0x04);
		GPIO_PORTF_DEN_R |= 0x04;
}
//--UUU-- Port E Initialization PE1, PE0
void PortE_Init(void){ volatile unsigned long delay;
		SYSCTL_RCGCGPIO_R |= 0x10;
		while((SYSCTL_RCGCGPIO_R&0x10) == 0) {};
		GPIO_PORTE_DIR_R |= 0x02;
		GPIO_PORTE_AFSEL_R &= ~(0x03);
		GPIO_PORTE_DEN_R |= 0x03;	
}

void ledon(void){
	GPIO_PORTE_DATA_R = 0x02;
}

void toggle(uint8_t sw){
	GPIO_PORTE_DATA_R ^= (sw << 1);
}


// Declare Instrumentation Dump arrarys 
uint32_t TimeBuffer[50];
uint32_t DataBuffer[50];
uint8_t idx, sw1;


int main(void){ 
                                                                                                                                                                                                                                                      PortF_Init();   // initialize PF2 to output heartbeat
  PortE_Init();   // initialize PE1,0
	PLL_Init();			// runs at 80 MHz
  SysTick_Init(); // initialize SysTick 
// --UUU-- Call Instrument initialization function here
	Debug_Init();
	ledon();
  while(1){
		// --UUU-- Call Instrument recording function here
		Debug_Capture();
		// --UUU-- heartbeat on PF2 toggle here
		GPIO_PORTF_DATA_R ^= 0x04;
    // --UUU--  rewrite Code from Lab2 in C here
		Delay();
		sw1 = GPIO_PORTE_DATA_R & 0x01;
		switch(sw1){
			case 0x00:
				ledon();		
				break;
			case 0x01:
				toggle(sw1);	
				break;
			default: ledon();
			}
	}
}

//;------------Debug_Init------------
// Initializes the debugging instrument
// Input: none
// Output: none
// Modifies: none
void Debug_Init(void){
//--UUU-- 
		int i;
		for(i=0;i<50;i++){
		TimeBuffer[i] = 0xFFFFFFFF;
		DataBuffer[i] = 0xFFFFFFFF;
		}
}


//;------------Debug_Capture------------
// Record/Dump Port E and time into buffers
// Input: none
// Output: none
// Modifies: none
void Debug_Capture(void){
//--UUU--	
	TimeBuffer[idx] = NVIC_ST_CURRENT_R;
	DataBuffer[idx] = ((GPIO_PORTE_DATA_R & 0x01)<<16) + ((GPIO_PORTE_DATA_R & 0x02)>>1);
	idx++;
}

/*void PLL_Init(void){
	//Configure system to use RCC2
	SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2;
	//Bypass the PLL while initializing
	SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;
	//Select the crystal value and osc source
	SYSCTL_RCC_R &= ~SYSCTL_RCC_XTAL_M;
	SYSCTL_RCC_R += SYSCTL_RCC_XTAL_16MHZ;
	SYSCTL_RCC2_R &= ~SYSCTL_RCC2_OSCSRC2_M;
	SYSCTL_RCC2_R += SYSCTL_RCC2_OSCSRC2_MO;
	//Activate the PLL by clearing PWRDN
	SYSCTL_RCC2_R &= ~SYSCTL_RCC2_DIV400;
	SYSCTL_RCC2_R = (SYSCTL_RCC2_R&~0x1FC00000)+(SYSDIV2<<22);
	//Wait for PLL to lock by polling PLL LRIS
	while((SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS)==0){};
	//Enable PLL by clearing BYPASS
	SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2;
}
*/

/*void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = 0x00FFFFFF;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0x05;
}

void SysTick_Waiting(uint32_t delay){
  volatile uint32_t elapsedTime;
  uint32_t startTime = NVIC_ST_CURRENT_R;
  do{
    elapsedTime = (startTime-NVIC_ST_CURRENT_R)&0x00FFFFFF;
  }
  while(elapsedTime <= delay);
}

void SysTick_Wait1ms(uint32_t delay){
	unsigned long i;
	for(i=0; i<delay; i++)
	{
		SysTick_Waiting(80000);  //Waiting 1ms
	}
}*/
