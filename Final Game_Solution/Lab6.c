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
#define cursor 0x001F
#define select 0xF800
#define sol 0x001F
#define taken 0
#define up		0
#define down	1
#define left	2
#define right	3

void DelayWait10ms(uint32_t n);
void IO_Init(void);
void IO_HeartBeat(void);
void IO_Touch(void);
void LCD_OutDec(uint32_t num);
void LCD_OutFix(uint32_t num);  
uint32_t possible_moves(void);
uint32_t scoring(void);

uint32_t data[3] = {0, 0, 0};
uint32_t x_cur = 62;
uint32_t y_cur = 80;
uint32_t col_cur = 3;	//change in x direct left/right
uint32_t row_cur = 3;	//change in y direct up/down
uint32_t x_sel, y_sel;
uint32_t row_sel, col_sel;
uint8_t s_cur = 6;
uint8_t s_cir = 5;
int sol_x = 0;
int sol_y = 0;
int pressed = 0;
int moves;
uint32_t score;
uint8_t game[7][7] = {		2, 2, 1, 1, 1, 2, 2,
													2, 2, 1, 1, 1, 2, 2,
													1, 1, 1, 1, 1, 1, 1, 
													1, 1, 1, 0, 1, 1, 1,
													1, 1, 1, 1, 1, 1, 1,
													2, 2, 1, 1, 1, 2, 2,
												  2, 2, 1, 1, 1, 2, 2      };

