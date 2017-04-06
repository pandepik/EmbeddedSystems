// InputOutput.c
// Runs on LM4F120/TM4C123
// Test the switch initialization functions by setting the LED
// color according to the status of the switches.
// Sopheara Duong
// June 20, 2014

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013
   Section 4.2    Program 4.1

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

// negative logic switches connected to PF0 and PF4 on the Launchpad
// red LED connected to PF1 on the Launchpad
// blue LED connected to PF2 on the Launchpad
// green LED connected to PF3 on the Launchpad
// NOTE: The NMI (non-maskable interrupt) is on PF0.  That means that
// the Alternate Function Select, Pull-Up Resistor, Pull-Down Resistor,
// and Digital Enable are all locked for PF0 until a value of 0x4C4F434B
// is written to the Port F GPIO Lock Register.  After Port F is
// unlocked, bit 0 of the Port F GPIO Commit Register must be set to
// allow access to PF0's control registers.  On the LM4F120, the other
// bits of the Port F GPIO Commit Register are hard-wired to 1, meaning
// that the rest of Port F can always be freely re-configured at any
// time.  Requiring this procedure makes it unlikely to accidentally
// re-configure the JTAG pins as GPIO, which can lock the debugger out
// of the processor and make it permanently unable to be debugged or
// re-programmed.
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
//#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
#define PF0       (*((volatile uint32_t *)0x40025004))
#define PF4       (*((volatile uint32_t *)0x40025040))
//#define SWITCHES  (*((volatile uint32_t *)0x40025044))
//#define SW1       0x10                      // on the left side of the Launchpad board
//#define SW2       0x01                      // on the right side of the Launchpad board
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control
//#define RED       0x02
#define BLUE      0x04
//#define GREEN     0x08
void PortF_Init(void){ volatile uint32_t delay;
  SYSCTL_RCGCGPIO_R |= 0x20;  // 1) activate clock for Port F
  delay = SYSCTL_RCGCGPIO_R;        // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x04;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  //GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x05;          // 7) enable digital I/O on PF4-0
}
uint32_t PortF_Input(void){     
  return (GPIO_PORTF_DATA_R&0x01);  // read PF4,PF0 inputs
}

void PortF_OutputOn(uint32_t data){ // write PF3-PF1 outputs
  GPIO_PORTF_DATA_R = data;      
}

void PortF_Toggle(uint32_t data){
	GPIO_PORTF_DATA_R ^= data;
}

void delay(void){
	int count = 0x2D090;
	while(count != 0)
	{
		count--;
	}
}

int main(void){ 
	uint32_t status;
  PortF_Init();              // initialize PF0 and PF4 and make them inputs
  PortF_OutputOn(BLUE);                           // make PF3-1 out (PF3-1 built-in LEDs)
  while(1){
		delay();
    status = PortF_Input();
    if (status == 0x00)
		{
			PortF_Toggle(BLUE);
		}
		else
		{
			PortF_OutputOn(BLUE);
		}
		
/*		switch(status){                    // switches are negative logic on PF0 and PF4
      case 0x01: PortF_Toggle(BLUE); break;   // SW1 pressed
      //case 0x10: PortF_Output(RED); break;    // SW2 pressed
      case 0x00: PortF_OutputOn(BLUE); break;  // both switches pressed
      //case 0x11: PortF_Output(0); break;      // neither switch pressed*/
    }
  }


