
.equ pwm0_port     = PORTD
.equ pwm0_port_ddr = DDRD
.equ pwm0_port_pin_mask = (1<<6)+(1<<3)

.equ pwm1_port     = PORTB
.equ pwm1_port_ddr = DDRB
.equ pwm1_port_pin_mask = (1<<3)+(1<<1)+(1<<2)


.def r_chan0 = r0
.def r_chan1 = r1
.def r_chan2 = r2
.def r_chan3 = r3
.def r_chan4 = r4

init_pwm:
	push r16

	ldi r16, pwm0_port_pin_mask
	out pwm0_port_ddr, r16
	out pwm0_port, r16

	ldi r16, pwm1_port_pin_mask
	out pwm1_port_ddr, r16
	out pwm1_port, r16
	
	; setup timer 0
	ldi r16, (1<<WGM01)|(1<<WGM00)|(1<<COM0A1) ; fast pwm
	out TCCR0A, r16
	
	ldi r16, 0x01 ; direct clock
	;ldi r16, 0x03 ; prescaler 1/64
	;ldi r16, 0x04 ; prescaler 1/256
	out TCCR0B, r16

	; setup timer 1
	; pwm phase correct, 8 bit 
	ldi r16, (1<<WGM10)|(1<<COM1A1)|(1<<COM1B1) ; fast pwm
	sts TCCR1A, r16

	ldi r16, 3 ; prescaler 1/64
	sts TCCR1B, r16

	; setup timer 2
	ldi r16, (1<<WGM20)|(1<<COM2A1)|(1<<COM2B1) ; fast pwm
	sts TCCR2A, r16

	ldi r16, 4 ; prescaler 1/64
	sts TCCR2B, r16

	pop r16

ret

write_pwm_compare_value:
	push r16
	push r18
	push r5

	; Save Global Interrupt Flag
	in r18,SREG
	cli

	; channel 0
	ldi r16,0	
	sts OCR1AH,r16
	sts OCR1AL,r_chan0
	nop
	nop
	nop
	nop

	; channel 1
	sts OCR1BH,r16
	sts OCR1BL,r_chan1
	nop
	nop
	nop
	nop

	; restore global int flag
	out SREG,r18	

	; channel 2
	sts OCR2A,r_chan2 
	nop
	nop
	nop
	nop

	; channel 3
	sts OCR2B,r_chan3 
	nop
	nop
	nop
	nop

	; channel 4
	out OCR0A, r_chan4
	nop
	nop
	nop
	nop

	pop r5
	pop r18
	pop r16
ret

;takes 0-31 code in r5 of brightness and returns value for pwm
brightness_to_pwm:
	rjmp brightness_to_pwm_table_end

brightness_to_pwm_table:
;	.db 0, 1
;	.db 2, 3
;	.db 4, 5
;	.db 7, 9
;	.db 12, 15
;	.db 18, 22
;	.db 27, 32
;	.db 38, 44
;	.db 51, 58
;	.db 67, 76
;	.db 86, 96
;	.db 108, 120
;	.db 134, 148
;	.db 163, 180
;	.db 197, 216
;	.db 235, 255

	.db 0, 3
	.db 4, 5 
	.db 6, 7
	.db 9, 11
	.db 14, 17
	.db 21, 24
	.db 28, 32
	.db 37, 41
	.db 47, 53
	.db 60, 67
	.db 75, 84
	.db 95, 106
	.db 119, 133
	.db 148, 165
	.db 185, 206
	.db 230, 255


brightness_to_pwm_table_end:
	push zl
	push zh
	ldi zl, 0b00011111
	and r5, zl 

	ldi zh, high( brightness_to_pwm_table * 2 )
	ldi zl, low( brightness_to_pwm_table * 2 )

	add zl, r5
	clr r5
	adc zh, r5 ; adds carry if there's such

	lpm r5, Z

	pop zh
	pop zl
	ret

