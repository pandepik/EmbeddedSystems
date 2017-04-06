// dac.h
// This software configures DAC output
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

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void);


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(uint32_t data);
