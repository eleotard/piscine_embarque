#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

//PORTX: envoie des donnes sur la carte
//PINX: recoit des donnees de la carte

int main()
{
	DDRB |= (1 << PB0); //definit la broche 0 du portB comme output
	PORTB &= ~(1 << PB0);
	DDRD &= ~(1 << PD2); //input sur la switch
	
	bool laststate = PIND & (1 << PD2);
	while(1)
	{
		bool newstate = PIND & (1 << PD2); //si bouton appuye (=0)
		if ((laststate != newstate) && newstate == 0)
			PORTB ^= (1 << PB0);
		_delay_ms(500);
		laststate = newstate;
	}
}