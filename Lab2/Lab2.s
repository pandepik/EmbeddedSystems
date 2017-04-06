;****************** Lab2.s ***************
; Program written by: put your names here
; Date Created: 6/5/2016 
; Last Modified: 6/5/2016 
; Brief description of the program
;   If the switch is pressed, the LED toggles at 8 Hz
;   8 Hz implies 8 cycles per second. A cycle is "one ON and one OFF"
;      - - - - - - - - 
;      1010101010101010
;      <-    1sec    -> 
; Hardware connections
;  PE0 is switch input  (1 means pressed, 0 means not pressed --- Positive Logic)
;  PE1 is LED output    (1 turns ON external LED   --- Positive Logic) 
; Operation
;	1) Make PE1 an output and make PE0 an input. 
;	2) The system starts with the LED on (make PE1 = 1). 
;   3) Wait about 1/16th of a second
;   4) If the switch is pressed (PE0 is 1), then toggle the LED once, else turn the LED on. 
;   5) Steps 3 and 4 are repeated over and over
; Notes:
;   * Switch is interfaced using positive logic with external pull-down resistor, so
;     there is no need to engage the internal pull-down.
;   * LED current draw is >10mA, so it needs an external driver(7406) 

GPIO_PORTE_DATA_R       EQU   0x400243FC
GPIO_PORTE_DIR_R        EQU   0x40024400
GPIO_PORTE_AFSEL_R      EQU   0x40024420
GPIO_PORTE_DEN_R        EQU   0x4002451C
GPIO_PORTE_AMSEL_R      EQU   0x40024528
GPIO_PORTE_PCTL_R       EQU   0x4002452C
SYSCTL_RCGC2_R          EQU   0x400FE108
DELAYTIME				EQU	  0x00028A7C  

       AREA    |.text|, CODE, READONLY, ALIGN=2
       THUMB
       EXPORT  Start
		   
Start	;Set Port E
		LDR 	R0, =SYSCTL_RCGC2_R
		LDR 	R1, [R0]
		ORR 	R1, #0x10
		STR 	R1, [R0]
		;Delay
		NOP
		NOP
		;Set DIR register
		LDR 	R0, =GPIO_PORTE_DIR_R
		LDR 	R1, [R0]
		AND 	R1, #0xFE
		ORR		R1, #0x02
		STR 	R1, [R0]
		;Turn off alt funct
		LDR		R0, =GPIO_PORTE_AFSEL_R
		LDR		R1, [R0]
		AND		R1, #0xFC
		STR		R1, [R0]
		;Turn on Digital Enable
		LDR 	R0, =GPIO_PORTE_DEN_R
		LDR		R1, [R0]
		ORR		R1, #0x03
		STR		R1, [R0]
		;Turn PE1 on
		LDR		R0, =GPIO_PORTE_DATA_R
		LDR		R1, [R0]
		ORR		R1, #0x02
		STR		R1, [R0]
		;Set Delay Length
		LDR		R3, =DELAYTIME
LOOP  	
		BL		DELAY				;Wait 1/16th sec
		LDR		R1, [R0]			;Update the switch
		AND		R2, R1, #0x01		;Isolate switch
		CMP		R2, #0				;Check if switch is on
		BEQ		SWOFF				;Branch if switch is off
		LSL		R2, R2, #0x01		;Shift left to pin 1
		EOR		R1, R1, R2			;Toggles Led if switch is on
		STR		R1, [R0]			;Store back into Data register
		B    	LOOP
SWOFF	MOV		R1, #0x02			;Turns on the LED
		STR		R1, [R0]	
		B		LOOP

;; Delay subroutine here
DELAY	CMP		R3, #0				;Check if counter is 0
		BEQ		DONE	
		SUB 	R3, #1				;Counter is decremented
		B		DELAY
DONE	LDR		R3, =DELAYTIME		;Reset delay counter
		BX		LR

      ALIGN      ; make sure the end of this section is aligned
      END        ; end of file
       