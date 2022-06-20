.include "../../def_files_nodefs/m88def.inc"

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


;.def dispout = r24

; r0 - r4, r6-r10 occupied by pwm

.include "../../irprot_rc5/irprot_rc5.inc"


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

	;ldi r25, 0
	;ldi r24, 10
	;ldi r23, 5
	;ldi r22, 10
	;ldi r21, 0b11111100
	;rcall color_square_rel

	;ldi r25, 11
	;ldi r24, 10
	;ldi r23, 5
	;ldi r22, 20
	;ldi r21,0b00011111
	;rcall color_square_rel


	ldi reg_pos_y,0
	ldi reg_pos_x,0
	ldi r22, 0b00000000
	ldi r23, 0b11100011
	ldi r24, '0'
	rcall put_char

	ldi reg_pos_y,0
	ldi reg_pos_x,5
	ldi r24, 'S'
	rcall put_char

	ldi reg_pos_y,1
	ldi reg_pos_x,0
	ldi r24, '1'
	rcall put_char

	ldi reg_pos_y,1
	ldi reg_pos_x,5
	ldi r24, '2'
	rcall put_char

	ldi reg_pos_y,2
	ldi reg_pos_x,0
	ldi r24, '3'
	rcall put_char

	ldi reg_pos_y,2
	ldi reg_pos_x,5
	ldi r24, '4'
	rcall put_char


	rcall irprot_reinit
	rcall irprot_clean_result

	ldi r16,0 ; clearing interrupt flags
	out EIFR, r16

	ldi r16, 1 << INT0 ; enable int0
	out EIMSK, r16

	clr r0

	rcall init_pwm

	sei

	load_y_z_regs
	ldi r16, 1
	save_pwm_sram_value r16, pwm_var_status
	ldi r16, 1
	save_current_pwm_channel r16, 0
	save_current_pwm_channel r16, 1
	save_current_pwm_channel r16, 2
	save_current_pwm_channel r16, 3
	save_current_pwm_channel r16, 4

	mov r15, r16 
	ldi r16, 0
	mov r14, r16 ; simulate key 1 press - starting with preset 1

	rjmp loop_after_irprot_read

.include "utils.inc"

mainloop:	

	rcall show_vars_to_lcd
	sleep
	rcall irprot_read_result
	brtc mainloop
	; r16 - key
	; r17 - repetitions
	mov r14, r16
	mov r15, r17

	clr r2
	rcall switch_power_enable

loop_after_irprot_read:

	load_y_z_regs

	mov r16, r14

	cpi r16, 0x0C ; off key
	brne check_key_chup 
	ldi r16, 0
	save_new_pwm_channel r16, 0 
	save_new_pwm_channel r16, 1 
	save_new_pwm_channel r16, 2 
	save_new_pwm_channel r16, 3 
	save_new_pwm_channel r16, 4 

	;rcall do_smooth_pwm_transition

	clr r2
	rcall switch_power_enable

	; todo here - put power to sleep
	rjmp mainloop


check_key_chup:
	ldi r16, 0x20
	cp r14, r16
	brne check_key_chdown

	load_current_pwm_channel r0, 0
	rcall inc_checked
	save_current_pwm_channel r0, 0
	load_current_pwm_channel r0, 1
	rcall inc_checked
	save_current_pwm_channel r0, 1
	load_current_pwm_channel r0, 2
	rcall inc_checked
	save_current_pwm_channel r0, 2
	load_current_pwm_channel r0, 3
	rcall inc_checked
	save_current_pwm_channel r0, 3
	load_current_pwm_channel r0, 4
	rcall inc_checked
	save_current_pwm_channel r0, 4

	rcall write_pwm_compare_value
	rjmp mainloop

check_key_chdown:
	ldi r16, 0x21
	cp r14, r16
	brne check_key_left_1

	load_current_pwm_channel r0, 0
	rcall dec_checked
	save_current_pwm_channel r0, 0
	load_current_pwm_channel r0, 1
	rcall dec_checked
	save_current_pwm_channel r0, 1
	load_current_pwm_channel r0, 2
	rcall dec_checked
	save_current_pwm_channel r0, 2
	load_current_pwm_channel r0, 3
	rcall dec_checked
	save_current_pwm_channel r0, 3
	load_current_pwm_channel r0, 4
	rcall dec_checked
	save_current_pwm_channel r0, 4

	rcall write_pwm_compare_value
	rjmp mainloop

