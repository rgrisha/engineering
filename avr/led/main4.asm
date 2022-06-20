; Program LED_blink

; definitions
.DEVICE ATtiny26
;.INCLUDE "../tn26def.inc"


.CSEG
.ORG 0000
		rjmp	init	

		RETI ; Int0-Interrupt
		RETI ; Int1-Interrupt
		RETI ; TC1-Capture
		RETI ; TC1-Compare A
		RETI ; TC1-Compare B
		RETI ; TC1-Overflow
		RETI ; overflow cnt 0
		;RJMP   tc0i ; Timer/Counter 0 Overflow, my jump to the service routine
		RETI ; Serial Transfer complete
		RETI ; UART Rx complete
		RETI ; UART Data register empty
		RETI ; UART Tx complete
		RETI ; Analog Comparator

;videobits led pixels
led_char_bits:
.DB 0x77, 0x24, 0x5D, 0x6D, 0x2E, 0x6B, 0x7A, 0x25, 0x7F, 0x2F, 0x2F

.DSEG
key1cnt: .BYTE 1	; key 1 hold cycles
key2cnt: .BYTE 1	; key 2 hold cycles
key3cnt: .BYTE 1	; key 3 hold cycles
key4cnt: .BYTE 1	; key 4 hold cycles
key5cnt: .BYTE 1	; key 5 hold cycles
key6cnt: .BYTE 1	; key 6 hold cycles
videomem: .BYTE 8	; 2 bytes per char. first byte is char led bits
					; 2 nd byte is: least 2 is blink frq:
					; 00 no blink 01 rare blink 10 frequent 11 fade blink
					

.DEF	var1 = R16
.DEF	var2 = R17
.DEF	var3 = R18
.DEF	var4 = R20
.DEF	var5 = R21
.DEF	var6 = R22
	
; Instruction start at adress 0000

.CSEG


init:		
;set the stackpointer
			ldi		var1, RAMEND
			out		SP, var1
			rjmp	main

.INCLUDE "led_base.inc"
		
main:		
; start the main programm
			ldi		var1, 0xFF
			out		DDRB, var1
			out		DDRA, var1
			ldi 	var5, 0x00

loopm:
			ldi		var4, 0xEF

			mov 	R16,var5
			ldi 	R17,0
			rcall	put_char_pos	

			inc 	var5
			cpi 	var5,0x0A
			brne 	cont_7
			ldi 	var5,0
			rjmp 	loopm
cont_7:


			out		PORTA, var4

			ldi		var1, 0x00
			out		PORTB, var1
			ldi		var1, 0xFF
			out		PORTB, var1

			ldi		zh, high( videomem )
			ldi		zl, low( videomem )
			ld 	 	var3,z
			out		PORTA, var3

			rcall	wait
			rcall	wait


			rjmp	loopm

wait:		ldi		var1, 0xFF
lp1_wait:	ldi		var2, 0xFF
lp2_wait:	dec		var2
			brne	lp2_wait
			dec		var1
			brne	lp1_wait
			ret

