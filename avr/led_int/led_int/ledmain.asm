; Program LED_blink

; definitions
.DEVICE ATtiny26
.INCLUDE "tn26def.inc"


.CSEG
.ORG 0000
		rjmp	init	

		RETI ; Int0-Interrupt
		RETI ; Pin change
		RETI ; TC1-Compare A
		RETI ; TC1-Compare B
		RETI ; TC1-Overflow
		RJMP   tc0i ; Timer/Counter 0 Overflow, my jump to the service routine
		RETI ; USI Start
		RETI ; USI Overflow
		RETI ; EEPROM Ready
		RETI ; Analog Comparator
		RETI ; ADC Conversion handler


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
intcnt:  .BYTE 1	; Interrupt counter. ISR increases it every int tick
					

.CSEG

.DEF	var1 = R16
.DEF	var2 = R17
.DEF	var3 = R18
.DEF	var4 = R20
.DEF	var5 = R21
.DEF	var6 = R22
.DEF 	temp = R23
	
.DEF    vk0  = R10
.DEF    vk1  = R11
.DEF    vk2  = R12
.DEF    vk3  = R13
.DEF 	vkp  = R14

; Instruction start at adress 0000

init:		
;set the stackpointer
			cli     ; disable interrupts at init routine
			ldi		var1, RAMEND
			out		SP, var1
			rjmp	main

.INCLUDE "led_base.inc"
		

;timer 0 ISR
tc0i:

			push 	zl
			push 	zh
			push 	R1
  			push	R2
			push 	R3
			push 	var1
			in  	var1,SREG
			push 	var1

			ldi 	zl,low( intcnt )
			ldi 	zh,high( intcnt )
			ld   	R1,z
			inc 	R1
			st 		z,R1
			dec  	R1
			mov 	R2,R1
			ldi 	var1,0b00000011
			and 	r2,var1 ; R2 now has masked value 0-3 for led digit
							; Each int tick we update only 1 digit

			ldi		var1, 0xEF
		
loop_get_digit:	
			breq 	cont_get_digit
			lsl 	var1
			dec 	R2
			rjmp 	loop_get_digit

cont_get_digit:
			; here we have var1 shifter from 0 to 3 times - 
			; which the digit we need

			out		PORTA, var1

			ldi		var1, 0x00
			out		PORTB, var1
			ldi		var1, 0xFF
			out		PORTB, var1
			; here we made rise from zero to one to write to 74HC175

			mov 	R2,R1
			ldi 	var1,0b00000011
			and 	r2,var1 ; R2 now has masked value 0-3 for led digit

			ldi		zh, high( videomem )
			ldi		zl, low( videomem )
			lsl 	R2
			add 	zl, R2	
			ld 	 	var1,z
			out		PORTA, var1

			ldi		var1,-4 		;1MHZ / 1024 / 4 ~ 250 ints per sec
			out		TCNT0,var1

			; end the deal with screen, keyboard starts

			ldi 	zh,high( key1cnt )
			ldi 	zl,low( key1cnt )

			ldi 	var1,0b11111001
			mov 	R1,var1

loop_keyb_out:
			ldi 	var1,0b00010000
			mov 	R3,var1

			out 	PORTB,R1
			lsl 	R1	
			ldi 	var1,0b11111000
			or 		R1,var1
			ldi 	var1,0b11100111
			and 	R1,var1

loop_keyb_in:
			nop
			nop
			in 		R2,PINB
			and 	R2,R3
			breq 	add_key
			; key press present
			ldi 	var1,0x01
			mov 	R2,var1
add_key:	
			ld  	var1,z			
			add 	var1,R2
			st 		z+,var1

			mov 	var1,R3
			cpi 	var1,0b00001000
			breq 	loop_keyb_out1
			lsr 	R3
			rjmp 	loop_keyb_in

loop_keyb_out1:
			mov 	var1,R1
			cpi 	var1,0b11100000 
			brne	loop_keyb_out

			pop 	var1
			out 	SREG,var1
			pop  	var1
			pop 	R3
			pop 	R2
 			pop		R1
			pop 	zh
			pop 	zl	

			reti

main:		
; start the main programm
			ldi		var1, 0b11100111
			out		DDRB, var1
			ldi		var1, 0xFF
			out		DDRA, var1

			ldi	temp,5		;in Clock=1/1024
			out	TCCR0,temp	;T/C0

			ldi	temp,-4 		;1MHZ / 1024 / 4 ~ 250 ints per sec
			out	TCNT0,temp	
			ldi	temp,2
			out	TIMSK,temp	;Timse Interrupts ON

			ldi zl,low( intcnt )
			ldi zh,high( intcnt )
			ldi temp, 0
 			st  z,temp

			;init keyboard counters
			ldi zl,low( key1cnt )
			ldi zh,high( key1cnt )
			st  z+,temp
			st  z+,temp
			st  z+,temp
			st  z+,temp
			st  z+,temp
			st  z,temp

			;init videomemory
			ldi temp,0xFF
			ldi zl,low( videomem )
			ldi zh,high( videomem )
			st  z+,temp	
			st  z+,temp	
			st  z+,temp	
			st  z+,temp	
			st  z+,temp	
			st  z+,temp	
			st  z+,temp	
			st  z,temp	

			
			ldi 	temp,0			
			mov 	VK0,temp
			mov 	VKP,temp

			inc 	temp
			mov 	VK1,temp

			inc 	temp
			mov 	VK2,temp

			inc 	temp
			mov 	VK3,temp

			ldi 	temp,0b00100000
			out 	MCUCR,temp

			sei     ; Enable interrupts

loopm:

			
			ldi 	zl,low( key1cnt )
			ldi 	zh,high( key1cnt )
			ld  	temp,z
			cpi 	temp,30
			brne 	after_check_key0
			ldi 	temp,0
			;action of the press
			inc 	VK0	
after_check_key0:
			st 		z+,temp
 			
			ld  	temp,z
			cpi 	temp,30
			brne 	after_check_key1
			ldi 	temp,0
			;action of the press
			inc 	VK1	
after_check_key1:
			st 		z+,temp

			ld  	temp,z
			cpi 	temp,30
			brne 	after_check_key2
			ldi 	temp,0
			;action of the press
			inc 	VK2	
after_check_key2:
			st 		z+,temp

			ld  	temp,z
			cpi 	temp,30
			brne 	after_check_key3
			ldi 	temp,0
			;action of the press
			inc 	VK3	
after_check_key3:
			st 		z+,temp



			mov 	R16,vk0
			ldi 	R17,0
			rcall	put_char_pos	

			mov 	R16,VK1
			ldi 	R17,1
			rcall	put_char_pos	

			mov 	R16,VK2
			ldi 	R17,2
			rcall	put_char_pos	

			mov 	R16,VK3
			ldi 	R17,3
			rcall	put_char_pos	



;			rcall	wait
			sleep

			rjmp	loopm

wait:		ldi		var1, 0xFF
lp1_wait:	ldi		var2, 0xFF
lp2_wait:	dec		var2
			brne	lp2_wait
			dec		var1
			brne	lp1_wait
			ret

