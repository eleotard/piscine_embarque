#include <avr/io.h>

int main()
{
	DDRB |= (1 << PB1);
	
	//Mode 15: compte de 0 à ICR1
	TCCR1A |= (1 << WGM10); //
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM12);
	TCCR1B |= (1 << WGM13);

	//switch led 0/1
	TCCR1A |= (1 << COM1A0);
	TCCR1A &= ~(1 << COM1A1);
	
	//pre-division clock/1024
	TCCR1B |= (1 << CS10);
	TCCR1B &= ~(1 << CS11);
	TCCR1B |= (1 << CS12);

	// ICR1 = 15625 ; //resultat de la pre-division
	// OCR1A = ICR1 * 1 / 2;
	OCR1A = 15625 / 2;
	while (1)
	{
	}
}