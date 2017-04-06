; Delay.s
; You must complete this file
; 1. Complete the body of the delay routine by putting the correct constant in R0
; 2. Provide the correct EXPORT statement
        AREA    |.text|, CODE, READONLY, ALIGN=2
        THUMB
		EXPORT Delay
;; linkage: ;--UUU--Put correct EXPORT statement here -----

;------------Delay------------
; Delay for 62 ms
; Input: none
; Output: none
; Modifies: R0
;bus = 80MHz, 12.5ns
;4 cycles in simulation, 3 on the real board
Delay LDR  R0,=0x12EBC0     ;0x193A55		;--UUU--  Put correct value here -----
wait  SUBS R0,#1    ;delay, 3 cycles/loop
      BNE  wait
      BX   LR

    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file
