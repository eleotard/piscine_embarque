#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//-T vt100
#define UART_BAUDRATE (uint32_t)115200

void	uart_init()
{
	uint32_t ubrr;

	ubrr = (F_CPU / (16 * UART_BAUDRATE));
	UBRR0L = (uint8_t)(ubrr & 0xFF);
	UBRR0H = (uint8_t)(ubrr >> 8);
	// enable the transmitter and receiver
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
}

void	uart_tx(unsigned char c)
{
	while(!(UCSR0A & (1 << UDRE0)))
	{}
	UDR0 = c;
}

void uart_printstr(const char* s)
{
	while (*s)
	{
		if (*s == '\n')
			uart_tx('\r');
		uart_tx(*s);
		s++;
	}
}

int main()
{
	uart_init();
	while(1)
	{
		uart_printstr("Hello World!\n");
		_delay_ms(2000);
	}
}