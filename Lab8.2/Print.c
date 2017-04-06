/* Print.c
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 6 device driver for any LCD
;
; As part of Lab 6, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 
*/
#include <stdint.h>
#include "ST7735.h"
/*
;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: 32-bit unsigned number
; Output: none
; Notes: This must be written as a recursive function
*/
void LCD_OutDec(uint32_t number){
	 // --UUU-- Code to convert and print a unisigned decimal
	if(number<10){
		ST7735_OutChar(number+0x30);
		return;
	}
	else{
		LCD_OutDec(number/10);
		ST7735_OutChar((number%10)+0x30);
	}
	
}	

/*
; -----------------------LCD _OutFix----------------------
; Displays characters on LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  an unsigned 32-bit number
; Outputs: none
; E.g., Input=0,    then display "0.000 " on LCD
;       Input=3,    then display "0.003 " on LCD
;       Input=89,   then display "0.089 " on LCD
;       Input=123,  then display "0.123 " on LCD
;       Input=9999, then display "9.999 " on LCD
;       Input>9999, then display "*.*** " on LCD
*/
void LCD_OutFix(uint32_t fixpt){
	 // --UUU-- Code to convert and print a fixed-point number
	if(fixpt>9999){
		 ST7735_OutChar(0x2A);
		 ST7735_OutChar(0x2E);
		 ST7735_OutChar(0x2A);
		 ST7735_OutChar(0x2A);
		 ST7735_OutChar(0x2A);
	 }
	 else{
		 uint8_t d;
		 d=fixpt/1000;
		 ST7735_OutChar(d+0x30);
		 ST7735_OutChar(0x2E);
		 fixpt=fixpt%1000;
		 d=fixpt/100;
		 ST7735_OutChar(d+0x30);
		 fixpt=fixpt%100;
		 d=fixpt/10;
		 ST7735_OutChar(d+0x30);
		 fixpt=fixpt%10;
		 d=fixpt;
		 ST7735_OutChar(d+0x30);
	 }

}

