; PLL.s
; Runs on LM4F120/TM4C123
; A software function to change the bus speed using the PLL.
; The EQU statement allows the function PLL_Init() to
; initialize the PLL to the desired frequency.
; Daniel Valvano
; September 11, 2013
;
;
;
;Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
;   You may use, edit, run or distribute this file
;   as long as the above copyright notice remains
;THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
;OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
;MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
;VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
;OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
;For more information about my classes, my research, and my books, see
;http://users.ece.utexas.edu/~valvano/

; The EQU statement SYSDIV2 initializes
; the PLL to the desired frequency.
SYSDIV2                EQU 4
; bus frequency is 400MHz/(SYSDIV2+1) = 400MHz/(4+1) = 80 MHz

SYSCTL_RIS_R           EQU 0x400FE050
SYSCTL_RIS_PLLLRIS     EQU 0x00000040  ; PLL Lock Raw Interrupt Status
SYSCTL_RCC_R           EQU 0x400FE060
SYSCTL_RCC_XTAL_M      EQU 0x000007C0  ; Crystal Value
SYSCTL_RCC_XTAL_6MHZ   EQU 0x000002C0  ; 6 MHz Crystal
SYSCTL_RCC_XTAL_8MHZ   EQU 0x00000380  ; 8 MHz Crystal
SYSCTL_RCC_XTAL_16MHZ  EQU 0x00000540  ; 16 MHz Crystal
SYSCTL_RCC2_R          EQU 0x400FE070
SYSCTL_RCC2_USERCC2    EQU 0x80000000  ; Use RCC2
SYSCTL_RCC2_DIV400     EQU 0x40000000  ; Divide PLL as 400 MHz vs. 200
                                       ; MHz
SYSCTL_RCC2_SYSDIV2_M  EQU 0x1F800000  ; System Clock Divisor 2
SYSCTL_RCC2_SYSDIV2LSB EQU 0x00400000  ; Additional LSB for SYSDIV2
SYSCTL_RCC2_PWRDN2     EQU 0x00002000  ; Power-Down PLL 2
SYSCTL_RCC2_BYPASS2    EQU 0x00000800  ; PLL Bypass 2
SYSCTL_RCC2_OSCSRC2_M  EQU 0x00000070  ; Oscillator Source 2
SYSCTL_RCC2_OSCSRC2_MO EQU 0x00000000  ; MOSC

        AREA    |.text|, CODE, READONLY, ALIGN=2
        THUMB
        EXPORT  PLL_Init

;------------PLL_Init------------
; Configure the system to get its clock from the PLL.
; Input: none
; Output: none
; Modifies: R0, R1, R2, R3
PLL_Init
    ; 0) configure the system to use RCC2 for advanced features
    LDR R3, =SYSCTL_RCC2_R          ; R3 = &SYSCTL_RCC2_R 
    LDR R2, [R3]                    
    ORR R2, R2, #0x80000000         ; USERCC2
    STR R2, [R3]                 
    ; 1) bypass PLL while initializing
    ORR R2, R2, #0x00000800         ; BYPASS2
    STR R2, [R3]               
    ; 2) select the crystal value and oscillator source
    LDR R1, =SYSCTL_RCC_R           ; R1 =&SYSCTL_RCC_R 
    LDR R0, [R1]                  
    BIC R0, R0, #0x000007C0         ; clear XTAL field
    ORR R0, R0, #0x00000540         ; configure for 16 MHz crystal
    STR R0, [R1]   
    BIC R2, R2, #0x00000070         ; MOSC
    ; 3) activate PLL by clearing PWRDN
    BIC R2, R2, #0x00002000         ; Power-Down PLL 
    ; 4) set the desired system divider
    ORR R2, R2, #0x40000000         ; use 400 MHz PLL
    BIC R2, R2, #0x1FC00000         ; clear system clock divider field
    ADD R2, R2, #(SYSDIV2<<22)      ; SYSDIV2 = 4 (80 MHz clock)
    STR R2, [R3]                    ; set RCC2
    ; 5) wait for the PLL to lock by polling PLLLRIS
    LDR R1, =SYSCTL_RIS_R           ; R1 = &SYSCTL_RIS_R 
PLL_Init_loop
    LDR R0, [R1]                    ; R0 = [R1] (value)
    ANDS R0, R0, #0x00000040        ; PLL RIS
    BEQ PLL_Init_loop               ; if(R0 == 0), keep polling
    ; 6) enable use of PLL by clearing BYPASS
    BIC R2, R2, #0x00000800         ; BYPASS2
    STR R2, [R3]                    ; enable PLL   
    BX  LR

    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file
