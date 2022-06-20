@ECHO OFF
"C:\Program Files\Atmel\AVR Tools\AvrAssembler2\avrasm2.exe" -S "C:\rolandas\avr\proj\led\labels.tmp" -fI -W+ie -o "C:\rolandas\avr\proj\led\led.hex" -d "C:\rolandas\avr\proj\led\led.obj" -e "C:\rolandas\avr\proj\led\led.eep" -m "C:\rolandas\avr\proj\led\led.map" "C:\rolandas\avr\proj\led\main5.asm"
