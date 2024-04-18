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
	//UCSR0B -> USART Control and Status Register B : utilise pour controler les fonctionnalite de l'UART
	// enable the transmitter and receiver
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
	//active l interruption pour uart rx
	UCSR0B |= (1 << RXCIE0);
}

void	uart_tx(unsigned char c)
{
	//UCSR0A : contient divers indicateurs de statut liés à la communication série, tels que des indicateurs de transmission complète, de réception de données, etc
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

ISR (USART_RX_vect)
{
	unsigned char rec;

	rec = uart_rx();
	if (rec >= 'a' && rec <= 'z')
		rec -= 32;
	else if (rec >= 'A' && rec <= 'Z')
		rec += 32;
	uart_tx(rec);
}

int main()
{
	uart_init();
	sei();
	while(1)
	{
	}
}