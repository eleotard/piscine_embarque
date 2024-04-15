#include <avr/io.h>


int main()
{
	DDRB = 0b00000001; //definit la broche 0 du portB comme output
	PORTB = 0b00000001; //change letat a 1 de cette broche
}