#include <avr/io.h>

int main()
{
	DDRB |= (1 << PB1);
	
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
	OCR1A = ICR1 * 1 / 10;
	while (1)
	{
	}
}