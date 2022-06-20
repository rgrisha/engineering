.include "../../def_files_nodefs/m88def.inc"



.CSEG

rjmp prog_main ; Reset Handler
rjmp irq0_isr ; IRQ0 Handler
reti ; IRQ1 Handler
reti ; PCINT0 Handler
reti ; PCINT1 Handler
reti ; PCINT2 Handler
reti ; Watchdog Timer Handler
reti ; Timer2 Compare A Handler
reti ; Timer2 Compare B Handler
reti ; Timer2 Overflow Handler
reti ; Timer1 Capture Handler
reti ; Timer1 Compare A Handler
reti ; Timer1 Compare B Handler
reti ; Timer1 Overflow Handler
reti ; Timer0 Compare A Handler
reti ; Timer0 Compare B Handler
rjmp timer0_isr ; Timer0 Overflow Handler
reti ; SPI Transfer Complete Handler
reti ; USART, RX Complete Handler
reti ; USART, UDR Empty Handler
reti ; USART, TX Complete Handler
reti ; ADC Conversion Complete Handler
reti ; EEPROM Ready Handler
reti ; Analog Comparator Handler
reti ; 2-wire Serial Interface Handler
reti ; Store Program Memory Ready Handler



;.equ t0_c = -2
;.equ t0_c = -8
.def dispout = r24

; r0 - r4 occupied by pwm

;.include "../../irprot_rc5/irprot_rc5_test.inc"
.include "../../irprot_rc5/irprot_rc5.inc"

;reload_timer0:
;	ldi r16,t0_c ;  64 us * t0_c
;	out TCNT0, r16
;ret

timer0_isr:
	push r16
	in r16, sreg
	push r16

	;rcall reload_timer0

	rcall irprot_on_timer

	pop r16
	out sreg, r16
	pop r16
reti

irq0_isr:
	push r16
	in r16, sreg
	push r16

	rcall irprot_isr0

	pop r16
	out sreg, r16
	pop r16

reti

.include "../../n3510i/lcd3510i.inc"
.include "../../n3510i/chargen.inc"
.include "pwm.inc"

prog_main:
	cli
	ldi r16,high(RAMEND); Main program start
	out SPH,r16 ; Set Stack Pointer to top of RAM
	ldi r16,low(RAMEND)
	out SPL,r16

	;ldi r16, 0b00101110
	;out DDRB, r16 ; 


	; set in pwm.inc
	;ldi r16,0x04 ; clk / 256 - expected 32us * (-t0_c + 1) ~ 64us
	;out TCCR0, r16

	;rcall reload_timer0

	ldi r16, (1<<TOIE0)
	sts TIMSK0, r16

	;ldi r16, 0b10001010 ; enable sleep, idle, interrupts on falling edge
	;ldi r16, 0b10001001 ; enable sleep, idle, interrupts on any edge
	;out MCUCR, r16

	ldi r16, (1<<ISC00) ; int0 on any edge
	sts EICRA, r16
	ldi r16, (1<<SE) ; enable sleep
	out SMCR, r16



	rcall lcd_init
	rcall ir_prepare_port_pin

	ldi r20,1
	ldi r21,6
	ldi r22,1
	ldi r23,20


	ldi r25, 0
	ldi r24, 10
	ldi r23, 5
	ldi r22, 10
	ldi r21, 0b11111100
	rcall color_square_rel

	ldi r25, 11
	ldi r24, 10
	ldi r23, 5
	ldi r22, 20
	ldi r21,0b00011111
	rcall color_square_rel


	ldi r20,1
	ldi r21,6
	ldi r22, 0b00000000
	ldi r23, 0b11100011
	ldi r24, 'S'
	rcall put_char

	ldi r20,5
	ldi r24, 'C'
	rcall put_char

	ldi r21, 5
	ldi r20, 1
	ldi r24, 'M'
	rcall put_char

	ldi r20,5
	ldi r24, 'L'
	rcall put_char

	ldi r20, 1
	ldi r21, 4
	ldi r24, 'P'
	rcall put_char

	ldi r20, 5
	ldi r21, 4
	ldi r24, 'R'
	rcall put_char

	rcall irprot_reinit
	rcall irprot_clean_result

	ldi r16,0 ; clearing interrupt flags
	out EIFR, r16

	ldi r16, 1 << INT0 ; enable int0
	out EIMSK, r16

	clr r0

	rcall init_pwm

	ldi r16, 0x00
	mov r_chan0, r16
	mov r_chan1, r16
	mov r_chan2, r16
	mov r_chan3, r16
	mov r_chan4, r16

	clr r6

	sei

mainloop:	

	;tst r_chan0
	;brne mainloop_1

	;rcall stupid_wait_sleep

	load_ir_sram_var ir_var_status
	mov r24, r17
	ldi r20,2
	ldi r21,6
	rcall put_char_hex

	load_ir_sram_var ir_var_calibr
	mov r24, r17
	ldi r20,6
	rcall put_char_hex

	ldi r20,2
	ldi r21,5
	load_ir_sram_var ir_var_result2
	mov r24, r17
	rcall put_char_hex
	
	load_ir_sram_var ir_var_result1
	mov r24, r17
	ldi r20,6
	rcall put_char_hex

	load_ir_sram_var ir_var_bitcnt
	ldi r20, 2
	ldi r21, 4
	mov r24, r_chan0
	rcall put_char_hex

	rcall irprot_read_result
	brtc irprot_no_result

	inc r6
	ldi r20, 0x20 ; ch-up
	cpse r20, r17
	dec r6

	dec r6
	ldi r20, 0x21 ; ch-down
	cpse r20, r17
	inc r6
	
	ldi r20, 31
	sbrc r6, 6 ; correct > 31
	mov r6, r20

	sbrc r6, 7 ; correct minus
	clr r6

	mov r5, r6
	rcall brightness_to_pwm

	mov r_chan0, r5
	mov r_chan1, r5
	mov r_chan2, r5
	mov r_chan3, r5

	; correction for channel 4 - side led's
	mov r_chan4, r5
	ldi r20, 0xFF
	sub r20, r_chan4
	mov r_chan4, r20

	rcall write_pwm_compare_value


irprot_no_result:
	ldi r20, 6
	ldi r21, 4
	mov r24, r0
	rcall put_char_hex


	rjmp mainloop


stupid_wait_sleep:
	push r16
	push r17
	ldi r16, 0xA
	ldi r17, 0xFF
stupid_wait_sleep_loop1:
	sleep
	dec r17
	brne stupid_wait_sleep_loop1
	dec r16
	brne stupid_wait_sleep_loop1

	pop r17
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
