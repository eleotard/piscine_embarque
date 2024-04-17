#include <avr/io.h>
#include <stdbool.h>

int main()
{
	//activer PB1 comme output
	DDRB |= (1 << PB1);
	
	TCCR1A = 0;
    TCCR1B = 0;
	//Mode 14: compte de 0 à ICR1
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM12);
	TCCR1B |= (1 << WGM13);

	//allume led quand =OCR1A et eteint quand retour au BOTTOM
	TCCR1A |= (1 << COM1A1);
	
	//pre-division clock/576
	TCCR1B |= (1 << CS12);

	ICR1 = 62500;
	OCR1A = 31250;

	//boutons SW1 et SW2 en input 
	DDRD &= ~(1 << PD2);
	DDRD &= ~(1 << PD4);

	uint8_t duty_cycle = 50;
    uint8_t pd2_is_pressed = 0;
	uint8_t pd4_is_pressed = 0;
	while (1)
	{
		if (!(PIND & (1 << PD2)))
		{
			if(!pd2_is_pressed)
			{
				pd2_is_pressed = 1;
				if (duty_cycle + 10 <= 100)
					duty_cycle += 10;
				OCR1A = (duty_cycle * 62500) / 100;
			}
		}
		else
			pd2_is_pressed = 0;
		if (!(PIND & (1 << PD4)))
		{
			if (!pd4_is_pressed)
			{
				pd4_is_pressed = 1;
				if (duty_cycle - 10 >= 10)
					duty_cycle -= 10;
				OCR1A = (duty_cycle * 62500) / 100;
			}
		}
		else
			pd4_is_pressed = 0;
	}
}