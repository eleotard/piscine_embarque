#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int main()
{
	DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);
	uint8_t i = 0;
	while (1)
	{
		if (i > 6)
			i = 0;
		if (i == 0)
			PORTD = (1 << PD5); //rouge
		else if (i == 1)
			PORTD = (1 << PD6); //vert
		else if (i == 2)
			PORTD = (1 << PD3); //bleu
		else if (i == 3)
			PORTD = (1 << PD5) | (1 << PD6);
		else if (i == 4)
			PORTD = (1 << PD6) | (1 << PD3);
		else if (i == 5)
			PORTD = (1 << PD5) | (1 << PD3);
		else if (i == 6)
			PORTD = (1 << PD5) | (1 << PD6) | (1 << PD3);
		_delay_ms(1000);
		i++;
	}	
	return 0;
}