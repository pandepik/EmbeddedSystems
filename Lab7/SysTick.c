// SysTick.c
// --UUU--
// Provide implementations for the two routines SysTick_Wait and SysTick_Wait10ms 
#include <stdint.h>
#include "TExaS.h"
#include "ST7735.h"
#include "ADC.h"
#include "Print.h"
#include "../inc/tm4c123gh6pm.h"

// Initialize SysTick with busy wait running at bus clock.
//#define NVIC_ST_CTRL_R      (*((volatile unsigned long *)0xE000E010))
//#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))
//#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))
//#define NVIC_SYS_PRI3_R 		(*((volatile unsigned long *)0xE000ED20))
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
extern uint32_t Convert(uint32_t);
extern uint32_t Data;
extern uint32_t Position;

void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;
		NVIC_ST_RELOAD_R = 0x00FFFFFF;	//Set the period length
		NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000;
		NVIC_ST_CTRL_R = 0x07;					//Enable SysTick with Interrupts
}

void SysTick_Handler(void){
		PF2 ^= 0x04; 
		Data = ADC_In();
		PF3 = 0x08;
		Position = Convert(Data);
		PF3 = 0x00;
		PF1 = 0x02;
		ST7735_SetCursor(0,0);
		LCD_OutFix(Position);
		PF1 = 0x00;
}


