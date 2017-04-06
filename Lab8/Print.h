// Print.h
// This software has interface for printing
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 
// Last Modified:  
// Lab number: 6

#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>

//-----------------------LCD_OutDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: none
// Output: none
void LCD_OutDec(uint32_t);

//-----------------------LCD_OutDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: none
// Output: none
void LCD_OutFix(uint8_t []);
