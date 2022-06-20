
.equ pwm0_port     = PORTD
.equ pwm0_port_ddr = DDRD
.equ pwm0_port_pin_mask = (1<<6)+(1<<3)

.equ pwm1_port     = PORTB
.equ pwm1_port_ddr = DDRB
.equ pwm1_port_pin_mask = (1<<3)+(1<<1)+(1<<2)

.equ enable_port = PORTB
.equ enable_port_DDR = DDRB
.equ enable_chan_1_bit = PB6
.equ enable_chan_2_bit = PB7



.DSEG
pwm_vars: .BYTE  5  ; PWM channel variables - current
pwm_vars_new: .BYTE 5 ; PWM channel variables - new

main_status: .BYTE 1 ; current status
; 0 - startup
; 1 - in preset
; 2 - channel adjust

current_preset: .BYTE 1 ; number of current preset
current_adj_chan: .BYTE 1 ; channel currently adjusted

.EQU pwm_var_status = main_status - pwm_vars_new
.EQU pwm_var_preset = current_preset - pwm_vars_new
.EQU pwm_var_current_adj_chan = current_adj_chan - pwm_vars_new

; Y - current pwm vals
; Z - new pwm vals

.MACRO load_y_pwm_current
	ldi yh, high( pwm_vars ) 
	ldi yl, low( pwm_vars ) 
.ENDMACRO

.MACRO load_y_pwm_new
	ldi yh, high( pwm_vars_new ) 
	ldi yl, low( pwm_vars_new ) 
.ENDMACRO

.MACRO load_pwm_sram_value
	ldd @0, Y+@1
.ENDMACRO

.MACRO save_pwm_sram_value
	std Y+@1, @0
.ENDMACRO

.MACRO save_pwm_sram_value_with_postinc 
	st Y+, @0
.ENDMACRO

; .MACRO load_current_pwm_channel
;  	ldd @0, X+@1
; .ENDMACRO

;.MACRO save_current_pwm_channel
;	std X+@1, @0
;.ENDMACRO

;.MACRO load_new_pwm_channel
;	ldd @0, Y+@1
;.ENDMACRO

;.MACRO save_new_pwm_channel
;	std Y+@1, @0
;.ENDMACRO

.CSEG

init_pwm:
	push r16

	ldi r16, pwm0_port_pin_mask
	out pwm0_port_ddr, r16
	out pwm0_port, r16

	ldi r16, pwm1_port_pin_mask
	out pwm1_port_ddr, r16
	out pwm1_port, r16
	
	; setup timer 0
	;ldi r16, (1<<WGM01)|(1<<WGM00)|(1<<COM0A1) ; fast pwm
	ldi r16, (1<<WGM00)|(1<<COM0A1) ; phase correct pwm
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

	;	non pwm - setting enable power supply outputs
	in r16, enable_port_ddr
	ori r16, (1 << enable_chan_1_bit) | (1 << enable_chan_2_bit) 
	out enable_port_ddr, r16

	pop r16

ret

write_pwm_compare_value:
	push r16
	push r18
	push r0
	push zh 
	push zl

	; Save Global Interrupt Flag
	in r18,SREG
	cli

	ldi zl, low( pwm_vars )
	ldi zh, high( pwm_vars )

	; channel 0 - side LED's
	ld r0, Z+
	rcall brightness_to_pwm ; convert to non linear value
	out OCR0A, r0
	nop
	nop
	nop
	nop

	; channel 1
	ld r0, Z+
	rcall brightness_to_pwm ; convert to non linear value
	ldi r16,0	
	sts OCR1AH,r16
	sts OCR1AL,r0
	nop
	nop
	nop
	nop

	; channel 2
	ld r0, Z+
	rcall brightness_to_pwm ; convert to non linear value
	sts OCR1BH,r16
	sts OCR1BL,r0
	nop
	nop
	nop
	nop


	; channel 3
	ld r0, Z+
	rcall brightness_to_pwm ; convert to non linear value
	sts OCR2A,r0
	nop
	nop
	nop
	nop

	; channel 4
	ld r0, Z+
	rcall brightness_to_pwm ; convert to non linear value
	sts OCR2B,r0
	nop
	nop
	nop
	nop


	; restore global int flag
	out SREG,r18	
	sei

	pop zl
	pop zh
	pop r0
	pop r18
	pop r16
ret

;takes 0-31 code in r0 of brightness and returns value for pwm
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
	and r0, zl 

	ldi zh, high( brightness_to_pwm_table * 2 )
	ldi zl, low( brightness_to_pwm_table * 2 )

	add zl, r0
	clr r0
	adc zh, r0 ; adds carry if there's such

	lpm r0, Z

	pop zh
	pop zl
	ret

