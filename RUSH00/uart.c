#include "rush00.h"

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

void	uart_debug(const char *s)
{
	(void)s;
	#if DEBUG
	while (*s)
	{
		if (*s == '\n')
			uart_tx('\r');
		uart_tx(*s);
		s++;
	}
	#endif
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

void	uart_newline(void)
{
	uart_tx('\n');
	uart_tx('\r');
}
