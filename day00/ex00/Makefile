# Écrire un fichier Makefile avec la règle all qui exécute
# la règle hex puis la règle flash.
##########################################################
#-0s :
#optimisation du code pour optimiser utilisation memoire
#du microcontrolleur

#-mmcu=atmega328p :
# Indique à avr-gcc quel microcontrôleur est utilisé

# -j .text -j .data :
# selectionne code executable (.text)
# selectionne donnees initialisees (.data)

#-0 inex
# format de sortie Intel HEX

#avrdude
#-e efface la memoire flash avant de programmer de nvx fichiers
#-b bault rate vitesse de communication avec le pc bits/secondes
#-c specifie le type de programmeur utilise
#-U flash:w:main.hex:i specifie les operations de programmation
#flash	-> indique qu'on veut programmer la memoire flash
#w		-> write, on veut ecrire des donnees
#main.hex ->le nom du fichier qu on veut programmer
#i le fichier hex est en format intel HEX

SRCS = main.c

OBJS = ${SRCS:%.c=%.o}

FLAGS = -mmcu=atmega328p -Wall -Wextra -Werror -Os -DF_CPU=1000000 

all: main.hex flash

%.o: %.c Makefile #pour que ca recompile si le makefile est modifie
	avr-gcc ${FLAGS} -c $< -o $@

main.bin: ${OBJS} #doit etre le nom du fichier
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