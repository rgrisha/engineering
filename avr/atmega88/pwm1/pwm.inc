
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

	out SREG,r18
	pop r18
	pop r16
ret

; r17 - pwm compare value
write_pwm_compare_value_old:
	push r18
	push r16
	; Save Global Interrupt Flag
	in r18,SREG
	; Disable interrupts
	cli
	ldi r16,0	
	sts OCR1AH,r16
	sts OCR1AL,r17
; Restore Global Interrupt Flag
	out SREG,r18
	pop r16
	pop r18
ret

