#include <avr/io.h>
#include <avr/interrupt.h>


//SW1=PD2(INT0)
//LED D1=PB0

ISR (INT0_vect, ISR_BLOCK)
{
	PORTB ^= (1 << PB0);

	// Effacer le drapeau d'interruption INTF0 en écrivant un 1 dans le bit correspondant
    EIFR |= (1 << INTF0);
}

int main()
{
	DDRB |= (1 << PB0);
	DDRD &= ~(1 << PD2);

	//enable les interrupts
	sei();
	
	//mask that enable interrupt INT0
	EIMSK |= (1 << INT0);

	//configurer Interrupt Sense Control 0 Bit 1 et Bit 0
	//The low level of INT0 generates an interrupt request.
	
	//00 //nuit //jour 
	EICRA &= ~(1 << ISC01);
	EICRA &= ~(1 << ISC00);

	//01 //seteint quand je reclache
	// EICRA &= ~(1 << ISC01);
	// EICRA |= (1 << ISC00);

	while (1)
	{}



}