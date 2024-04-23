SRCS = main.c uart.c i2c.c utils.c game.c
ELSIE = /dev/ttyUSB1
MOHA = /dev/ttyUSB0
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
	killall screen || true
	avrdude -p atmega328p -c arduino -P $(MOHA) -b 115200 -e -U flash:w:main.hex:i 
	gnome-terminal -- screen $(MOHA) 115200
	avrdude -p atmega328p -c arduino -P $(ELSIE) -b 115200 -e -U flash:w:main.hex:i 
	gnome-terminal -- screen $(ELSIE) 115200
	sleep 1
	@echo -n "je suis MOHA\n\r" > /dev/pts/1
	@echo -n "je suis ELSIE\n\r" > /dev/pts/3

clean:
	rm *.o
	rm *.hex
	rm *.bin

.PHONY: all clean flash