

.EQU nok_cs=1
.EQU nok_clk=3
.EQU nok_sio=2
.EQU nok_reset=5
.EQU nok_port=PORTB
.EQU carry_bit=0

; Nokia 3510i lcd driving lib
; depends on base.inc

; Raises CS
lcd_3510i_raise_cs:
	sbi   nok_port,nok_cs
	ret

lcd_3510i_hw_reset:
	cbi   nok_port,nok_reset
	rcall wait_16th_sec
	sbi   nok_port,nok_reset
	rcall wait_16th_sec

	sbi   nok_port,nok_cs
	sbi   nok_port,nok_clk

	ret


; Write out command
; Expects command code on R16
lcd_3510i_command:

	push  R17
	push  R2
	push  R3

; CS=PB1=0
	cbi   nok_port,nok_cs
; CLK=PB3=0
	cbi   nok_port,nok_clk
; Zero for command
	cbi   nok_port,nok_sio
;   raise clock
	sbi   nok_port,nok_clk

	ldi   R17,0x80
	mov   R3,R16


lcd_3510i_cmd_loop_main:

;   low clock
	cbi   nok_port,nok_clk

	mov   R2,R17		
	and   R2,R3

	breq  lcd_3510i_cmd_set_sio	
	sbi   nok_port,nok_sio
	rjmp lcd_3510i_cmd_set_sio_cont

lcd_3510i_cmd_set_sio:
	cbi   nok_port,nok_sio

lcd_3510i_cmd_set_sio_cont:


;   raise clock
	sbi   nok_port,nok_clk

	lsr   R17
	brne  lcd_3510i_cmd_loop_main

;   raise cs
	sbi   nok_port,nok_cs


	pop   R3
	pop   R2
	pop   R17

	ret



; Write out parameter
; Expects command code on R16
; Does not raise CS signal (PIN 1 port B ) !!!!!!!!!!!!
lcd_3510i_param:

	push  R17
	push  R2
	push  R3

; CS=PB1=0
	cbi   nok_port,nok_cs
; CLK=PB3=0
	cbi   nok_port,nok_clk
; 1 for parameter
	sbi   nok_port,nok_sio
;   raise clock
	sbi   nok_port,nok_clk

	ldi   R17,0x80
	mov   R3,R16

lcd_3510i_prm_loop_main:

;   low clock
	cbi   nok_port,nok_clk

	mov   R2,R17
	and   R2,R3

	breq  lcd_3510i_prm_low
	rjmp lcd_3510i_prm_low
	sbi   nok_port,nok_sio

	rjmp lcd_3510i_prm_low_cont

lcd_3510i_prm_low:
	cbi   nok_port,nok_sio

lcd_3510i_prm_low_cont:


;   raise clock
	sbi   nok_port,nok_clk

	lsr   R17
	brne  lcd_3510i_prm_loop_main

;   raise cs
;   as there can be next prm, cs should not be raised
;	sbi   nok_port,nok_cs

	pop   R3
	pop   R2
	pop   R17

	ret



