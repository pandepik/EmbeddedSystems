// Lab7.c
// Runs on LM4F120 or TM4C123
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 7/5/2016 

// Analog Input connected to PE2=ADC1
// displays on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats

#include <stdint.h>
#include "TExaS.h"
#include "ST7735.h"
#include "ADC.h"
#include "Print.h"
#include "../inc/tm4c123gh6pm.h"
#include "UART.h"

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on LCD
// main3 adds your convert function, position data is on LCD

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000021;
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
	GPIO_PORTF_DIR_R|=0x0E;
	GPIO_PORTF_AFSEL_R&=0xF1;
	GPIO_PORTF_DEN_R|=0x0E;

}
uint32_t Data;        // 12-bit ADC
uint32_t Position;    // 32-bit fixed-point 0.001 cm
uint8_t mailbox=0;
//uint32_t i=0;
int main1(void){      // single step this program and look at Data
  TExaS_Init();       // Bus clock is 80 MHz 
  ADC_Init();         // turn on ADC, set channel to 1
  while(1){                
    Data = ADC_In();  // sample 12-bit channel 1
  }
}

int main2(void){
  TExaS_Init();       // Bus clock is 80 MHz 
  ADC_Init();         // turn on ADC, set channel to 1
  ST7735_InitR(INITR_REDTAB); 
  PortF_Init();
  while(1){           // use scope to measure execution time for ADC_In and LCD_OutDec           
    PF2 = 0x04;       // Profile ADC
    Data = ADC_In();  // sample 12-bit channel 1
    PF2 = 0x00;       // end of ADC Profile
    ST7735_SetCursor(0,0);
    PF1 = 0x02;       // Profile LCD
    LCD_OutDec(Data); 
    ST7735_OutString("    ");  // these spaces are used to coverup characters from last output
    PF1 = 0;          // end of LCD Profile
  }
}

uint32_t Convert(uint32_t input){
  return (((input+1)*2000)/4096);
}
int main3(void){ 
  TExaS_Init();         // Bus clock is 80 MHz 
  ST7735_InitR(INITR_REDTAB); 
  PortF_Init();
  ADC_Init();         // turn on ADC, set channel to 1
  while(1){  
    PF2 ^= 0x04;      // Heartbeat
    Data = ADC_In();  // sample 12-bit channel 1
    PF3 = 0x08;       // Profile Convert
    Position = Convert(Data); 
    PF3 = 0;          // end of Convert Profile
    PF1 = 0x02;       // Profile LCD
    ST7735_SetCursor(0,0);
    LCD_OutDec(Data); ST7735_OutString("    "); 
    ST7735_SetCursor(6,0);
    LCD_OutFix(Position);
    PF1 = 0;          // end of LCD Profile
  }
} 

void SysTick_Init(){
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = 0x00FFFFFF;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it        
  NVIC_ST_CTRL_R = 0x07; // enable SysTick with core clock and interrupts
  // enable interrupts after all initialization is finished
}

void SysTick_Handler(void){
	//Data = ADC_In();  // sample 12-bit channel 1
	
}

// Use SysTick Interrupts to periodically sample ADC 
// inside the SysTick ISR and pass this sampled value
// to the main loop using a "Mailbox"
int main(void){
  TExaS_Init();       // Bus clock is 80 MHz 
  // --UUU-- This is your responsibility 
  ST7735_InitR(INITR_REDTAB); 
  PortF_Init();
  ADC_Init();         // turn on ADC, set channel to 1
	UART_Init();
	SysTick_Init();
  while(1){  
    PF2 ^= 0x04;      // Heartbeat
		PF3 = 0x08;       // Profile Convert
		Data = ADC_In();
		Position = Convert(Data); 
		UART_OutChar(Position);
		Position = UART_InChar();
		PF3 = 0;          // end of Convert Profile
		PF1 = 0x02;       // Profile LCD
		ST7735_SetCursor(0,0);
		LCD_OutDec(Data); ST7735_OutString("    "); 
		ST7735_SetCursor(6,0);
		LCD_OutFix(Position);
		PF1 = 0;          // end of LCD Profile
  }
 
}

