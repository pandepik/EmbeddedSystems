// Lab6.c
// Runs on LM4F120/TM4C123
// Test the functions in ST7735.c by printing basic
// patterns to the LCD.
//    16-bit color, 128 wide by 160 high LCD
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
#include <stdio.h>
#include <stdint.h>
#include "ST7735.h"
#include "IO.h"
#include "Print.h"
#include "PLL.h"
#include "../inc/tm4c123gh6pm.h"
#include "ADCSWTrigger.h"
#include "dac.h"
#include "Sound.h"
#define color 0xFFFF

void DelayWait10ms(uint32_t n);

	uint32_t i;
	uint32_t data[3] = {0, 0, 0};
	//2048 = middle of joystick

	int16_t x = 5;
	int16_t y = 5;
	int16_t x_spd = 3;
	int16_t y_spd = 3;
	uint16_t count = 0;

	int16_t b_pad_x = 0;
	const int16_t b_pad_y = 153;

	const int16_t r_pad_x = 122;
	int16_t r_pad_y = 0;

	int16_t pad_spd = 1;
	const int16_t sq = 5;
	const uint16_t length = 20;

void IO_Init(void);
void IO_HeartBeat(void);
void IO_Touch(void);
void LCD_OutDec(uint32_t num);
void LCD_OutFix(uint32_t num);  
int main(void){  
//  uint32_t i;
  PLL_Init();                           // set system clock to 80 MHz
  IO_Init();
	ADC_Init89();
	Sound_Init(7000);
    // test DrawChar() and DrawCharS()
  ST7735_InitR(INITR_REDTAB);
  ST7735_FillScreen(0);            // set screen to black
//	ST7735_DrawCircle(63, 79, 1, 0xFFFF);
	ST7735_FillRect(x, y, sq, sq, color); //Starting ball
	ST7735_FillRect(b_pad_x, b_pad_y, length, sq, color);
	ST7735_FillRect(r_pad_x, r_pad_y, sq, length, color);
	IO_Touch();
  
	
//  ST7735_OutString("Lab 6!\nUT-IITH-Study Abroad");
//  ST7735_DrawBitmap(44, 159, Logo, 40, 160);
//  i = 0;
//  IO_Touch();
//  ST7735_FillScreen(0);            // set screen to black
//	ST7735_DrawCircle(63, 79, 3, 0xFFFF);
//	IO_Touch();
  while(1){
    IO_HeartBeat();
		// only bounce if paddle |  	always bounce
		if((x+x_spd+sq) >= 122 ||       (x+x_spd) <= 0){
			x_spd *= -1;
			Sound_Play();
			//Play sound
		}			
		//only bounce if paddle | always bounce
		if(y+y_spd+sq >= 153 || (y+y_spd) <= 0){
			y_spd *= -1;
			Sound_Play();
			//Play sound
		}
		for(i=0; i<250000; i++){}
		if(x_spd > 3){
			ST7735_MoveBallB(x, y, x+x_spd, y+y_spd, color);
		}
		else{
			ST7735_MoveBall(x, y, x+x_spd, y+y_spd, color);
		}
		
		x += x_spd;
		y += y_spd;
		
		ADC_In89(data);			//data[0] = PE5(x)  data[1] = PE4(y)
//		ST7735_
  }
}