uint32_t solution_x[31] = {62, 46, 46, 46, 62, 46, 30, 46, 94, 14, 30, 78, 46, 30, 46, 78,  78, 78, 110, 94,  62,  78, 62, 46, 46, 78, 78,  62, 46, 62, 78};
uint32_t solution_y[31] = {62, 62, 44, 62, 62, 98, 98, 98, 62, 80, 98, 98, 98, 80, 62, 62, 116, 98,  80, 62, 134, 116, 26, 44, 80, 62, 98, 116, 98, 80, 80};
int main(void){  
  PLL_Init();                           								// set system clock to 80 MHz
	ADC_Init89();
	IO_Init();
	Sound_Init(7000);
  ST7735_InitR(INITR_REDTAB);
  ST7735_FillScreen(0);            											// set screen to black
	
														
//		IO_Touch();													
																												//Drawing Initial Game Board
	int x, y;
	for(y = 62; y <= 98; y+=18){
		for(x = 14; x<= 110; x+=16){
			ST7735_DrawECircle(x, y, s_cur, color);
			if(x == 62 && y == 80){}
			else{ST7735_DrawCircle(x, y, 5, color);}
		}
	}
	for(y = 26; y <= 44; y+=18){
		for(x = 46; x <= 78; x+=16){
			ST7735_DrawECircle(x, y, s_cur, color);
			ST7735_DrawCircle(x, y, 5, color);
		}
	}
	for(y = 116; y <= 134; y+=18){
		for(x = 46; x <= 78; x+=16){
			ST7735_DrawECircle(x, y, s_cur, color);
			ST7735_DrawCircle(x, y, 5, color);
		}
	}
	ST7735_DrawECircle(x_cur, y_cur, s_cur, cursor);
	ST7735_DrawCircle(solution_x[sol_x], solution_y[sol_y], s_cir, sol);
  while(1){
//		moves = possible_moves();
//		if(moves == 0){																			//GAME OVER SCREEN
//			score	= scoring();
//			ST7735_SetCursor(0,0);
//			ST7735_OutString("Game Over");
//			ST7735_SetCursor(0,1);
//			ST7735_OutString("Score");
//			ST7735_SetCursor(6,1);
//			LCD_OutDec(score);
//			IO_Touch();
//		}
//		else{
			ADC_In89(data);																		//data[0] = PE5(y)  data[1] = PE4(x)
			data[0] /= 100;    
			data[1] /= 100;

			if((GPIO_PORTF_DATA_R&0x01)==0x00){								//If the button is pressed
				while((GPIO_PORTF_DATA_R&0x01)==0x00){}
				if(pressed == 0){
					if(game[row_cur][col_cur] != 0){							//If there is a piece there, turns blue
						ST7735_DrawCircle(x_cur, y_cur, 5, select);
						x_sel = x_cur;
						y_sel = y_cur;
						row_sel = row_cur;
						col_sel = col_cur;
						pressed = 1;
					}
					else if(game[row_cur][col_cur] != 1){					//If there is not a piece there
						ST7735_SetCursor(0,0);
						ST7735_OutString("Invalid Selection");
						uint32_t i;
						for(i = 5000000; i>0; i--){}
						ST7735_SetCursor(0,0);
						ST7735_OutString("                 ");
						}
				}
				else if(pressed == 1){
					if(x_sel == x_cur && y_sel == y_cur){					//Unselecting
						ST7735_DrawCircle(x_cur, y_cur, 5, 0xFFFF);  
						pressed = 0;
					}
					else{																					//Taking out a piece
						if(col_sel == col_cur && row_sel > row_cur){//Choosing up piece
							if(game[row_cur+1][col_cur] == 1 && game[row_cur][col_cur] == 0){
								ST7735_DrawCircle(x_cur, y_cur, s_cir, color);
								ST7735_DrawCircle(x_cur, y_cur+18, s_cir, taken);
								ST7735_DrawCircle(x_sel, y_sel, s_cir, taken);
								game[row_cur][col_cur] = 1;
								game[row_cur+1][col_cur] = 0;
								game[row_sel][col_sel] = 0;
								pressed = 0;
							}
							else{
								ST7735_SetCursor(0,0);
								ST7735_OutString("Invalid Selection");
								uint32_t i;
								for(i = 5000000; i>0; i--){}
								ST7735_SetCursor(0,0);
								ST7735_OutString("                 ");
							}
						}
						else if(col_sel == col_cur && row_sel < row_cur){//Choosing down piece
							if(game[row_cur-1][col_cur] == 1 && game[row_cur][col_cur] == 0){
								ST7735_DrawCircle(x_sel, y_sel, s_cir, taken);
								ST7735_DrawCircle(x_cur, y_cur-18, s_cir, taken);
								ST7735_DrawCircle(x_cur, y_cur, s_cir, color);
								game[row_sel][col_sel] = 0;
								game[row_cur-1][col_cur] = 0;
								game[row_cur][col_cur] = 1;
								pressed = 0;
							}
							else{
								ST7735_SetCursor(0,0);
								ST7735_OutString("Invalid Selection");
								uint32_t i;
								for(i = 5000000; i>0; i--){}
								ST7735_SetCursor(0,0);
								ST7735_OutString("                 ");
							}
						}
						else if(row_sel == row_cur && col_sel > col_cur){//Choosing left piece
							if(game[row_cur][col_cur+1] == 1 && game[row_cur][col_cur] == 0){
								ST7735_DrawCircle(x_sel, y_sel, s_cir, taken);
								ST7735_DrawCircle(x_cur+16, y_cur, s_cir, taken);
								ST7735_DrawCircle(x_cur, y_cur, s_cir, color);
								game[row_sel][col_sel] = 0;
								game[row_cur][col_cur+1] = 0;
								game[row_cur][col_cur] = 1;
								pressed = 0;
							}
							else{
								ST7735_SetCursor(0,0);
								ST7735_OutString("Invalid Selection");
								uint32_t i;
								for(i = 5000000; i>0; i--){}
								ST7735_SetCursor(0,0);
								ST7735_OutString("                 ");
							}
						}
						else if(row_sel == row_cur && col_sel < col_cur){//Choosing right piece
							if(game[row_cur][col_cur-1] == 1 && game[row_cur][col_cur] == 0){
								ST7735_DrawCircle(x_sel, y_sel, s_cir, taken);
								ST7735_DrawCircle(x_cur-16, y_cur, s_cir, taken);
								ST7735_DrawCircle(x_cur, y_cur, s_cir, color);
								game[row_sel][col_sel] = 0;
								game[row_cur][col_cur-1] = 0;
								game[row_cur][col_cur] = 1;
								pressed = 0;
							}
							else{
								ST7735_SetCursor(0,0);
								ST7735_OutString("Invalid Selection");
								uint32_t i;
								for(i = 5000000; i>0; i--){}
								ST7735_SetCursor(0,0);
								ST7735_OutString("                 ");
							}
						}
						sol_x++;
						sol_y++;
						if(sol_x != 31){
						ST7735_DrawCircle(solution_x[sol_x], solution_y[sol_y], s_cir, sol);
						}
					}
				}
			}
			int success;
			switch(pressed){
				case 0:
				if(data[1] < 20){																//Move left
					success = ST7735_MoveCursor(x_cur, y_cur, s_cur, left, cursor);
					if(success == 1){
						col_cur -= 1;
						x_cur -= 16;
					}
					while(data[1] != 20){ 												//Wait to return
						ADC_In89(data);	
						data[1] /= 100;
					}
				}
				else if(data[1] > 20){													//Move right
					success = ST7735_MoveCursor(x_cur, y_cur, s_cur, right, cursor);
					if(success == 1){
						col_cur += 1;
						x_cur += 16;
					}
					while(data[1] != 20){ 												//Wait to return
						ADC_In89(data);	
						data[1] /= 100;
					}
				}
				else if(data[0] < 20){													//Move up
					success = ST7735_MoveCursor(x_cur, y_cur, s_cur, up, cursor);
					if(success == 1){
						row_cur -= 1;
						y_cur -= 18;
					}
					while(data[0] != 20){ //Wait to return
						ADC_In89(data);	
						data[0] /= 100;
					}
				}
				else if(data[0] > 20){													//Move down
					success = ST7735_MoveCursor(x_cur, y_cur, s_cur, down, cursor);
					if(success == 1){
						row_cur += 1;
						y_cur += 18;
					}
					while(data[0] != 20){ 												//Wait to return
						ADC_In89(data);	
						data[0] /= 100;
					}
				}
				break;
				case 1:
					switch(row_sel){
						case 0:
							switch(col_sel){
							case 2:
								if(row_sel == row_cur && col_sel == col_cur){	//Move to a piece
									if(data[0] > 20){														//Move down 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
										y_cur += (18*2);
										row_cur += 2;
									}
									else if(data[1] > 20){											//Move right 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
										x_cur += (16*2);
										col_cur += 2;
									}
								}
								else if(row_sel == row_cur){									//Move back to select from right
									if(data[1] < 20){														//Move left 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
										x_cur -= (16*2);
										col_cur -= 2;
										}
									}
								else if(col_sel == col_cur){									//Move back to select from down
									if(data[0] < 20){														//Move up 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
										y_cur -= (18*2);
										row_cur -= 2;
									}
								}								
								break;
							case 3:
								if(row_sel == row_cur && col_sel == col_cur){ //Move to a piece
									if(data[0] > 20){
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
										y_cur += (18*2);
										row_cur += 2;
									}
								}
								else{																					//Move back to select from down
									if(data[0] < 20){
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
										y_cur -= (18*2);
										row_cur -= 2;
									}
								}
								break;
							case 4:
								if(row_sel == row_cur && col_sel == col_cur){	//Move to a piece
									if(data[1] < 20){														//Move left 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
										x_cur -= (16*2);
										col_cur -= 2;
									}
									else if(data[0] > 20){											//Move down 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
										y_cur += (18*2);
										row_cur += 2;
									} 
								}
								else if(col_sel == col_cur){									//Move back to select from down
									if(data[0] < 20){														//Move up 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
										y_cur -= (18*2);
										row_cur -= 2;
									}
								}
								else if(row_sel == row_cur){									//Move back to select from left
									if(data[1] > 20){														//Move right 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
										x_cur += (16*2);
										col_cur += 2;
									}
								}
								break;
						}
							break;
						case 1:
							switch(col_sel){
								case 2:
								if(row_sel == row_cur && col_sel == col_cur){	//Move to a piece
									if(data[0] > 20){														//Move down 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
										y_cur += (18*2);
										row_cur += 2;
									}
									else if(data[1] > 20){											//Move right 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
										x_cur += (16*2);
										col_cur += 2;
									}
								}
								else if(row_sel == row_cur){									//Move back to select from right
									if(data[1] < 20){														//Move left 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
										x_cur -= (16*2);
										col_cur -= 2;
										}
									}
								else if(col_sel == col_cur){									//Move back to select from down
									if(data[0] < 20){														//Move up 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
										y_cur -= (18*2);
										row_cur -= 2;
									}
								}								
								break;
							case 3:
								if(row_sel == row_cur && col_sel == col_cur){	//Move to a piece
									if(data[0] > 20){														//Move down 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
										y_cur += (18*2);
										row_cur += 2;
									}
								}
								else{																					//Move back to select from down
									if(data[0] < 20){														//Move up 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
										y_cur -= (18*2);
										row_cur -= 2;
									}
								}
								break;
							case 4:
								if(row_sel == row_cur && col_sel == col_cur){	//Move to a piece
									if(data[1] < 20){														//Move left 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
										x_cur -= (16*2);
										col_cur -= 2;
									}
									else if(data[0] > 20){											//Move down 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
										y_cur += (18*2);
										row_cur += 2;
									} 
								}
								else if(col_sel == col_cur){									//Move back to select from down
									if(data[0] < 20){														//Move up 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
										y_cur -= (18*2);
										row_cur -= 2;
									}
								}
								else if(row_sel == row_cur){									//Move back to select from left
									if(data[1] > 20){														//Move right 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
										x_cur += (16*2);
										col_cur += 2;
									}
								}
								break;
						}
							break;
						case 2:
							switch(col_sel){
								case 0:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										else if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
									}
									else if(row_sel == row_cur){
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
									}
									else if(col_sel == col_cur){
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
									}
									break;
								case 1:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										else if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
									}
									else if(row_sel == row_cur){
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
									}
									else if(col_sel == col_cur){
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
									}
									break;
								case 2:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										
									}
									else if(row_sel == row_cur && col_sel > col_cur){//Left of select
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(row_sel == row_cur && col_sel < col_cur){//Right of select
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel > row_cur){//Up of select
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel < row_cur){//Down of select
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									break;
								case 3:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										
									}
									else if(row_sel == row_cur && col_sel > col_cur){//Left of select
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(row_sel == row_cur && col_sel < col_cur){//Right of select
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel > row_cur){//Up of select
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel < row_cur){//Down of select
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									break;
								case 4:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										
									}
									else if(row_sel == row_cur && col_sel > col_cur){//Left of select
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(row_sel == row_cur && col_sel < col_cur){//Right of select
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel > row_cur){//Up of select
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel < row_cur){//Down of select
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									break;
								case 5:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										else if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
									}
									else if(row_sel == row_cur){
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
									}
									else if(col_sel == col_cur){
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
									}
									break;
								case 6:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										else if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
									}
									else if(row_sel == row_cur){
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
									}
									else if(col_sel == col_cur){
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
									}
									break;
							}
							break;
						case 3:
							switch(col_sel){
								case 0:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
									}
									else{
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
									}
									break;
								case 1:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
									}
									else{
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
									}
									break;
								case 2:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										
									}
									else if(row_sel == row_cur && col_sel > col_cur){//Left of select
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(row_sel == row_cur && col_sel < col_cur){//Right of select
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel > row_cur){//Up of select
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel < row_cur){//Down of select
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									break;
								case 3:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										
									}
									else if(row_sel == row_cur && col_sel > col_cur){//Left of select
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(row_sel == row_cur && col_sel < col_cur){//Right of select
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel > row_cur){//Up of select
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel < row_cur){//Down of select
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									break;
								case 4:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										
									}
									else if(row_sel == row_cur && col_sel > col_cur){//Left of select
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(row_sel == row_cur && col_sel < col_cur){//Right of select
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel > row_cur){//Up of select
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel < row_cur){//Down of select
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									break;
								case 5:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
									}
									else{
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
									}
									break;
								case 6:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
									}
									else{
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
									}
									break;
							}
							break;
						case 4:
							switch(col_sel){
								case 0:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										else if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
									}
									else if(row_sel == row_cur){
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
									}
									else if(col_sel == col_cur){
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
									}
									break;
								case 1:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										else if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
									}
									else if(row_sel == row_cur){
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
									}
									else if(col_sel == col_cur){
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
									}
									break;
								case 2:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										
									}
									else if(row_sel == row_cur && col_sel > col_cur){//Left of select
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(row_sel == row_cur && col_sel < col_cur){//Right of select
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel > row_cur){//Up of select
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel < row_cur){//Down of select
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									break;
								case 3:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										
									}
									else if(row_sel == row_cur && col_sel > col_cur){//Left of select
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(row_sel == row_cur && col_sel < col_cur){//Right of select
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel > row_cur){//Up of select
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel < row_cur){//Down of select
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									break;
								case 4:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										
									}
									else if(row_sel == row_cur && col_sel > col_cur){//Left of select
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(row_sel == row_cur && col_sel < col_cur){//Right of select
										if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
										while(data[1] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[1] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel > row_cur){//Up of select
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									else if(col_sel == col_cur && row_sel < row_cur){//Down of select
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										while(data[0] != 20){ //wait for it to return to normal position
											ADC_In89(data);	
											data[0] /= 100;
										}
									}
									break;
								case 5:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										else if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
									}
									else if(row_sel == row_cur){
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
									}
									else if(col_sel == col_cur){
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
									}
									break;
								case 6:
									if(row_sel == row_cur && col_sel == col_cur){
										if(data[0] < 20){//Move up 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
											y_cur -= (18*2);
											row_cur -= 2;
										}
										else if(data[1] < 20){//Move left 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
											x_cur -= (16*2);
											col_cur -= 2;
										}
									}
									else if(row_sel == row_cur){
										if(data[1] > 20){//Move right 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
											x_cur += (16*2);
											col_cur += 2;
										}
									}
									else if(col_sel == col_cur){
										if(data[0] > 20){//Move down 2
											ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
											y_cur += (18*2);
											row_cur += 2;
										}
									}
									break;
							}
							break;
						case 5:
							switch(col_sel){
							case 2:
								if(row_sel == row_cur && col_sel == col_cur){	//Move to a piece
									if(data[0] < 20){//Move up 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
										y_cur -= (18*2);
										row_cur -= 2;
									}
									else if(data[1] > 20){//Move right 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
										x_cur += (16*2);
										col_cur += 2;
									}
								}
								else if(row_sel == row_cur){									//Move back to select from right
									if(data[1] < 20){//Move left 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
										x_cur -= (16*2);
										col_cur -= 2;
										}
									}
								else if(col_sel == col_cur){									//Move back to select from down
									if(data[0] > 20){//Move down 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
										y_cur += (18*2);
										row_cur += 2;
									}
								}								
								break;
							case 3:
								if(row_sel == row_cur && col_sel == col_cur){	//Move to a piece
									if(data[0] < 20){
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
										y_cur -= (18*2);
										row_cur -= 2;
									}
								}
								else{																					//Move back to select from up
									if(data[0] > 20){
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
										y_cur += (18*2);
										row_cur += 2;
									}
								}
								break;
							case 4:
								if(row_sel == row_cur && col_sel == col_cur){	//Move to a piece
									if(data[1] < 20){//Move left 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
										x_cur -= (16*2);
										col_cur -= 2;
									}
									else if(data[0] < 20){//Move up 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
										y_cur -= (18*2);
										row_cur -= 2;
									} 
								}
								else if(col_sel == col_cur){									//Move back to select from down
									if(data[0] > 20){//Move down 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
										y_cur += (18*2);
										row_cur += 2;
									}
								}
								else if(row_sel == row_cur){									//Move back to select from left
									if(data[1] > 20){//Move right 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
										x_cur += (16*2);
										col_cur += 2;
									}
								}
								break;
							}
							break;
						case 6:
							switch(col_sel){
							case 2:
								if(row_sel == row_cur && col_sel == col_cur){	//Move to a piece
									if(data[0] < 20){//Move up 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
										y_cur -= (18*2);
										row_cur -= 2;
									}
									else if(data[1] > 20){//Move right 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
										x_cur += (16*2);
										col_cur += 2;
									}
								}
								else if(row_sel == row_cur){									//Move back to select from right
									if(data[1] < 20){//Move left 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
										x_cur -= (16*2);
										col_cur -= 2;
										}
									}
								else if(col_sel == col_cur){									//Move back to select from down
									if(data[0] > 20){//Move down 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
										y_cur += (18*2);
										row_cur += 2;
									}
								}								
								break;
							case 3:
								if(row_sel == row_cur && col_sel == col_cur){	//Move to a piece
									if(data[0] < 20){
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
										y_cur -= (18*2);
										row_cur -= 2;
									}
								}
								else{																					//Move back to select from up
									if(data[0] > 20){
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
										y_cur += (18*2);
										row_cur += 2;
									}
								}
								break;
							case 4:
								if(row_sel == row_cur && col_sel == col_cur){	//Move to a piece
									if(data[1] < 20){//Move left 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
										x_cur -= (16*2);
										col_cur -= 2;
									}
									else if(data[0] < 20){//Move up 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, up, cursor);
										y_cur -= (18*2);
										row_cur -= 2;
									} 
								}
								else if(col_sel == col_cur){									//Move back to select from down
									if(data[0] > 20){//Move down 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, down, cursor);
										y_cur += (18*2);
										row_cur += 2;
									}
								}
								else if(row_sel == row_cur){									//Move back to select from left
									if(data[1] > 20){//Move right 2
										ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
										x_cur += (16*2);
										col_cur += 2;
									}
								}
								break;
							}
							break;
			}
			break;
			}
		}
	}
