// Lab6.c
// Runs on LM4F120/TM4C123
// Test the functions in ST7735.c by printing basic
// patterns to the LCD.
//    16-bit color, 128 wide by 160 high LCD6
// Daniel Valvano
// Last Modified: 7/4/2016 (by Yerraballi)

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

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

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected 
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground
#include "EdgeInterrupt.h"
#include <stdio.h>
#include <stdint.h>
#include "ST7735.h"
#include "IO.h"
#include "Print.h"
#include "PLL.h"
#include "../inc/tm4c123gh6pm.h"

void DelayWait10ms(uint32_t n);
	
void IO_Init(void);
void IO_HeartBeat(void);
void IO_Touch(void);

int main(void){  
  PLL_Init();                           // set system clock to 80 MHz
  IO_Init();
  // test DrawChar() and DrawCharS()
  ST7735_InitR(INITR_REDTAB);
  ST7735_FillScreen(0xFFFF);            // set screen to white
//  ST7735_FillScreen(0);            // set screen to black
		EdgeCounter_Init();  
  while(1){
		IO_HeartBeat();
  }
}
