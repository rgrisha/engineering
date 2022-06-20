.include "m8def.inc"



.CSEG

rjmp prog_main ; Reset Handler
rjmp irq0_isr ; IRQ0 Handler
reti ; IRQ1 Handler
reti ; Timer2 Compare Handler
reti ; Timer2 Overflow Handler
reti ; Timer1 Capture Handler
reti ; Timer1 CompareA Handler
reti ; Timer1 CompareB Handler
reti ; Timer1 Overflow Handler
rjmp timer0_isr ; Timer0 Overflow Handler
reti ; SPI Transfer Complete Handler
reti ; USART RX Complete Handler
reti ; UDR Empty Handler
reti ; USART TX Complete Handler
reti ; ADC Conversion Complete Handler
reti ; EEPROM Ready Handler
reti ; Analog Comparator Handler
reti ; Two-wire Serial Interface Handler
reti ; Store Program Memory Ready Handler

.equ t0_c = -2
;.equ t0_c = -8
.def dispout = r24


; config: 
; PORTB 2 (PIN 16) - SDATA (4)
; PORTB 5 (PIN 19) - RESET (1)
; PORTB 1 (PIN 15) - CS (2)
; PORTB 3 (PIN 17) - SCLK

.include "irprot.inc"

reload_timer0:
	ldi r16,t0_c ;  64 us * t0_c
	out TCNT0, r16
ret

timer0_isr:
	push r16
	in r16, sreg
	push r16

	rcall reload_timer0

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


.include "lcd3510i.inc"
.include "chargen.inc"

prog_main:
	cli
	ldi r16,high(RAMEND); Main program start
	out SPH,r16 ; Set Stack Pointer to top of RAM
	ldi r16,low(RAMEND)
	out SPL,r16

	;ldi r16, 0b00101110
	;out DDRB, r16 ; 


	ldi r16,0x04 ; clk / 256 - expected 32us * (-t0_c + 1) ~ 64us
	out TCCR0, r16

	rcall reload_timer0

	ldi r16, (1<<TOIE0)
	out TIMSK, r16

	;ldi r16, 0b10001010 ; enable sleep, idle, interrupts on falling edge
	ldi r16, 0b10001001 ; enable sleep, idle, interrupts on any edge
	out MCUCR, r16



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

	rcall irprot_reinit
	rcall irprot_clean_result

	ldi r16,0 ; clearing interrupt flags
	out GIFR, r16

	ldi r16, 0b01000000 ; enable int0
	out GICR, r16

	sei

mainloop:	

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

	rjmp mainloop


