#include <avr/io.h>
#include <util/delay.h>

//PORTX: envoie des donnes sur la carte
//PINX: recoit des donnees de la carte

int main()
{
	DDRB |= (1 << PB0); //definit la broche 0 du portB comme output
	DDRD &= ~(1 << PD2); //input sur la switch
	PORTB &= ~(1 << 0);
	
	while(1)
	{
		if (!((PIND >> 2) & 1)) //si bouton appuye (=0)
		{
			PORTB |= (1 << PB0);
			_delay_ms(50);
		}
		else
			PORTB &= ~(1 << 0);
	}
}