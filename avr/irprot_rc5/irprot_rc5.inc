; Version 0.2 key result counter
; Improved timer routine

.EQU ir_port            = PORTD   ; which port infrared is connected to
.EQU ir_port_pinreg     = PIND    ; which port infrared is connected to - for reading
.EQU ir_port_ddr        = DDRD    ; DDR reg of infrared port
.EQU ir_port_pin = 2              ; which port pin is connected

.DSEG
ir_vars:    .BYTE 10 ; infrared variables

.EQU   ir_var_status    = 0
.EQU   ir_var_counterl  = 1
.EQU   ir_var_counterh  = 2 
.EQU   ir_var_calibr4   = 3
.EQU   ir_var_presslen  = 4
.EQU   ir_var_calibr    = 5 
.EQU   ir_var_bitcnt    = 6 
.EQU   ir_var_result1   = 7 
.EQU   ir_var_result2   = 8 
.EQU   ir_var_saveresult  = 9 

.EQU   rc5_bitcnt = 0x0C

.DEF   reg_status    = r0
.DEF   reg_counterl  = r1
.DEF   reg_counterh  = r2
.DEF   reg_calibr4   = r3
.DEF   reg_presslen  = r4
.DEF   reg_saveresult  = r6

.CSEG

; requires zl and zh to be saved
; r17 - sram variable value answer
.MACRO load_ir_sram_var
	ldi zl, low( ir_vars + @0 )
	ldi zh, high( ir_vars + @0 )
	ld  r17, Z+
.ENDMACRO

.MACRO save_ir_sram_var
	ldi zl, low( ir_vars + @0 )
	ldi zh, high( ir_vars + @0 )
	ldi r17, @1
	st  Z+, r17
.ENDMACRO

.MACRO save_ir_sram_var_reg
	ldi zl, low( ir_vars + @0 )
	ldi zh, high( ir_vars + @0 )
	st  Z+, @1
.ENDMACRO



ir_prepare_port_pin:
	push r16
	;prepare port D reg, we need d2 pin,int 0

	in r16, ir_port_ddr
	andi r16, ~( 1 << ir_port_pin )
	out ir_port_ddr, r16

	in r16, ir_port
	andi r16, ~( 1 << ir_port_pin )
	out ir_port, r16
	
	pop r16
ret

irprot_clean_result:
	push r17
	push zh
	push zl

	save_ir_sram_var ir_var_result1, 0
	st Z+, r17 ; ir_var_result2 <- 0
	st Z,  r17 ; ir_var_saveresult <- 0

	save_ir_sram_var ir_var_presslen,  0

	pop zl
	pop zh
	pop r17
ret

irprot_clean_result_st4:
	push r17
	push zh
	push zl

	save_ir_sram_var ir_var_result1, 0
	st Z, r17 ; ir_var_result2

	pop zl
	pop zh
	pop r17
ret

; if result OK, sreg T=1 and r16=key r17=repetitions count
; if no result, sreg T=0
irprot_read_result:
	push zh
	push zl

	load_ir_sram_var ir_var_status
	cpi r17,5
	breq irprot_read_result_ok

	clt
	rjmp irprot_read_result_end

irprot_read_result_ok:
	load_ir_sram_var ir_var_result1
	mov r16, r17

	load_ir_sram_var ir_var_presslen

	rcall irprot_reinit
	rcall irprot_clean_result

	set

irprot_read_result_end:
	pop zl
	pop zh
ret


irprot_reinit:
	push r16
	push zh
	push zl

	ldi zl, low(ir_vars)
	ldi zh, high(ir_vars)
	ldi r16, 0

	st Z+, r16 ; status
	st Z+, r16 ; counterl
	st Z+, r16 ; counterh
	st Z+, r16 ; calibr4
	adiw zh:zl, 1  ; skip presslen
	st Z+, r16 ; calibr
	st Z+, r16 ; bit cnt

	pop zl
	pop zh
	pop r16
ret


;called from timer isr, expected ~ each 64us 
irprot_on_timer:
	push reg_status
	push reg_counterl
	push reg_counterh
	push reg_calibr4
	push reg_presslen
	push reg_saveresult

	push zh
	push zl

	push r16

	ldi zl, low( ir_vars )
	ldi zh, high( ir_vars )

	ldd reg_saveresult, Z+ir_var_saveresult

	ld reg_status, Z+
	ld reg_counterl, Z+
	ld reg_counterh, Z+
	ld reg_calibr4, Z+
	ld reg_presslen, Z+

	; inc counter l+h
	ldi r16, 1
	add reg_counterl, r16
	ldi r16, 0
	adc reg_counterh, r16 ; add carry if exists

	save_ir_sram_var_reg ir_var_counterl, reg_counterl
	st Z, reg_counterh ; store counterh

	ldi r16, 0x0F ; more than silent period between key presses
	cp  reg_counterh, r16
	brlo irprot_on_timer_check_st3

	tst reg_presslen
	breq irprot_on_timer_check_st3

	; here we have counterh >= 0x0E and non zero key press length
	rcall irprot_reinit
	save_ir_sram_var ir_var_status, 5	
	rjmp irprot_on_timer_end

