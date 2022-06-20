
; inputs R2 as binary bit number
; outputs R3 as answer
calc_set_bit:
	push R16
	push R2

	ldi  R16,1
	inc  R2
	dec  R2 ; test for zero for low regs
	breq end_calc_bit

lsl_loop:
	lsl  R16
	dec  R2
	brne lsl_loop 	
end_calc_bit:
	mov  R3,R16
	pop  R2
	pop  R16
	ret


; sets specified bit on port B
; expects binary bit number in R2
set_bit_port_b:
	push R3
	rcall calc_set_bit
	in   R2,PORTB
	or   R2,R3
	out  PORTB,R2
	pop  R3
	ret


; clears specified bit on port B
; expects binary bit number in R2
clr_bit_port_b:
	push R3
	rcall calc_set_bit
	com  R3  ; do invert there
	in   R2,PORTB
	and  R2,R3
	out  PORTB,R2
	pop  R3


; out bit specified by R3 lsb to port's B pin spec. by R2
out_bit_port_b:
	push R16	
	mov  R16,R3
	andi R16,1
	breq clear_bit_and_out_b
	rcall set_bit_port_b
	rjmp out_bit_port_b_ret

clear_bit_and_out_b:
	rcall clr_bit_port_b

out_bit_port_b_ret:
	pop  R16
	ret


; sets specified bit on port D 
; expects binary bit number in R2
set_bit_port_d:
	push R3
	rcall calc_set_bit
	in   R2,PORTD
	or   R2,R3
	out  PORTD,R2
	pop  R3
	ret


; clears specified bit on port D
; expects binary bit number in R2
clr_bit_port_d:
	push R3
	rcall calc_set_bit
	com  R3  ; do invert there
	in   R2,PORTD
	and  R2,R3
	out  PORTD,R2
	pop  R3



; out bit specified by R3 lsb to port's B pin spec. by R2
out_bit_port_d:
	push R16	
	mov  R16,R3
	andi R16,1
	breq clear_bit_and_out_d
	rcall set_bit_port_d
	rjmp out_bit_port_d_ret

clear_bit_and_out_d:
	rcall clr_bit_port_d

out_bit_port_d_ret:
	pop  R16
	ret


; waits 1/16 th sec on 8mhz
wait_16th_sec:       
			push    R16
			push  	R17
			ldi     R16, 0xFF
lp1_wait:   ldi     R17, 0xFF
lp2_wait:   dec     R17 
            brne    lp2_wait
            dec     R16
            brne    lp1_wait
			pop 	R17
			pop 	R16
            ret


; waits 256*2 cpu cycles
wait_512:
			push  	R17
		    ldi     R17, 0xFF
lp22_wait:  dec     R17 
            brne    lp22_wait
			pop 	R17
            ret

