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

char	uart_rx(void)
{
	while (!(UCSR0A & (1 << RXC0)))
    {}
	return UDR0;
}

int main()
{
	unsigned char recieved;

	uart_init();
	while(1)
	{
		recieved = uart_rx();
		uart_tx(recieved);
	}
}