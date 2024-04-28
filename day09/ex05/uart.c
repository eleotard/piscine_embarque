#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define UART_BAUDRATE (uint32_t)115200
#define DEBUG 0
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

void	uart_debug(const char *s)
{
	if (DEBUG)
	{
		while (*s)
		{
			if (*s == '\n')
				uart_tx('\r');
			uart_tx(*s);
			s++;
		}

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


void	uart_printhexa(uint8_t data)
{
	const char hex_digits[] = "0123456789ABCDEF";
	uart_tx(hex_digits[data >> 4]);
	uart_tx(hex_digits[data & 0x0F]);
	uart_newline();
}

void uart_printbinary(uint8_t value)
{
    const char binary_digits[] = "01";

    for (int8_t i = 7; i >= 0; i--) {
        uint8_t bit = (value >> i) & 0x01;
        uart_tx(binary_digits[bit]);
    }
    uart_newline();
}