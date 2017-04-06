// Lab5.c
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 7/7/2014 
// Last Modified:  
// Lab number: 5
// Brief description of the program
//   A digital piano with 4 keys and a 4-bit DAC
// Hardware connections
// Port B bits 3-0 have the 4-bit DAC (output)
// Port E bits 3-0 have 4 piano keys  (input)
// Port F is onboard LaunchPad switches and LED  
// Port F bits 4,0 are negative logic switches for user input  (input)
//    PF4 = toggles voice (SineWave, Trumpet, Horn, Flute)
//    PF0 = play Mario
// Piano: PF3 ISR heartbeat, PF2 main heartbeat
// Mario: PF2 Timer1A toggle, PF1 Timer0A toggle

#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "dac.h"
#include "Sound.h"


// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
//--UUU--
// function protypes for additional functions you write follow
//--UUU--
// Any globals you want to use go here
uint8_t input, input2; 
uint8_t p_input = 0;
uint8_t toggle;
int main(void){
//	Piano_Init();	
  PLL_Init();          // bus clock at 80 MHz
  // all initializations go here
	Sound_Init(7000);
  EnableInterrupts();
  while(1){                
    //--UUU--
		// main loop, read from switchs change sounds
		GPIO_PORTF_DATA_R ^= 0x04;
		input = GPIO_PORTF_DATA_R&0x01;
	}             
}
