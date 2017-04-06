//****************** TrafficLight.c ***************
// Program written by: Sopheara Duong
// Date Created:  7/1/2016
// Last Modified:  7/1/2016
// Brief description of the program
//   A traffic light controller with 3 inputs and 8 output
// Hardware connections:
// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
///Blue wire = east/west

// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
///Purple Wire = north/south

// pedestrian detector (a switch) connected to PE2 (1=pedestrian present)
// north/south car detector (a switch) connected to PE1 (1=car present)
// east/west car detector (a switch) connected to PE0 (1=car present)

// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include "SysTick.h"
#include "TExaS.h"
#define comb 8
#define nstate 12

void Output (uint8_t light, uint8_t walk){
	GPIO_PORTB_DATA_R = light;
	GPIO_PORTF_DATA_R = walk;
}

void Port_Init(void){
	SYSCTL_RCGCGPIO_R = 0x32;
	while((SYSCTL_RCGCGPIO_R&0x32) == 0) {};
	GPIO_PORTB_DIR_R = 0x3F;
	GPIO_PORTE_DIR_R = 0x00;
	GPIO_PORTF_DIR_R = 0x0A;
	GPIO_PORTB_AFSEL_R = 0x00;
	GPIO_PORTE_AFSEL_R = 0x00;
	GPIO_PORTF_AFSEL_R = 0x00;
	GPIO_PORTB_DEN_R = 0x3F;
	GPIO_PORTE_DEN_R = 0x07;
	GPIO_PORTF_DEN_R = 0x0A;
}

// Function Prototypes
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

 // --UUU-- Define State struct and the new type here
 struct State {
	 uint8_t out_traf;
	 uint8_t out_walk;
	 uint32_t wait;
	 uint8_t next[comb];
 };
 
// --UUU-- Define symbolic names for each state (indexes)
	#define GreN		0
	#define GreE		1
	#define Walk		2
	#define Yel_N		3
	#define Yel_E		4
	#define Bink1		5	
	#define Bink2		6
	#define Bink3		7
	#define Bink4		8
	#define Bink5		9
	#define Bink6 	10
 	#define Yel_W 	11
 
// --UUU-- Declare and populate the array that holds the State-Transition-Graph
/*struct State fsm[nstate] = {
//                        Walk/North/East	Switch combinations    
//  Out_T| Out_W| Wait	  000 | 001  | 010  | 011  | 100  | 101  | 110  | 111   
		{0x21, 0x02 , 6000, {GreN , Yel_N, GreN , Yel_N, Yel_N, Yel_N, Yel_N, Yel_N}},	//GoN
		{0x0C, 0x02 , 6000, {GreE , GreE , Yel_E, Yel_E, Yel_E, Yel_E, Yel_E, Yel_E}},	//GoE
		{0x24, 0x08 , 7000, {Walk , Bink1, Bink1, Bink1, Walk , Bink1, Bink1, Bink1}},  //Wait
		{0x22, 0x02 , 3000, {Walk , GreE , Walk , GreE , Walk , Walk , Walk , Walk }},	//North_Yellow
		{0x14, 0x02 , 3000, {Walk , Walk , GreN , GreN , Walk , Walk , Walk , GreN }},	//East_Yellow
		{0x24, 0x02 , 800 , {Bink2, Bink2, Bink2, Bink2, Bink2, Bink2, Bink2, Bink2}},  //Blink1
		{0x24, 0x00 , 500 , {Bink3, Bink3, Bink3, Bink3, Bink3, Bink3, Bink3, Bink3}},  //Blink2
		{0x24, 0x02 , 800 , {Bink4, Bink4, Bink4, Bink4, Bink4, Bink4, Bink4, Bink4}},  //Blink3
		{0x24, 0x00 , 500 , {Bink5, Bink5, Bink5, Bink5, Bink5, Bink5, Bink5, Bink5}},  //Blink4
		{0x24, 0x02 , 800 , {Bink6, Bink6, Bink6, Bink6, Bink6, Bink6, Bink6, Bink6}},  //Blink5
		{0x24, 0x00 , 500 , {Yel_W, Yel_W, Yel_W, Yel_W, Yel_W, Yel_W, Yel_W, Yel_W}},  //Blink6
		{0x24, 0x02 , 800 , {GreN , GreE , GreN , GreE , GreN , GreE , GreN , GreE }}   //Walk_Yellow
	};
*/
struct State fsm[nstate] = {
//                        Walk/North/East	Switch combinations    
//  Out_T| Out_W| Wait	   000 | 001  | 010  | 011  | 100  | 101  | 110  | 111   
		{0x21, 0x02 , 4000, {Yel_N, Yel_N, GreN , Yel_N, Yel_N, Yel_N, Yel_N, Yel_N}},	//GoN
		{0x0C, 0x02 , 4000, {Yel_E, GreE , Yel_E, Yel_E, Yel_E, Yel_E, Yel_E, Yel_E}},	//GoE
		{0x24, 0x08 , 3000, {Bink1, Bink1, Bink1, Bink1, Walk , Bink1, Bink1, Bink1}},  //Walk
		{0x22, 0x02 , 1500, {GreE , GreE , Walk , GreE , Walk , Walk , Walk , Walk }},	//North_Yellow
		{0x14, 0x02 , 1500, {Walk , Walk , GreN , GreN , Walk , Walk , Walk , GreN }},	//East_Yellow
		{0x24, 0x02 , 800 , {Bink2, Bink2, Bink2, Bink2, Bink2, Bink2, Bink2, Bink2}},  //Blink1
		{0x24, 0x00 , 500 , {Bink3, Bink3, Bink3, Bink3, Bink3, Bink3, Bink3, Bink3}},  //Blink2
		{0x24, 0x02 , 800 , {Bink4, Bink4, Bink4, Bink4, Bink4, Bink4, Bink4, Bink4}},  //Blink3
		{0x24, 0x00 , 500 , {Bink5, Bink5, Bink5, Bink5, Bink5, Bink5, Bink5, Bink5}},  //Blink4
  	{0x24, 0x02 , 800 , {Bink6, Bink6, Bink6, Bink6, Bink6, Bink6, Bink6, Bink6}},  //Blink5
		{0x24, 0x00 , 500 , {Yel_W, Yel_W, Yel_W, Yel_W, Yel_W, Yel_W, Yel_W, Yel_W}},  //Blink6
		{0x24, 0x02 , 800 , {GreN , GreE , GreN , GreE , GreN , GreE , GreN , GreE }}   //Walk_Yellow
	};	
 
// --UUU-- Declare current state (index)
uint8_t c_state = Walk;
uint8_t input;

int main(void){ volatile unsigned long delay;
	TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210); // activate grader and set system clock to 80 MHz
  EnableInterrupts();
  SysTick_Init();    
	// --UUU--Port Initializations for all lights, sensors and pedestrian input
	Port_Init();
	// --UUU--MooreFSM Controller Engine Loop
  while(1){
		Output(fsm[c_state].out_traf, fsm[c_state].out_walk);
		SysTick_Wait1ms(fsm[c_state].wait);
		input = GPIO_PORTE_DATA_R & 0x07;
		c_state = fsm[c_state].next[input];
  }
}
