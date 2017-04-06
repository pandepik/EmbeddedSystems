// SysTick.c
// --UUU--
// Provide implementations for the two routines SysTick_Wait and SysTick_Wait10ms 
#include <stdint.h>
// Initialize SysTick with busy wait running at bus clock.
#define NVIC_ST_CTRL_R      (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
	NVIC_ST_RELOAD_R =0x00FFFFFF;
	NVIC_ST_CURRENT_R = 0;
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}
// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay){
 // --UUU--
	volatile uint32_t elapsedTime;
  uint32_t startTime = NVIC_ST_CURRENT_R;
  do{
    elapsedTime = (startTime-NVIC_ST_CURRENT_R)&0x00FFFFFF;
  }
  while(elapsedTime <= delay);
}
// 10000us equals 10ms
void SysTick_Wait1ms(unsigned long delay){
 // --UUU--
	unsigned long i;
	for(i=0; i<delay; i++)
	{
		SysTick_Wait(80000);  //Waiting 1ms
	}
}

