// Lab5.c
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 7/7/2014 
// Last Modified:  
// Lab number: 5
// Brief description of the program
//   A digital piano with 4 keys and a 4-bit DAC
// Hardware connections
// Port B bits 3-0 have the 4-bit DAC (output)
// Port E bits 3-0 have 4 piano keys  (input)
// Port F is onboard LaunchPad switches and LED  
// Port F bits 4,0 are negative logic switches for user input  (input)
//    PF4 = toggles voice (SineWave, Trumpet, Horn, Flute)
//    PF0 = play Mario
// Piano: PF3 ISR heartbeat, PF2 main heartbeat
// Mario: PF2 Timer1A toggle, PF1 Timer0A toggle

#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "dac.h"
#include "Sound.h"
#include "Piano.h"
#include "Song.h"   // optional for extra credit

#define A0			0
#define B0			1
#define C1			2
#define D1			3
#define E1			4
#define F1			5
#define G1			6
#define A1			7
#define B1			8
#define C2			9
#define D2			10
#define E2			11
#define F2			12
#define G2			13
#define A2			14
#define B2			15
#define C3			16
#define	D3			17
#define	E3			18
#define F3			19
#define G3			20
#define A3			21
#define B3			22
#define C4 			23	
#define D4			24
#define E4			25
#define F4			26
#define G4			27 
#define A4			28 
#define B4	    29
#define C5			30
#define D5			31
#define E5			32
#define F5			33
#define G5 			34
#define A5			35 
#define B5			36
#define C6			37
#define D6			38
#define E6			39
#define F6			40
#define G6			41
#define A6			42
#define B6			43
#define C7			44
#define D7			45
#define E7			46
#define F7			47
#define G7			48
#define	A7			49
#define	B7			50
#define C8			51

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
//--UUU--
// function protypes for additional functions you write follow
//--UUU--
// Any globals you want to use go here
uint8_t input, b_input, s_input; 
uint8_t p_input = 0;
//A0(0), B0(1)
//C1(2) , D1(3) , E1(4) , F1(5) , G1(6) , A1(7) , B1(8)
//C2(9) , D2(10), E2(11), F2(12), G2(13), A2(14), B2(15)
//C3(16), D3(17), E3(18), F3(19), G3(20), A3(21), B3(22)
//C4(23), D4(24), E4(25), F4(26), G4(27), A4(28), B4(29)
//C5(30), D5(31), E5(32), F5(33), G5(34), A5(35), B5(36)
//C6(37), D6(38), E6(39), F6(40), G6(41), A6(42), B6(43)
//C7(44), D7(45), E7(46), F7(47), G7(48), A7(49), B7(50)
//C8(51)(sounds bad lol)
float period[] = {
	36.36, 32.40,
	30.58, 27.24, 24.27, 22.91, 20.41, 18.18, 16.20,
	15.29, 13.62, 12.13, 11.45, 10.20, 9.091, 8.099,
	7.645, 6.811, 6.068, 5.727, 5.102, 4.545, 4.050,
	3.822, 3.405, 3.034, 2.863, 2.551, 2.273, 2.025,
	1.910, 1.703, 1.517, 1.432, 1.276, 1.136, 1.012,
	.9556, .8513, .7584, .7159, .6378, .5682, .5062,
	.4778, .4257, .3792, .3580, .3189, .2841, .2531,
	.2389};
	
uint32_t reload(float period){
	uint32_t temp = (int)(((period*1000000)/32)/12.5);
	return temp;
}
	
int main(void){
	Piano_Init();	
  PLL_Init();          // bus clock at 80 MHz
  // all initializations go here
	Sound_Init(reload(period[C5]));
  EnableInterrupts();
  while(1){                
    //--UUU--
		// main loop, read from switchs change sounds
		GPIO_PORTF_DATA_R ^= 0x04;
		input = Piano_In();
		b_input = GPIO_PORTF_DATA_R&0x10;
		if(b_input == 0x00){
			p_input = (p_input+1)%4;
		}
//		s_input = GPIO_PORTF_DATA_R &0x01;
//		if(s_input == 0x00){
//			Play_Song();
//		}
		if((GPIO_PORTF_DATA_R&0x10)==0x10){
			Sound_Play(reload(period[C5]));
		}
		if (input == 0x01){
			Sound_Play(reload(period[C5]));	//C5
		}
		else if (input == 0x02){
			Sound_Play(reload(period[D5]));	//D5
		}
		else if(input == 0x04){
			Sound_Play(reload(period[E5]));	//E5
		}
		else if(input == 0x08){
			Sound_Play(reload(period[A0]));	//G5
		}
		else{
			Sound_Play(0);
		}
  }             
}