check_key_left_1:
check_digit_key:
	mov r16, r14
	cpi r16, 10
	brsh check_key_end

	clr r2
	dec r2 ;  ser r2
	rcall switch_power_enable ;enable power supply

	ldi r16, 1
	save_pwm_sram_value r16, pwm_var_status

	mov r16, r15
	cpi r16, 9
	brlo check_digit_key_load_preset

	; todo here - save preset

check_digit_key_load_preset:
	ldi r16, 0
	mov r1, r16 ; eeprom hi addr
	mov r0, r14 ; key code
	lsl r0
	lsl r0
	lsl r0 ; multiply address by 8 - rough address calculation
	
	rcall eeprom_read
	save_new_pwm_channel r16, 0

	inc r0
	rcall eeprom_read
	save_new_pwm_channel r16, 1

	inc r0
	rcall eeprom_read
	save_new_pwm_channel r16, 2

	inc r0
	rcall eeprom_read
	save_new_pwm_channel r16, 3

	inc r0
	rcall eeprom_read
	save_new_pwm_channel r16, 4
		
;	rcall do_smooth_pwm_transition

check_key_end:
	ldi r16, 1
	save_pwm_sram_value r16, pwm_var_status
	save_pwm_sram_value r14, pwm_var_preset

	rjmp mainloop

num_keys_table:


do_smooth_pwm_transition:
	push r16
	push r2
	push r0
	push r1

do_smooth_transition_loop2:

	load_y_z_regs
	clr r2
	ldi r16, 5
do_smooth_transition_loop1:
	ld r0, Y
	ld r1, Z

	cp r0, r1
	breq do_smooth_transition_chan_done
	brlo do_smooth_transition_when_less
	; here if r0 > r1
	inc r2
	dec r0
	st Y, r0
	rjmp do_smooth_transition_chan_done

do_smooth_transition_when_less:
	inc r2
	inc r0
	st Y, r0

do_smooth_transition_chan_done:
	adiw zh:zl,1
	adiw yh:yl,1
	dec r16
	brne do_smooth_transition_loop1

	rcall show_vars_to_lcd
	rcall stupid_wait_sleep
	rcall write_pwm_compare_value

	tst r2
	brne do_smooth_transition_loop2 

	load_y_z_regs

	pop r1
	pop r0
	pop r2
	pop r16
	
ret



stupid_wait_sleep:
	push r18
	push r19

	ldi r18, 0x05
stupid_wait_sleep_loop1:
	ldi r19, 0xFF
stupid_wait_sleep_loop2:
	sleep
	dec r19
	tst r19
	brne stupid_wait_sleep_loop2
	dec r18
	tst r18
	brne stupid_wait_sleep_loop1

	pop r19
	pop r18

		
	ret


show_vars_to_lcd:
	ldi reg_pos_y,0
	ldi reg_pos_x,1
	ldi r22, 0b00000000
	ldi r23, 0b00111000
	load_current_pwm_channel r24, 0
	rcall put_char_hex

	ldi reg_pos_y,0
	ldi reg_pos_x,6
	load_pwm_sram_value r24, pwm_var_status
	rcall put_char_hex

	ldi reg_pos_y,1
	ldi reg_pos_x,1
	load_current_pwm_channel r24, 1
	rcall put_char_hex

	ldi reg_pos_y,1
	ldi reg_pos_x,6
	load_current_pwm_channel r24, 2
	rcall put_char_hex

	ldi reg_pos_y,2
	ldi reg_pos_x,1
	load_current_pwm_channel r24, 3
	rcall put_char_hex

	ldi reg_pos_y,2
	ldi reg_pos_x,6
	load_current_pwm_channel r24, 4
	rcall put_char_hex
ret

; if r2 is non zero, will always switch on
switch_power_enable:
	push r16
	push r17
	push r0
	push r1

	in r17, enable_port
	; zero enable port bits
	andi r17, 0xFF - (1 << enable_chan_1_bit) - (1 << enable_chan_2_bit) 

	clr r1
	clr r0
	load_current_pwm_channel r16, 0
	or r0, r16
	load_current_pwm_channel r16, 1
	or r0, r16
	load_current_pwm_channel r16, 2
	or r0, r16
	or r0, r2
	cpse r0, r1
	ori r17, 1 << enable_chan_2_bit

	clr r0
	load_current_pwm_channel r16, 3
	or r0, r16
	load_current_pwm_channel r16, 4
	or r0, r16
	or r0, r2
	cpse r0, r1
	ori r17, 1 << enable_chan_1_bit

	out enable_port, r17

	rcall stupid_wait_sleep

	pop r1
	pop r0
	pop r17
	pop r16
ret
