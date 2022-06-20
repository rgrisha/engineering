
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

; if r0 is between 1 and 30, inc it
inc_checked:
	push r16

	ldi r16, 31
	cp r0, r16
	brsh inc_checked_end

	; between 1 and 30 here
	inc r0

inc_checked_end:
	pop r16
	ret


; if r0 is >= 2 dec it
dec_checked:
	push r16

	ldi r16, 2
	cp r0, r16
	brlo dec_checked_end

	dec r0
	
dec_checked_end:
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


; address in r1:r0
; out in r16
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