//}

uint32_t scoring(void){
	int a, b, score;
	for(a = 0; a<7; a++){
				for(b = 0; b<7; b++){
					if(game[a][b] == 1){
						score++;
					}
				}
			}
	return score;
}

uint32_t possible_moves(void){
	int r, c;
//Up		[r-1][c]   [r-2][c]
//Left  [r][c-1]   [r][c-2]
//Right [r][c+1]   [r][c+2]
//Down  [r+1][c]   [r+2][c]
	//Checking possible moves for first row and second row
	for(r = 0; r < 2; r++){
		if(game[r][2] == 1){
				if(game[r+1][2] == 1 && game[r+2][2] == 0){return 1;}//Down move
				if(game[r][3] == 1 && game[r][4] == 0){return 1;}//Right move
			}
			if(game[r][3] == 1){
				if(game[r+1][3] == 1 && game[r+2][3] == 0){return 1;}//Down move
			}
			if(game[0][4] == 1){
				if(game[r][3] == 1 && game[r][2] == 0){return 1;}//Left move
				if(game[r+1][4] == 1 && game[r+2][4] == 0){return 1;}//Down move
			}
		}
	//Checking possible moves for sixth and seventh row
	for(r = 5; r < 7; r++){
		if(game[r][2] == 1){
				if(game[r-1][2] == 1 && game[r-2][2] == 0){return 1;}//Up move
				if(game[r][3] == 1 && game[r][4] == 0){return 1;}//Right move
			}
			if(game[r][3] == 1){
				if(game[r-1][3] == 1 && game[r-2][3] == 0){return 1;}//Up move
			}
			if(game[0][4] == 1){
				if(game[r][3] == 1 && game[r][2] == 0){return 1;}//Left move
				if(game[r-1][4] == 1 && game[r-2][4] == 0){return 1;}//Up move
			}
		}
	//Checking possible moves for first and second column
	for(c = 0; c<2; c++){
		if(game[2][c] == 1){  //column 2-4
			if(game[3][c] == 1 && game[4][c] == 0){return 1;}//Down move
			if(game[2][c+1] == 1 && game[2][c+2] == 0){return 1;} //Right move
		}
		if(game[3][c] == 1){
			if(game[3][c+1] == 1 && game[3][c+2] == 0){return 1;} //Right move
		}
		if(game[4][c] == 1){
			if(game[3][c] == 1 && game[2][c] == 0){return 1;} //Up move
			if(game[4][c+1] == 1 && game[4][c+2] == 0){return 1;}//Right move
		}
	}
	//Checking possible moves for seventh and eighth column
	for(c = 5; c<7; c++){
		if(game[2][c] == 1){  //column 2-4
			if(game[3][c] == 1 && game[4][c] == 0){return 1;}//Down move
			if(game[2][c-1] == 1 && game[2][c-2] == 0){return 1;}//Left move
		}
		if(game[3][c] == 1){
			if(game[3][c-1] == 1 && game[3][c-2] == 0){return 1;} //Left move
		}
		if(game[4][c] == 1){
			if(game[4][c-1] == 1 && game[4][c-2] == 0){return 1;}//Left move
			if(game[3][c] == 1 && game[2][c] == 0){return 1;}//Up move
		}
	}
	//Checking possible moves for third, fourth, and fifth row
	for(r=2; r<5; r++){ 
			if(game[r][2] == 1){
				if(game[r-1][2] == 1 && game[r-2][2] == 0){return 1;}//Up move
				if(game[r][1] == 1 && game[r][0] == 0){return 1;}//Left move
				if(game[r][3] == 1 && game[r][4] == 0){return 1;}//Right move
				if(game[r+1][2] == 1 && game[r+2][2] == 0){return 1;}//Down move
			}
			if(game[r][3] == 1){
				if(game[r-1][3] == 1 && game[r-2][3] == 0){return 1;}//Up move    
				if(game[r][2] == 1 && game[r][1] == 0){return 1;}//Left move  
				if(game[r][4] == 1 && game[r][5] == 0){return 1;}//Right move 
				if(game[r+1][3] == 1 && game[r+2][3] == 0){return 1;}//Down move  
			}
			if(game[r][4] == 1){
				if(game[r-1][4] == 1 && game[r-2][4] == 0){return 1;}//Up move
				if(game[r][3] == 1 && game[r][2] == 0){return 1;}//Left move
				if(game[r][5] == 1 && game[r][6] == 0){return 1;}//Right move
				if(game[r+1][4] == 1 && game[r+2][4] == 0){return 1;}//Down move
			}
	}
//Up		[r-1][c]   [r-2][c]
//Left  [r][c-1]   [r][c-2]
//Right [r][c+1]   [r][c+2]
//Down  [r+1][c]   [r+2][c]			
	return 0;
}
