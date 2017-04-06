// Music.c
// This module contains code to play/stop a song
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 7/7/2014 
// Last Modified:  
// Lab number: 5
// put code definitions for the software (actual C code)
// this file explains how the module works

#include "Sound.h"
#include "../inc/tm4c123gh6pm.h"
#include "dac.h"
#include "timer.h"

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
#define rest		52

extern uint32_t reload (float);

uint8_t idx = 0;
float piano[] = {36.36, 32.40,
	30.58, 27.24, 24.27, 22.91, 20.41, 18.18, 16.20,
	15.29, 13.62, 12.13, 11.45, 10.20, 9.091, 8.099,
	7.645, 6.811, 6.068, 5.727, 5.102, 4.545, 4.050,
	3.822, 3.405, 3.034, 2.863, 2.551, 2.273, 2.025,
	1.910, 1.703, 1.517, 1.432, 1.276, 1.136, 1.012,
	.9556, .8513, .7584, .7159, .6378, .5682, .5062,
	.4778, .4257, .3792, .3580, .3189, .2841, .2531,
	.2389, 0};
uint8_t song[] = {E5, rest, D5, rest, C5, rest, D5,
			rest, E5, rest, E5, rest,  E5, rest, D5, rest, 
			D5, rest, D5, rest, E5, rest, G5, rest, G5, rest, 
			E5, rest, D5, rest, C5, rest, D5, rest, E5, rest, 
			E5, rest, E5, rest, D5, rest, D5, rest, E5, rest,
			D5, rest, C5}; 
uint8_t tempo[] = {.5};
	
	
	uint32_t reload2(float period){
	uint32_t temp = (int)(((period*1000000000))/12.5);
	return temp;
}

// **************Play_Song*********************
// Play song, while button pushed or until end
// Input: You will determine how you want to
//        encode the song information and pass this
//        as input
// Output: none
void Play_Song(void){ //--UUU-- replace void with appropriate song data
//--UUU-- 
// use interrupts to play song (See manual for hints) 
	
}


void Play_Song_Dac (uint8_t arr []){
		Sound_Play(reload(song[idx]));
		Timer0A_Init(*next_note(),reload2(tempo[0]));
}


void next_note(void){
	if(idx == 3){
		Song_Stop();
	}
	else{
		idx++;
		Sound_Play(reload(song[idx]));
		Timer0A_Init(*next_note(), reload2(tempo[0]));
	}
}

// **************Stop_Song*********************
// Stop the song if playing
// Input: none
// Output: none
void Song_Stop(void){ 
//--UUU-- 
// Turn off interrupts corresponding to the song playing and reset state
	
}
