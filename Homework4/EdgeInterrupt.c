// EdgeInterrupt.c
// Runs on LM4F120 or TM4C123
// Request an interrupt on the falling edge of PF4 (when the user
// button is pressed) and increment a counter in the interrupt.  Note
// that button bouncing is not addressed.
// Daniel Valvano
// September 14, 2013

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers"
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014
   Volume 1, Program 9.4
   
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Volume 2, Program 5.6, Section 5.5

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// user button connected to PF4 (increment counter on falling edge)

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include <stdlib.h>
#include "Print.h"
#include "ST7735.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

void randomize(void);
void store_x(void);
void store_y(void);

uint16_t x, y, color;
uint16_t x_coor, y_coor;
uint16_t x_dir = 0, y_dir = 0;

// global variable visible in Watch window of debugger
// increments at least once per button press
volatile uint32_t RisingEdges = 0;

void EdgeCounter_Init(void){                          
  SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
  RisingEdges = 0;             // (b) initialize counter
	GPIO_PORTF_LOCK_R |= 0x4C4F434B;
	GPIO_PORTF_CR_R |= 0x1F;
  GPIO_PORTF_DIR_R &= ~0x11;    // (c) make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x11;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x11;     //     enable digital I/O on PF4   
  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x11;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x11;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R |= 0x11;    	//     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x11;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
  EnableInterrupts();           // (i) Clears the I bit
	randomize();
	store_x();
	store_y();
	ST7735_DrawPixel(x, y, color);
}

void GPIOPortF_Handler(void){
  GPIO_PORTF_ICR_R = 0x10;      // acknowledge flag4
  randomize();
	if((GPIO_PORTF_DATA_R&0x01)==0x00){
			if(y_dir == 0){
				if((y_coor+y)>160){
					y = 159 - y_coor;
				}
				ST7735_DrawFastVLine(x_coor, y_coor, y, color);
				if((y_coor+y)==159){
					y_dir ^= 1;		
				}
			}
			else if(y_dir == 1){
				if(y_coor-y<0){
					y = y_coor;	
				}
				ST7735_DrawFastVLine(x_coor, (y_coor - y), y, color);
				if(y_coor==y){
					y_dir ^= 1;
				}
			}
			store_y();
		}
		else if((GPIO_PORTF_DATA_R&0x10)==0x00){ 
			if(x_dir == 0){
				if((x_coor+x)>128){
					x = 127 - x_coor;
				}
				ST7735_DrawFastHLine(x_coor, y_coor, x, color);
				if((x_coor+x) == 127){
					x_dir ^= 1;
				}
			}
			else if(x_dir == 1){
				if(x_coor-x < 0){
					x = x_coor;
				}
				ST7735_DrawFastHLine((x_coor - x), y_coor, x, color);
				if(x_coor == x){
					x_dir ^= 1;
				}
			}
//			ST7735_DrawFastHLine(x_coor, y_coor, x, color);		
			store_x();
		}
}

void randomize(void){
	x = rand()%128;
	y = rand()%160;
	color = rand()%0xFFFF;
}	

void store_x(void){
	if(x_dir == 0){
		x_coor += x;
	}
	else{
		x_coor -= x;
	}
}

void store_y(void){
	if(y_dir ==0){
		y_coor += y;
	}
	else{
		y_coor -= y;
	}
}

