@ECHO OFF
"C:\Program Files\Atmel\AVR Tools\AvrAssembler2\avrasm2.exe" -S "C:\rolandas\avr\proj\nok_led\labels.tmp" -fI -W+ie -o "C:\rolandas\avr\proj\nok_led\nok_led.hex" -d "C:\rolandas\avr\proj\nok_led\nok_led.obj" -e "C:\rolandas\avr\proj\nok_led\nok_led.eep" -m "C:\rolandas\avr\proj\nok_led\nok_led.map" "C:\rolandas\avr\proj\nok_led\lcd_drive.asm"
