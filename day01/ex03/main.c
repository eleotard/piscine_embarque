#include <avr/io.h>
#include <stdbool.h>

int main()
{
	DDRB |= (1 << PB1);
	//boutons SW1 et SW2 en input 
	DDRD &= ~(1 << PD2);
	DDRD &= ~(1 << PD4);
	
	//Mode 14: compte de 0 à ICR1
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM12);
	TCCR1B |= (1 << WGM13);

	//allume led quand =OCR1A et eteint quand retour au BOTTOM
	TCCR1A &= ~(1 << COM1A0);
	TCCR1A |= (1 << COM1A1);
	
	//pre-division clock/1024
	TCCR1B |= (1 << CS10);
	TCCR1B &= ~(1 << CS11);
	TCCR1B |= (1 << CS12);
	ICR1 = 15625;

	int i = 50;
	bool laststate = PIND & (1 << PD2);
	bool laststate2 = PIND & (1 << PD4);
	while (1)
	{
		bool newstate = PIND & (1 << PD2);
		bool newstate2 = PIND & (1 << PD4);
		if (!newstate && laststate)
		{
			if (i - 10 >= 10)
				i -= 10;
		}
		if (!newstate2 && laststate2)
		{
			if (i + 10 <= 100)
				i += 10;
		}
		OCR1A = ICR1 * 1 / i;
		laststate = newstate;
		laststate2 = newstate2;
	}
}