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
int pressed = 0;
int moves, status = 1;
uint32_t score = 0;
uint8_t tutorial = 0;
uint8_t game[7][7] = {		2, 2, 1, 1, 1, 2, 2,
													2, 2, 1, 1, 1, 2, 2,
													1, 1, 1, 1, 1, 1, 1, 
													1, 1, 1, 0, 1, 1, 1,
													1, 1, 1, 1, 1, 1, 1,
													2, 2, 1, 1, 1, 2, 2,
												  2, 2, 1, 1, 1, 2, 2      };



int main(void){  
  PLL_Init();                           								// set system clock to 80 MHz
	ADC_Init89();
	IO_Init();
//	Sound_Init(7000);
  ST7735_InitR(INITR_REDTAB);
  ST7735_FillScreen(0);            											// set screen to black													
	
/*	ST7735_SetCursor(5,2);
	ST7735_OutString("Peg Solitare");
	ST7735_SetCursor(6,1);
	ST7735_OutString("Brainvitta");
	ST7735_SetCursor(4,5);
	ST7735_OutString("Would you like");
	ST7735_SetCursor(7,6);
	ST7735_OutString("to play");
	ST7735_SetCursor(4,7);
	ST7735_OutString("the tutorial?");
	ST7735_SetCursor(6,10);
	ST7735_OutString("Yes     No");
	ST7735_SetCursor(1,12);
	ST7735_OutString("*** Recommended ***");
	ST7735_SetCursor(1,13);
	ST7735_OutString("So I don't have to");
	ST7735_SetCursor(2,14);
	ST7735_OutString("explain a million");
	ST7735_SetCursor(0,15);
	ST7735_OutString("times lol Thank you!");
//	ST7735_DrawHRect(32, 95, 25, 17, 1, color); //YES Square
//	ST7735_DrawHRect(76, 95, 25, 17, 1, color); //No Square
	ST7735_DrawHRect(32, 95, 25, 17, 1, color); //YES Square
	int start_cur = 32;
	while(status ==1){
		ADC_In89(data);	
		data[1] /= 100;
		if(start_cur == 32 && data[1] > 20){
			ST7735_DrawHRect(32, 95, 25, 17, 1, 0);
			ST7735_DrawHRect(76, 95, 25, 17, 1, color);
			start_cur = 76;
		}
		else if(start_cur == 76 && data[1] < 20){
			ST7735_DrawHRect(76, 95, 25, 17, 1, 0);
			ST7735_DrawHRect(32, 95, 25, 17, 1, color);
			start_cur = 32;
		}
		if((GPIO_PORTF_DATA_R&0x01)==0x00){
			if(start_cur == 32){
				tutorial = 1;
			}
			else if(start_cur == 76){
				tutorial = 0;
			}
			status = 0;
		}
	}
	
	//Tutorial
	if(tutorial == 1){
		ST7735_FillScreen(0);
		ST7735_SetCursor(6,0);
		ST7735_OutString("Tutorial");
		ST7735_SetCursor(5,3);
		ST7735_OutString("This is your");
		ST7735_SetCursor(7,4);
		ST7735_OutString(" cursor");
		ST7735_SetCursor(3,11);
		ST7735_OutString("Use the joystick");
		ST7735_SetCursor(6,12);
		ST7735_OutString("to move it");
		ST7735_SetCursor(5,14);
		ST7735_OutString("Press SW2 to");
		ST7735_SetCursor(7,15);
		ST7735_OutString("continue");
		ST7735_DrawECircle(x_cur, y_cur, s_cur, cursor);
		ST7735_DrawECircle(x_cur-16, y_cur-18, s_cur, color);
		ST7735_DrawECircle(x_cur-16, y_cur, s_cur, color);
		ST7735_DrawECircle(x_cur-16, y_cur+18, s_cur, color);
		ST7735_DrawECircle(x_cur+16, y_cur-18, s_cur, color);
		ST7735_DrawECircle(x_cur+16, y_cur, s_cur, color);
		ST7735_DrawECircle(x_cur+16, y_cur+18, s_cur, color);
		ST7735_DrawECircle(x_cur, y_cur-18, s_cur, color);
		ST7735_DrawECircle(x_cur, y_cur+18, s_cur, color);
		IO_Touch();
		while((GPIO_PORTF_DATA_R&0x01)==0x01){
			ADC_In89(data);	
			data[0] /= 100;
			data[1] /= 100;
			if(data[0] > 20){//Move down
				if(y_cur != 98){
					ST7735_MoveCursorT(x_cur, y_cur, s_cur, down, cursor);
					y_cur += 18;
				}
				while(data[0] != 20){
					ADC_In89(data);	
					data[0] /= 100;
				}
			}
			else if(data[0] < 20){//Move up
				if(y_cur != 62){
					ST7735_MoveCursorT(x_cur, y_cur, s_cur, up, cursor);
					y_cur -= 18; 
				}
				while(data[0] != 20){
					ADC_In89(data);	
					data[0] /= 100;
				}
			}
			else if(data[1] < 20){//Move left
				if(x_cur != 46){
					ST7735_MoveCursorT(x_cur, y_cur, s_cur, left, cursor);
					x_cur -= 16;
				}
				while(data[1] != 20){
					ADC_In89(data);	
					data[1] /= 100;
				}
			}
			else if(data[1] > 20){//Move right
				if(x_cur != 78){	
					ST7735_MoveCursorT(x_cur, y_cur, s_cur, right, cursor);
					x_cur += 16;
				}
				while(data[1] != 20){
					ADC_In89(data);	
					data[1] /= 100;
				}
			}
		}
		//Second page of tutorial
		ST7735_FillScreen(0);
		ST7735_SetCursor(6,0);
		ST7735_OutString("Tutorial");
		ST7735_SetCursor(1,1);
		ST7735_OutString("To remove a piece,");
		ST7735_SetCursor(1,2);
		ST7735_OutString("have a piece jump");
		ST7735_SetCursor(1,3);
		ST7735_OutString("over another piece.");
		ST7735_SetCursor(1,5);
		ST7735_OutString("Select & deselect ");
		ST7735_SetCursor(1,6);
		ST7735_OutString("the piece with SW2.");
		x_cur = 46;
		y_cur = 80;
		ST7735_DrawECircle(x_cur, y_cur, s_cur, cursor);
		ST7735_DrawECircle(x_cur+16, y_cur, s_cur, color);
		ST7735_DrawECircle(x_cur+(16*2), y_cur, s_cur, color);
		ST7735_DrawCircle(x_cur, y_cur, s_cir, color);
		ST7735_DrawCircle(x_cur+16, y_cur, s_cir, color);
		ST7735_SetCursor(3,9);
		ST7735_OutString("Then use SW2 to");
		ST7735_SetCursor(2,10);
		ST7735_OutString("select the piece");
		ST7735_SetCursor(2,11);
		ST7735_OutString("& move the cursor");
		ST7735_SetCursor(1,12);
		ST7735_OutString("to the empty space.");
		ST7735_SetCursor(3,13);
		ST7735_OutString("Select again to");
		ST7735_SetCursor(2,14);
		ST7735_OutString(" remove a piece.");
		IO_Touch();
		ST7735_DrawCircle(x_cur, y_cur, s_cir, select);
		int status = 1;
		while(status == 1){
			ADC_In89(data);	
			data[1] /= 100;
			if((GPIO_PORTF_DATA_R&0x01)==0x00){
				while((GPIO_PORTF_DATA_R&0x01)==0x00){}
				if(pressed == 0){//Select
					ST7735_DrawCircle(x_cur, y_cur, s_cir, select);
					pressed = 1;
				}
				else if(pressed == 1){
					if(x_cur == 46){//Deselect
						ST7735_DrawCircle(x_cur, y_cur, s_cir, color);
						pressed = 0;
					}
					else{						//Removing a piece
						ST7735_DrawCircle(x_cur-32, y_cur, s_cir, 0);
						ST7735_DrawCircle(x_cur-16, y_cur, s_cir, 0);
						ST7735_DrawCircle(x_cur, y_cur, s_cir, color);
						status = 0;
					}
				}
			}
			if(pressed == 0){}
			else if(pressed == 1){
				if(x_cur == 46){
					if(data[1] > 20){//Move right
							ST7735_MoveCursor2(x_cur, y_cur, s_cur, right, cursor);
							x_cur += 32;
					}
				}
				else{
					if(data[1] < 20){//Move left
							ST7735_MoveCursor2(x_cur, y_cur, s_cur, left, cursor);
							x_cur -= 32;
					}
				}
			}
		}
		ST7735_SetCursor(0,15);
		ST7735_OutString("   SW2 to continue  ");	
		IO_Touch();
		ST7735_FillScreen(0);
		ST7735_SetCursor(6,0);
		ST7735_OutString("Tutorial");
		ST7735_SetCursor(1,1);
		ST7735_OutString("Rules");
		ST7735_SetCursor(1,2);
		ST7735_OutString("A valid move is");
		ST7735_SetCursor(1,3);
		ST7735_OutString("moving a piece over");
		ST7735_SetCursor(1,4);
		ST7735_OutString("another piece into");
		ST7735_SetCursor(1,5);
		ST7735_OutString("an empty position.");
		
		ST7735_SetCursor(1,7);
		ST7735_OutString("Goal");
		ST7735_SetCursor(1,8);
		ST7735_OutString("The goal is to end");
		ST7735_SetCursor(1,9);
		ST7735_OutString("with only one piece");
		ST7735_SetCursor(1,10);
		ST7735_OutString("left which is a");
		ST7735_SetCursor(1,11);
		ST7735_OutString("Perfect Game!");
		
		ST7735_SetCursor(1,13);
		ST7735_OutString("Controls");
		ST7735_SetCursor(1,14);
		ST7735_OutString("Joystick=Move");
		ST7735_SetCursor(1,15);
		ST7735_OutString("SW2=Select/Deselect");
		IO_Touch();
	}
*/
	int i;
	int a = 14;
	int b = 16;
	ST7735_DrawCircle(x_cur, y_cur, s_cir, color);
	for(i = 1; i<5; i++){
		ST7735_DrawCircle(x_cur+(i*a), y_cur, s_cir, color);
		ST7735_DrawCircle(x_cur-(i*a), y_cur, s_cir, color);
	}
	y_cur += b;
	ST7735_DrawCircle(x_cur, y_cur, s_cir, color);
	for(i = 1; i<4; i++){
		ST7735_DrawCircle(x_cur+(i*a), y_cur, s_cir, color);
		ST7735_DrawCircle(x_cur-(i*a), y_cur, s_cir, color);
	}
	y_cur += (b);
	ST7735_DrawCircle(x_cur, y_cur, s_cir, color);
	for(i = 1; i<3; i++){
		ST7735_DrawCircle(x_cur+(i*a), y_cur, s_cir, color);
		ST7735_DrawCircle(x_cur-(i*a), y_cur, s_cir, color);
	}
	y_cur += (b);
	ST7735_DrawCircle(x_cur, y_cur, s_cir, color);
	for(i = 1; i<2; i++){
		ST7735_DrawCircle(x_cur+(i*a), y_cur, s_cir, color);
		ST7735_DrawCircle(x_cur-(i*a), y_cur, s_cir, color);
	}
	y_cur += (b);
	ST7735_DrawCircle(x_cur, y_cur, s_cir, color);
	y_cur = (80 - b);
	ST7735_DrawCircle(x_cur, y_cur, s_cir, color);
	for(i = 1; i<4; i++){
		ST7735_DrawCircle(x_cur+(i*a), y_cur, s_cir, color);
		ST7735_DrawCircle(x_cur-(i*a), y_cur, s_cir, color);
	}
	y_cur -= (b);
	ST7735_DrawCircle(x_cur, y_cur, s_cir, color);
	for(i = 1; i<3; i++){
		ST7735_DrawCircle(x_cur+(i*a), y_cur, s_cir, color);
		ST7735_DrawCircle(x_cur-(i*a), y_cur, s_cir, color);
	}
	y_cur -= (b);
	ST7735_DrawCircle(x_cur, y_cur, s_cir, color);
	for(i = 1; i<2; i++){
		ST7735_DrawCircle(x_cur+(i*a), y_cur, s_cir, color);
		ST7735_DrawCircle(x_cur-(i*a), y_cur, s_cir, color);
	}
	y_cur -= (b);
	ST7735_DrawCircle(x_cur, y_cur, s_cir, color);
	
	IO_Touch();
	
	
//Drawing Initial Game Board
	ST7735_FillScreen(0);
	x_cur = 62;
	y_cur = 80;
	pressed = 0;
	ST7735_SetCursor(0,15);
	ST7735_OutString("Perfect Score: 31");
	int x, y;
	for(y = 62; y <= 98; y+=18){
		for(x = 14; x<= 110; x+=16){
			ST7735_DrawECircle(x, y, s_cur, color);
			if(x == 62 && y == 80){}
			else{ST7735_DrawCircle(x, y, s_cir, color);}
		}
	}
	for(y = 26; y <= 44; y+=18){
		for(x = 46; x <= 78; x+=16){
			ST7735_DrawECircle(x, y, s_cur, color);
			ST7735_DrawCircle(x, y, s_cir, color);
		}
	}
	for(y = 116; y <= 134; y+=18){
		for(x = 46; x <= 78; x+=16){
			ST7735_DrawECircle(x, y, s_cur, color);
			ST7735_DrawCircle(x, y, s_cir, color);
		}
	}
	ST7735_DrawECircle(x_cur, y_cur, s_cur, cursor);
	
  while(1){
//		moves = possible_moves();
		if(score == 31){																	//GAME OVER SCREEN
			int i;
			ST7735_FillScreen(0);
			for(i = 0; i<16; i++){
				ST7735_SetCursor(0,i); 
				ST7735_OutString(" Win! Win! Win! Win!");
			}
			for(i = 0; i<16; i++){
				ST7735_SetCursor(0,i); 
				ST7735_OutString("                    ");
			}
			for(i = 15; i>=0; i--){
				ST7735_SetCursor(0,i); 
				ST7735_OutString(" Win! Win! Win! Win!");
			}
			for(i = 15; i>=0; i--){
				ST7735_SetCursor(0,i); 
				ST7735_OutString("                    ");
			}
		}
		else{
			ST7735_SetCursor(0,0);
			ST7735_OutString("Score");
			ST7735_SetCursor(6,0);
			LCD_OutDec(score);
			ADC_In89(data);																		//data[0] = PE5(y)  data[1] = PE4(x)
			data[0] /= 100;    
			data[1] /= 100;

			if((GPIO_PORTF_DATA_R&0x01)==0x00){								//If the button is pressed
				while((GPIO_PORTF_DATA_R&0x01)==0x00){}
				if(pressed == 0){
					if(game[row_cur][col_cur] != 0){							//If there is a piece there, turns blue
						ST7735_DrawCircle(x_cur, y_cur, s_cir, select);
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
								score++;
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
								score++;
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
								score++;
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
								score++;
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
}

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
