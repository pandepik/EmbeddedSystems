// Piano.h
// This software configures the off-board piano keys
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 7/7/2014 
// Last Modified:  
// Lab number: 5
// Header files contain the prototypes for public functions
// this file explains what the module does (the Interface)
// (the how is in the corresponding C file)
#include <stdint.h>

// Header files contain the prototypes for public functions
// this file explains what the module does

// **************Piano_Init*********************
// Initialize piano key inputs, called once 
// Input: none 
// Output: none
void Piano_Init(void);

// **************Piano_In*********************
// Input from piano key inputs 
// Input: none 
// Output: 0 to 2^n - 1 depending on n keys
uint32_t Piano_In(void);
