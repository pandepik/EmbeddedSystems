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
#include <stdlib.h>
#define color 0xFFFF

void DelayWait10ms(uint32_t n);
	
	
	uint32_t i;
	uint32_t data[3] = {0, 0, 0};
	uint32_t ball = 0;
	
	uint32_t score = 0;
	uint32_t done = 0;

	int16_t x = 5;
	int16_t y = 5;
	int16_t x_spd = 2;
	int16_t y_spd = 2;
	int16_t count = 0;
	int case1 = 0;
	int case2 = 0;
	int case3 = 0;

	int16_t b_pad_x = 0;
	const int16_t b_pad_y = 153;
	int16_t b_pad_spd = 0;

	const int16_t r_pad_x = 122;
	int16_t r_pad_y = 0;
	int16_t r_pad_spd = 0;
	
	const int16_t sq = 6;
	const int16_t rad = 4;
	int16_t length = 50;

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
//	ST7735_DrawCircle(63, 80, sq, color); //Starting ball
//	IO_Touch();
/*	
	ST7735_SetCursor(5, 5);
	ST7735_OutString("Choose a ball");
	ST7735_SetCursor(7, 6);
	ST7735_OutString("to play");
	ST7735_FillRect(40, 108, sq, sq, color); //Starting square
	ST7735_DrawCircle(80, 110, rad, color); //Starting ball
	ST7735_SetCursor(6, 9);
	ST7735_OutString("SW1");
	ST7735_SetCursor(12, 9);
	ST7735_OutString("SW2");
	IO_Touch();
	if((GPIO_PORTF_DATA_R&0x01)==0x00){
		ball = 1;
	}
	else if((GPIO_PORTF_DATA_R&0x10)==0x00){
		ball = 0;
	}
	ST7735_SetCursor(5, 5);
	ST7735_OutString("             ");
	ST7735_SetCursor(7, 6);
	ST7735_OutString("       ");
	ST7735_SetCursor(6, 9);
	ST7735_OutString("   ");
	ST7735_SetCursor(12, 9);
	ST7735_OutString("   ");
	ST7735_FillRect(40, 108, sq, sq, 0);
	ST7735_DrawCircle(80, 110, rad, 0);
  if(ball == 1){
		ST7735_DrawCircle(x, y, rad, color); //Starting ball
	}
	else{
		ST7735_FillRect(x, y, sq, sq, color); //Starting square
	}
	ST7735_FillRect(b_pad_x, b_pad_y, length, sq, color);
	ST7735_FillRect(r_pad_x, r_pad_y, sq, length, color);
	*/
  while(1) {
		ADC_In89(data);
		data[0] /= 100;
		data[1] /= 100;
		ST7735_SetCursor(0, 0);
		ST7735_OutUDec(data[0]);ST7735_OutString(" ");
		ST7735_SetCursor(6, 0);
		ST7735_OutUDec(data[1]);
	}
}	
	/*while(1){
		if(done==0){
			ST7735_SetCursor(0,0);
			ST7735_OutString("Score");
			ST7735_SetCursor(6, 0);
			LCD_OutDec(score);	
			IO_HeartBeat();
		}
		for(i=0; i<350000; i++){} //Delay
		//bounce off left wall
		if((x+x_spd) <= 0 || (x+x_spd+rad) >= 122){
			x_spd *= -1;
			//Sound_Play();
			if(ball==1){
				ST7735_MoveCBall(x, y, x+x_spd, y+y_spd, rad, color);
			}
			else{
			ST7735_MoveSBall(x, y, x+x_spd, y+y_spd, sq, color);
			}
		}			
		//bounce off top wall
		else if((y+y_spd) <= 0 || (y+y_spd+rad) >= 153){
			y_spd *= -1;
			//Sound_Play();
			if(ball==1){
				ST7735_MoveCBall(x, y, x+x_spd, y+y_spd, rad, color);
			}
			else{
			ST7735_MoveSBall(x, y, x+x_spd, y+y_spd, sq, color);
			}
		}
		else if((x+x_spd+rad) >= 122 && ((y+y_spd)>=r_pad_y && (y+y_spd)<=(r_pad_y+length))){	//Bounces off right paddle																																			//if present
			x_spd *= -1;
			//Sound_Play();
			if(ball==1){
				ST7735_MoveCBall(x, y, x+x_spd, y+y_spd, rad, color);
			}
			else{
			ST7735_MoveSBall(x, y, x+x_spd, y+y_spd, sq, color);
			}
			count++;
			score++;	
		}
		else if((y+y_spd+rad) >= 153 &&	((x+x_spd)>=b_pad_x && (x+x_spd)<=(b_pad_x+length))){	//Bounces off bottom paddle																																				//if present
			y_spd *= -1;
			//Sound_Play();
			if(ball==1){
				ST7735_MoveCBall(x, y, x+x_spd, y+y_spd, rad, color);
			}
			else{
			ST7735_MoveSBall(x, y, x+x_spd, y+y_spd, sq, color);
			}
			count++;
			score++;
		}
		else if((x+x_spd+rad) >= 122 && !((y+y_spd)>=r_pad_y && (y+y_spd)<=(r_pad_y+length))){ //Doesn't bounce off right paddle
			while(x < 127 && y < 160){
				if(ball==1){
					ST7735_MoveCBall(x, y, x+x_spd, y+y_spd, rad, color);
				}
				else{
					ST7735_MoveSBall(x, y, x+x_spd, y+y_spd, sq, color);
				}
				x += x_spd;
				y += y_spd;
			}
			ST7735_SetCursor(6, 6);
			ST7735_OutString("Game Over");
			done = 1;
		}
		else if((y+y_spd+rad) >= 153 &&	!((x+x_spd)>=b_pad_x && (x+x_spd)<=(b_pad_x+length))){// Doesn't bounce off bot paddle
			while(x < 127 && y < 160){
				if(ball==1){
					ST7735_MoveCBall(x, y, x+x_spd, y+y_spd, rad, color);
				}
				else{
					ST7735_MoveSBall(x, y, x+x_spd, y+y_spd, sq, color);
				}
				x += x_spd;
				y += y_spd;
			}
			ST7735_SetCursor(6, 6);
			ST7735_OutString("Game Over");
			done = 1;
		}
		else{
			if(ball==1){
				ST7735_MoveCBall(x, y, x+x_spd, y+y_spd, rad, color);
			}
			else{
				ST7735_MoveSBall(x, y, x+x_spd, y+y_spd, sq, color);
			}
		}
		if(done==0){
			x += x_spd;
			y += y_spd;
		}
		switch(score){
			case 5: 
			if(case1 == 1){break;}                                                                           
			if(x_spd ==2){
				x_spd += (rand()%2);
			}
			else if(x_spd == -2){
				x_spd -= (rand()%2);
			}
			if(y_spd == 2){
				y_spd += (rand()%2);
			}
			else if(y_spd == -2){
				y_spd -= (rand()%2);
			}
			case1 = 1;
			break;
			case 10: if(case2 == 1){break;}
			if(x_spd >0){
				x_spd += (rand()%2);
			}
			else{
				x_spd -= (rand()%2);
			}
			if(y_spd > 0){
				y_spd += (rand()%2);
			}
			else{
				y_spd -= (rand()%2);
			}
			case2 = 1;
			break;
			case 17: if(case3 == 1){break;}
			if(x_spd >0){
				x_spd += (rand()%2);
			}
			else{
				x_spd -= (rand()%2);
			}
			if(y_spd > 0){
				y_spd += (rand()%2);
			}
			else{
				y_spd -= (rand()%2);
			}
			case3 = 1;
			break;
		}
			ADC_In89(data);			//data[0] = PE5(x)  data[1] = PE4(y)
			data[0] /= 1000;    //
			data[1] /= 1000;
			switch(data[0]){
				case 0: b_pad_spd = -5;
					break;
				case 1: b_pad_spd = -3;
					break;
				case 2: b_pad_spd = 0;
					break;
				case 3: b_pad_spd = 3;
					break;
				case 4: b_pad_spd = 5;
					break;
			}
			switch(data[1]){
				case 0: r_pad_spd = -6;
					break;
				case 1: r_pad_spd = -4;
					break;
				case 2: r_pad_spd = 0;
					break;
				case 3: r_pad_spd = 4;
					break;
				case 4: r_pad_spd = 6;
					break;
			}

			ST7735_MoveBPaddle(b_pad_x, b_pad_y, b_pad_x + b_pad_spd, length-(count/2), sq, color);
			ST7735_MoveRPaddle(r_pad_x, r_pad_y, r_pad_y + r_pad_spd, length-(count/2), sq, color);
			
			b_pad_x += b_pad_spd;
			r_pad_y += r_pad_spd;
  }
}*/

/*
			
			data[0] /= 100;    //
			data[1] /= 100;
			if(data[0] > 0 && data[0] <= 10){
				b_pad_spd = 4;
			}
			else if(data[0] > 10 && data[0] <= 17){
				b_pad_spd = 2;
			}
			else if(data[0] > 17 && data[0] <= 22){
				b_pad_spd = 0;
			}
			else if(data[0] > 22 && data[0] <= 30){
				b_pad_spd = -2;
			}
			else if(data[0] > 30 && data[0] <= 40){
				b_pad_spd = -4;
			}
			
			if(data[1] > 0 && data[1] <= 10){
				r_pad_spd = -5;
			}
			else if(data[1] > 10 && data[1] <= 17){
				r_pad_spd = -3;
			}
			else if(data[1] > 17 && data[1] <= 22){
				r_pad_spd = 0;
			}
			else if(data[1] > 22 && data[1] <= 30){
				r_pad_spd = 3;
			}
			else if(data[1] > 30 && data[1] <= 40){
				r_pad_spd = 5;
			}
*/
			
			