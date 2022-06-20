; Program LED_blink

; definitions
.DEVICE ATmega8
.INCLUDE "m8def.inc"
.DEF	var1 = R16
.DEF	var2 = R17
.DEF	var3 = R18
.DEF	var4 = R19
	
; Instruction start at adress 0000
.CSEG
.ORG 0000

			rjmp	init	
			
; interupt vectors			

.INCLUDE "base.inc"
.INCLUDE "n3510i_lcd.inc"
.INCLUDE "chargen.inc"


init:		
;set the stackpointer
	ldi   var1, low( RAMEND )
	out   SPL, var1
	ldi   var1, high( RAMEND )
	out   SPH, var1
		
main:		
; start the main programm

	ldi   R20,0x23
	rcall  make_char


	ldi   var1, 0xFF
	out   DDRB, var1
	out   DDRD, var1
	ldi   var1, 0xFF
	out   PORTB, var1
	ldi   var1, 0
	out   PORTD, var1

;rjmp nothing_loop

	rcall lcd_3510i_hw_reset
	

	ldi   R16,1 ; soft lcd reset		
	rcall lcd_3510i_command	
	rcall wait_16th_sec

	ldi   R16,0xC6 ; Initial Escape
	rcall lcd_3510i_command	

	; Display Setup 1
	ldi   R16,0xB9 ; REFSET
	rcall lcd_3510i_command	
	ldi   R16,0  
	rcall lcd_3510i_param
	rcall lcd_3510i_raise_cs

	; Display control DISCTL
	ldi   R16,0xB6
	rcall lcd_3510i_command	
	ldi   R16,128  ; clock set
	rcall lcd_3510i_param
	ldi   R16,128  ; no inversions
	rcall lcd_3510i_param
	ldi   R16,129  ; div clock when idle, bias 1/9
	rcall lcd_3510i_param
	ldi   R16,84  ; 
	rcall lcd_3510i_param
	ldi   R16,69  ; 
	rcall lcd_3510i_param
	ldi   R16,82  ; 
	rcall lcd_3510i_param
	ldi   R16,67  ; 
	rcall lcd_3510i_param

	rcall lcd_3510i_raise_cs


	; Gray scale position
	ldi   R16,0xB3
	rcall lcd_3510i_command	
	ldi   R16,1
	rcall lcd_3510i_param
	ldi   R16,2
	rcall lcd_3510i_param
	ldi   R16,4
	rcall lcd_3510i_param
	ldi   R16,8
	rcall lcd_3510i_param
	ldi   R16,16
	rcall lcd_3510i_param
	ldi   R16,30
	rcall lcd_3510i_param
	ldi   R16,40
	rcall lcd_3510i_param
	ldi   R16,50
	rcall lcd_3510i_param
	ldi   R16,60
	rcall lcd_3510i_param
	ldi   R16,70
	rcall lcd_3510i_param
	ldi   R16,80
	rcall lcd_3510i_param
	ldi   R16,90
	rcall lcd_3510i_param
	ldi   R16,100
	rcall lcd_3510i_param
	ldi   R16,110
	rcall lcd_3510i_param
	ldi   R16,127
	rcall lcd_3510i_param
	rcall lcd_3510i_raise_cs

	; Gamma curve set
	ldi   R16,0xB5
	rcall lcd_3510i_command	
	ldi   R16,1
	rcall lcd_3510i_param
	rcall lcd_3510i_raise_cs
	
	; Common driver output select
	ldi   R16,0xBD
	rcall lcd_3510i_command	
	ldi   R16,0
	rcall lcd_3510i_param
	rcall lcd_3510i_raise_cs

	; Power supply setup
	; PWRCTL
	ldi   R16,0xBE
	rcall lcd_3510i_command	
	ldi   R16,4
	rcall lcd_3510i_param
	rcall lcd_3510i_raise_cs

	; Sleep out
	ldi   R16,0x11
	rcall lcd_3510i_command	

	; Voltage ctrl	

	; Contrast
	ldi   R16,0x25
	rcall lcd_3510i_command	
	ldi   R16,63 ; middle value
	rcall lcd_3510i_param
	rcall lcd_3510i_raise_cs

	; Temperature gradient
	ldi   R16,0xB7
	rcall lcd_3510i_command	
	ldi   R17,13
set_lcd_temperature_gradient:		
	ldi   R16,0
	rcall lcd_3510i_param
	dec   R17
	brne  set_lcd_temperature_gradient
	rcall lcd_3510i_raise_cs

	; Booster voltage on		
	ldi   R16,0x03
	rcall lcd_3510i_command	

	; Wait for booster voltage stabilizing
	rcall wait_16th_sec

	; Display setup 2
	; Invsersion control
	ldi   R16,0x20 ; Off normal
	rcall lcd_3510i_command	

	; Display setup 3
	; 12 bits per pixel

	ldi   R16,0x2C
	rcall lcd_3510i_command	

	ldi   R18,30
loop112:
	ldi   R17,100
	ldi   R16,0xF0
loop111:
	rcall lcd_3510i_param
	dec   R17
	brne  loop111
	rcall lcd_3510i_raise_cs

	dec   R18
	brne  loop112
	
	
	ldi   R16,0x20 ; Display ON
	rcall lcd_3510i_command	
	


nothing_loop:
	sbi     PORTD,0
	cbi     PORTD,1
	rcall   wait_16th_sec
	rcall   wait_16th_sec
	rcall   wait_16th_sec
	rcall   wait_16th_sec
	rcall   wait_16th_sec
	rcall   wait_16th_sec
	cbi     PORTD,0
	sbi     PORTD,1
	rcall   wait_16th_sec
	rcall   wait_16th_sec
	rcall   wait_16th_sec
	rcall   wait_16th_sec
	rcall   wait_16th_sec
	rcall   wait_16th_sec
	rjmp    nothing_loop
	



