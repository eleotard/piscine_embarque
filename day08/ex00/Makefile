SRCS = main.c

OBJS = ${SRCS:%.c=%.o}

FLAGS = -mmcu=atmega328p -Wall -Wextra -Werror -Os -DF_CPU=16000000UL

all: main.hex flash

%.o: %.c Makefile
	avr-gcc ${FLAGS} -c $< -o $@

main.bin: ${OBJS}
	avr-gcc -mmcu=atmega328p -o $@ ${OBJS}

main.hex: main.bin
	avr-objcopy -j .text -j .data -O ihex main.bin $@

flash:
	avrdude -p atmega328p -c arduino -P /dev/ttyUSB0 -b 115200 -e -U flash:w:main.hex:i 

clean:
	rm *.o
	rm *.hex
	rm *.bin

.PHONY: all clean flash