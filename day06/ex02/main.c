#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void set_timer_2() //bleu
{
	//mode 7- FAST PWM - 0 a 0xFF PAGE 164
	TCCR2A |= (1 << WGM21) | (1 << WGM20); 
	
	//prediv 1024 //PAGE 166
	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);

	//Clear OC2B on Compare Match, set OC2B at BOTTOM, (non-inverting mode).
	TCCR2A |= (1 << COM2B1); //PAGE 163
}

void set_timer_0() //rouge et vert
{
	//mode 1 - Compte de 0 à 0xFF
	TCCR0A = 0;
	TCCR0B = 0;
	TCCR0A |= (1 << WGM00) | (1 << WGM01);

	//prediv 1024
	TCCR0B |= (1 << CS00);
	TCCR0B |= (1 << CS02);
	
	//OC0A/B = 0 lors de la détection d’égalité ;
	//OC0A/B = 1 lorsque le compteur passe à 0 (BOTTOM) (mode non-inversé)
	TCCR0A |= (1 << COM0B1);
	TCCR0A |= (1 << COM0A1);
}

void init_rgb()
{
	set_timer_0();
	set_timer_2();
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	OCR0B = r;
	OCR0A = g;
	OCR2B = b;
}

void wheel(uint8_t pos)
{
	pos = 255 - pos;
	if (pos < 85)
	{
		set_rgb(255 - pos * 3, 0, pos * 3); //rouge au vert
	}
	else if (pos < 170)
	{
		pos = pos - 85;
		set_rgb(0, pos * 3, 255 - pos * 3); //vert  au bleu
	}
	else
	{
		pos = pos - 170;
		set_rgb(pos * 3, 255 - pos * 3, 0); // bleu au rouge
	}
}

int main()
{
	DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);
	init_rgb();
	set_rgb(0,0,0);
	while (1) {}
	uint8_t pos = 0;
	while (1)
	{
		if (pos == 255)
			pos = 0;
		wheel(pos);
		_delay_ms(10);
		pos++;
	}	
	return 0;
}
