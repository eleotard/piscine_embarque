#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define UART_BAUDRATE (uint32_t)115200

void	uart_init(void)
{
	uint32_t	ubrr;

	ubrr = (F_CPU / (16 * UART_BAUDRATE));
	UBRR0L = (uint8_t)(ubrr & 0xFF);
	UBRR0H = (uint8_t)(ubrr >> 8);
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
	UCSR0B |= (1 << RXCIE0);
}

void	uart_tx(unsigned char c)
{
	while (!(UCSR0A & (1 << UDRE0)))
		;
	UDR0 = c;
}

void	uart_printstr(const char *s)
{
	while (*s)
	{
		if (*s == '\n')
			uart_tx('\r');
		uart_tx(*s);
		s++;
	}
}

char	uart_rx(void)
{
	while (!(UCSR0A & (1 << RXC0)))
    {}
	return UDR0;
}


void	uart_putnbr(int64_t n)
{
	if (n < 0)
	{
		uart_tx('-');
		n = -n;
	}
	if (n > 9)
	{
		uart_putnbr(n / 10);
		uart_putnbr(n % 10);
	}
	else
		uart_tx(n + '0');
	
}

void	uart_newline()
{
	uart_tx('\n');
	uart_tx('\r');
}

// ISR (USART_RX_vect)
// {
// 	unsigned char rec;

// 	rec = uart_rx();
// 	uart_tx(rec);
// }

void set_timer_2() //bleu
{
	//mode 7- FAST PWM - 0 a 0xFF PAGE 164
	TCCR2A |= (1 << WGM21) | (1 << WGM20); 
	
	//prediv 1024 //PAGE 166
	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);

	//Clear OC2B on Compare Match, set OC2B at BOTTOM, (non-inverting mode).
	TCCR2A |= (1 << COM2B1); //PAGE 163
	OCR2B = 0;
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
	OCR0B = 0;
	OCR0A = 0;
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	OCR0B = r;
	OCR0A = g;
	OCR2B = b;
}

void init_rgb()
{
	set_timer_0();
	set_timer_2();
}

int	check_char(char c, int i)
{
	if (i == 0 && c != '#')
		return 1;
	else if (i != 0 && (!(c >= 'A' && c <= 'F') && !(c >= '0' && c <= '9')))
		return 1;
	return 0;
}

void	print_error_and_reset(char *color_str, int *i)
{
	uart_newline();
	uart_printstr("ERROR input: ");
	uart_tx(color_str[*i]);
	uart_printstr(" is not a correct caracter (must be in 0123456789ABCDEF or #)\n");
	int index = 0;
	while (++index < 8)
		color_str[*i] = '\0';
	uart_newline();
	uart_printstr("Enter a HEX RGB value, format #RRGGBB:\n");
	*i = 0;
}

int main()
{
	DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);
	uart_init();
	init_rgb();
	char	color_str[8];
	//uint32_t color = 0;
	uint8_t color_str_is_set = 0;

	uart_printstr("Enter a HEX RGB value, format #RRGGBB:\n");
	int i = 0;
	while (1)
	{
		if (i < 8 && (color_str_is_set == 0))
		{

			color_str[i] = uart_rx();
			uart_tx(color_str[i]);
			if (check_char(color_str[i], i))
				print_error_and_reset(color_str, &i);
			else
				i++;
			if (i == 7)
				color_str_is_set = 1;
		}
	}
	return 1;
}