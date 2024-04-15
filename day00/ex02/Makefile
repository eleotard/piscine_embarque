all: hex flash

hex:
	avr-gcc -mmcu=atmega328p -Os -F_CPU=1000000 -o main.bin main.c
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex

flash:
	avrdude -p atmega328p -c arduino -P /dev/ttyUSB0 -b 115200 -e -U flash:w:main.hex:i 

clean:
	rm *.hex
	rm *.bin