irprot_on_timer_check_st3:

	; check if it is OK in status 3
	ldi r16, 3
	cp reg_status, r16

	brne irprot_on_timer_end ; checking here only status 3

	cp reg_counterl, reg_calibr4
	brlo irprot_on_timer_end

	; if counter is bigger than 4 IR tick periods and status still 3, reset irprot - noise
	;rcall irprot_clean_result_st4
	; not clean, but give back prev result ....
	save_ir_sram_var_reg ir_var_result1, reg_saveresult

	rcall irprot_reinit

irprot_on_timer_end:
	pop r16

	pop zl
	pop zh

	pop reg_saveresult
	pop reg_presslen
	pop reg_calibr4
	pop reg_counterh
	pop reg_counterl
	pop reg_status

ret

irprot_isr0:
	push reg_status
	push reg_counterl

	push zh
	push zl

	push r16
	push r17
	push r18

	load_ir_sram_var ir_var_status
	mov reg_status, r17
	tst r17
	brne irprot_isr0_status_1
	; here if status = 0

	sbiw zh:zl, 1 ; pointing back to ir_var_status
	ldi r17, 1 
	st Z+, r17 ; status
	ldi r17, 0
	st Z+, r17 ; counterl
	st Z+, r17 ; counterh

	rjmp irprot_isr0_end

irprot_isr0_status_1:
	ldi r17, 1
	cp reg_status, r17
	brne irprot_isr0_status_2

	load_ir_sram_var ir_var_counterl
	mov r18, r17
	mov reg_counterl, r17

	lsl r18
	lsl r18
	cp r18, r17
	brsh irprot_isr0_status_1_chk1
	ldi r18, 0xFF
irprot_isr0_status_1_chk1:
	save_ir_sram_var_reg ir_var_calibr4, r18

	mov r17, reg_counterl
	mov r18, r17
	lsr r18
	lsr r18
	add r17,r18 ; counter + 25pct

	save_ir_sram_var_reg ir_var_calibr, r17
	save_ir_sram_var ir_var_status, 2

	rjmp irprot_isr0_end

irprot_isr0_status_2:
	ldi r17, 2
	cp reg_status, r17
	brne irprot_isr0_status_3

	; prepare for calc
	save_ir_sram_var ir_var_status, 3
	ldi r17, 0
	st Z+, r17 ; counterl
	st Z , r17 ; counterh
	rjmp irprot_isr0_end

irprot_isr0_status_3:
	ldi r17, 3
	cp reg_status, r17
	brne irprot_isr0_end
	; main Mancester bit calc
	load_ir_sram_var ir_var_counterl
	mov r18, r17
	load_ir_sram_var ir_var_calibr
	cp r18, r17
	brlo irprot_isr0_end ; ignore, this is not information interrupt

	; here we have information bit

	load_ir_sram_var ir_var_result2
	mov r18, r17
	load_ir_sram_var ir_var_result1
	
	; read info
	;clc
	;sbic ir_port_pinreg, ir_port_pin 
	;sec

	sec
	sbic ir_port_pinreg, ir_port_pin 
	clc
	
	rol r17
	rol r18

	save_ir_sram_var_reg ir_var_result1, r17
	mov r16, r17
	st Z, r18 ; ir_var_result2

	load_ir_sram_var ir_var_bitcnt
	inc r17
	sbiw zh:zl, 1
	st Z, r17 ; store bit cnt

	cpi r17, rc5_bitcnt
	brsh irprot_isr0_set_st4

	save_ir_sram_var ir_var_counterl, 0
	rjmp irprot_isr0_end

irprot_isr0_set_st4:
	;save_ir_sram_var ir_var_status, 4

	; check if key is the same as before
	load_ir_sram_var ir_var_saveresult
	cp r16, r17
	breq irprot_isr0_same_key

	; here different key

	save_ir_sram_var ir_var_presslen, 1
	save_ir_sram_var_reg ir_var_saveresult, r16
	rcall irprot_reinit

	rjmp irprot_isr0_end

irprot_isr0_same_key:

	load_ir_sram_var ir_var_presslen
	inc r17
	sbiw zh:zl,1
	st Z, r17  ; save presslen

	rcall irprot_reinit

irprot_isr0_end:
	pop r18
	pop r17
	pop r16

	pop zl
	pop zh

	pop reg_counterl
	pop reg_status

ret



