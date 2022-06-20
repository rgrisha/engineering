
; definitions
;.DEVICE ATtiny26
.DEF	ledchar = R16
.DEF	ledpos = R17
.DEF	vartmp = R18
	

;led_char_bits:
;.DB 0x77, 0x24, 0x5D, 0x6D, 0x2E, 0x6B, 0x7A, 0x25, 0x7F, 0x2F


put_char_pos:
; converts position to videomem bits and position
; parameters: 
; R16 - character to put
; 	least 4 bits address character
; 	bit 5 is decimal point
; 	bits 6 and 7 are blinking bits
; R17 - position 
			push 	vartmp
			push	zh
			push 	zl
			push 	R0

			ldi 	vartmp,0x00
			mov 	R0,vartmp
			mov  	vartmp, ledchar
			andi 	vartmp, 0x0F ; to mask char position
			ldi 	zh, high( led_char_bits * 2 )
			ldi 	zl, low( led_char_bits * 2 )
			add		zl, vartmp
			adc 	zh,R0 ; inc zh if carry

;			brcc    end_led_addr_c
;			inc		zh ; if was carry, inc high reg
;end_led_addr_c:

			lpm
			ldi		vartmp,0x10 ; test 5th bit
			and		vartmp,ledchar
			lsl		vartmp
			lsl		vartmp
			or		vartmp, R0
			com		vartmp ; now we have full char bits in vartmp

			;lower blinking bits
			lsr		ledchar					
			lsr		ledchar					
			lsr		ledchar					
			lsr		ledchar					
			lsr		ledchar					
			lsr		ledchar					
			
			ldi 	zl,0x00
			mov 	R0,zl
			ldi		zh, high( videomem )
			ldi		zl, low( videomem )
			lsl 	ledpos ; as there are 2 bytes per char
			add		zl, ledpos	
			adc 	zh,R0  ; inc zh if carry
;			brcc    end_video_addr1
;			inc		zh ; if was carry, inc high reg
; end_video_addr1:
			st      z+,vartmp
			st		z,ledchar

			pop 	R0
			pop 	zl
			pop 	zh
			pop 	vartmp

			ret
		


