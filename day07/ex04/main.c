#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void	uart_init(void);
void	uart_tx(unsigned char c);
void	uart_printstr(const char *s);
char	uart_rx(void);
void	uart_putnbr(int64_t n);
void	uart_newline();

void set_timer_2() //bleu
{
	//mode 7- FAST PWM - 0 a 0xFF PAGE 164
	TCCR2A |= (1 << WGM21) | (1 << WGM20); 
	
	//prediv 1024 //PAGE 166
	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);

	//Clear OC2B on Compare Match, set OC2B at BOTTOM, (non-inverting mode).
	//Set OC2B on Compare Match, clear OC2B at BOTTOM, (inverting mode).
	TCCR2A |= (1 << COM2B1); //PAGE 163 (nn invert)
	//TCCR2A |= (1 << COM2B0);
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
	//OC0A/B = 1 lors de la détection d’égalité ;
	//OC0A/B = 0 lorsque le compteur passe à 0 (BOTTOM) (mode inversé)
	TCCR0A |= (1 << COM0B1); //nn invert
	//TCCR0A |= (1 << COM0B0);
	TCCR0A |= (1 << COM0A1); //nn invert
	//TCCR0A |= (1 << COM0A0);
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

void	set_ADC()
{
	//Select the high reference voltage: use of AVCC as a reference
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1 << REFS1);
	//Adjust for 8bits results
	ADMUX |= (1 << ADLAR);
	//on selectionne la ligne d entree du potentiometre rv1, la A0
	ADMUX &= ~(1 << MUX0);
	ADMUX &= ~(1 << MUX1);
	ADMUX &= ~(1 << MUX2);
	ADMUX &= ~(1 << MUX3);
	//clock prescaler
	ADCSRA |= (1 << ADPS2);
	ADCSRA |= (1 << ADPS1);
	ADCSRA |= (1 << ADPS0); 
	//activer les operations ADC
	ADCSRA |= (1 << ADEN);
}

void	int_to_hex_str(uint8_t data, char *data_str)
{
	const char hex_digits[] = "0123456789ABCDEF";

	data_str[0] = hex_digits[(data >> 4) & 0x0F];
	data_str[1] = hex_digits[data & 0x0F];
	data_str[2] = '\0';
}

void	ft_bzero(char *data_str, uint8_t size)
{
	uint8_t i = 0;
	while (++i < size)
		data_str[i] = 0;
}

void	set_average(uint8_t nb)
{
	if (nb <= (25 * 255 / 100))
		PORTB = (1 << PB0);
	else if ((nb > (25 * 255 / 100)) && (nb <= (50 * 255 / 100)))
		PORTB = (1 << PB0) | (1 << PB1);
	else if ((nb > (50 * 255 / 100)) && (nb <= (75 * 255 / 100)))
		PORTB = (1 << PB0) | (1 << PB1) | (1 << PB2);
	else
		PORTB = (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);
}

void	ft_ADC()
{
	char	data_str[3];

	ft_bzero(data_str, 3);
	ADCSRA |= (1 << ADSC);
	while ((ADCSRA & (1 << ADSC)));
	int_to_hex_str(ADCH, data_str);
	uart_printstr(data_str);
	uart_newline();
}


int main()
{
	DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);
	DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);
	uart_init();
	init_rgb();
	set_ADC();
	while (1)
	{
		ft_ADC();
		set_average(ADCH);
		wheel(ADCH);
	}	
	return 0;
}
