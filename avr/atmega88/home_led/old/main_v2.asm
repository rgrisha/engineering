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

	ldi r16, 1
	save_pwm_sram_value r16, pwm_var_status
	mov r14, r16 ; simulate key 1 press - starting with preset 1
	mov r15, r16 
	rjmp check_digit_key

mainloop:	

	sleep
	rcall irprot_read_result
	rcall show_vars_to_lcd
	brtc mainloop

	load_y_z_regs

	; r16 - key
	; r17 - repetitions

	mov r14, r16
	mov r15, r17

	cpi r16, 0x0C ; off key
	brne check_key_chup 
	ldi r16, 0
	save_new_pwm_channel r16, 0 
	save_new_pwm_channel r16, 1 
	save_new_pwm_channel r16, 2 
	save_new_pwm_channel r16, 3 
	save_new_pwm_channel r16, 4 

	rcall do_smooth_pwm_change
	; todo here - put power to sleep
	rjmp mainloop

check_key_chup:
	ldi r16, 0x20
	cp r14, r16
	brne check_key_chdown

	load_current_pwm_channel r0, 0
	rcall inc_checked
	load_current_pwm_channel r0, 1
	rcall inc_checked
	load_current_pwm_channel r0, 2
	rcall inc_checked
	load_current_pwm_channel r0, 3
	rcall inc_checked
	load_current_pwm_channel r0, 4
	rcall inc_checked

	rcall write_pwm_compare_value

check_key_chdown:
	ldi r16, 0x21
	cp r14, r16
	brne check_key_left

	load_current_pwm_channel r0, 0
	rcall dec_checked
	load_current_pwm_channel r0, 1
	rcall dec_checked
	load_current_pwm_channel r0, 2
	rcall dec_checked
	load_current_pwm_channel r0, 3
	rcall dec_checked
	load_current_pwm_channel r0, 4
	rcall dec_checked

	rcall write_pwm_compare_value

check_key_left:
	ldi r16, 0x11
	cp r14, r16
	brne check_key_right

	ldi r16, 0
	save_pwm_sram_value r16, pwm_var_preset 

	load_pwm_sram_value r16, pwm_var_status
	cpi r16, 2
	brne check_key_left_start_adjust

	load_pwm_sram_value r0, pwm_var_current_adj_chan
	ldi r16, 0
	ldi r17, 4
	rcall dec_in_loop
	save_pwm_sram_value r0, pwm_var_current_adj_chan

	rjmp check_key_right
check_key_left_start_adjust:
	ldi r16, 2
	save_pwm_sram_value r16, pwm_var_status
	ldi r16, 1
	save_pwm_sram_value r16, pwm_var_current_adj_chan


check_key_right:
	ldi r16, 0x11
	cp r14, r16
	brne check_digit_key

	ldi r16, 0
	save_pwm_sram_value r16, pwm_var_preset 

	load_pwm_sram_value r16, pwm_var_status
	cpi r16, 2
	brne check_key_right_start_adjust

	load_pwm_sram_value r0, pwm_var_current_adj_chan
	ldi r16, 0
	ldi r17, 4
	rcall inc_in_loop
	save_pwm_sram_value r0, pwm_var_current_adj_chan

	rjmp check_digit_key
check_key_right_start_adjust:
	ldi r16, 2
	save_pwm_sram_value r16, pwm_var_status
	ldi r16, 1
	save_pwm_sram_value r16, pwm_var_current_adj_chan

check_digit_key:
	mov r16, r14
	cpi r16, 10
	brsh check_key_end

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
	lsl r16
	lsl r16 ; multiply address to 4 - rough address calculation
	
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
		
	rcall do_smooth_pwm_change

check_key_end:
	ldi r16, 1
	save_pwm_sram_value r16, pwm_var_status
	save_pwm_sram_value r14, pwm_var_preset
	
	rjmp mainloop

; if r0 is between 2 and 31, dec it
dec_checked:
	push r16

	ldi r16, 2
	cp r0, r16
	brlo dec_checked_end
	ldi r16, 32
	cp r0, r16
	brsh dec_checked_end

	dec r0
	
dec_checked_end:
	pop r16
	ret

do_smooth_pwm_change:
	push r16
	push r17
	push r18
	push r0
	push r1
	
do_smooth_pwm_change_loop_h:
	ldi r17, 0
	load_y_z_regs
	ldi r16, 4
do_smooth_pwm_change_loop:
	ld r0, Y
	ld r1, Z

	cp r0, r1
	breq do_smooth_pwm_change_loop_end

	brlo do_smooth_pwm_change_loop_if_lo
	; here if r0 > r1
	dec r0
	inc r17
	st Y, r0
	rjmp do_smooth_pwm_change_loop_end

do_smooth_pwm_change_loop_if_lo:
	; here if r0 < r1
	inc r0
	inc r17
	st Y, r0

do_smooth_pwm_change_loop_end:
	adiw yh:yl, 1
	adiw zh:zl, 1
	dec r16
	brne do_smooth_pwm_change_loop

	tst r17 ; how many channels are not equal to new values 
	breq do_smooth_pwm_change_end_end

	rcall write_pwm_compare_value
	rcall show_vars_to_lcd
	rcall stupid_wait_sleep
	rjmp do_smooth_pwm_change_loop_h

do_smooth_pwm_change_end_end:
	load_y_z_regs

	pop r1
	pop r0
	pop r18
	pop r17
	pop r16
	
ret


; if r0 is between 1 and 30, inc it
inc_checked:
	push r16
	ldi r16, 1
	cp r0, r16
	brlo inc_checked_end

	ldi r16, 31
	cp r0, r16
	brsh inc_checked_end

	; between 1 and 30 here
	inc r0

inc_checked_end:
	pop r16
	ret

; decrement r0 in a loop
; loop boundaries : low r16, high - r17
dec_in_loop:
	cp r0, r16
	brne dec_in_loop_dec

	mov r0, r17
	rjmp dec_in_loop_end

dec_in_loop_dec:
	dec r0
dec_in_loop_end:
	ret

; increment r0 in a loop
; loop boundaries : low r16, high - r17
inc_in_loop:
	cp r0, r17
	brne inc_in_loop_inc

	mov r0, r16
	rjmp inc_in_loop_end

inc_in_loop_inc:
	inc r0
inc_in_loop_end:
	ret

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

eeprom_write:
	; Wait for completion of previous write
	sbic EECR,EEPE
	rjmp eeprom_write
	; Set up address (r18:r17) in address register
	out EEARH, r1
	out EEARL, r0
	; Write data (r16) to Data Register
	out EEDR,r16
	; Write logical one to EEMPE
	sbi EECR,EEMPE
	; Start eeprom write by setting EEPE
	sbi EECR,EEPE
	ret

eeprom_read:
	; Wait for completion of previous write
	sbic EECR,EEPE
	rjmp EEPROM_read
	; Set up address (r18:r17) in address register
	out EEARH, r1
	out EEARL, r0
	; Start eeprom read by writing EERE
	sbi EECR,EERE
	; Read data from Data Register
	in r16,EEDR
ret

show_vars_to_lcd:
	ldi reg_pos_y,0
	ldi reg_pos_x,1
	ldi r22, 0b00000000
	ldi r23, 0b00111000
	load_current_pwm_channel r24, 0
	ldi r24, '0'
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
	ldi reg_pos_x,2
	load_current_pwm_channel r24, 3
	rcall put_char_hex

	ldi reg_pos_y,2
	ldi reg_pos_x,6
	load_current_pwm_channel r24, 4
	rcall put_char_hex
ret

