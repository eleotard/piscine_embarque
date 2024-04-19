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
}

void	uart_tx(unsigned char c)
{
	//UCSR0A : contient divers indicateurs de statut liés à la communication série, tels que des indicateurs de transmission complète, de réception de données, etc
	//UDRE0 : bit de UCSR0A
	//if UDRE0 is one, the buffer is empty, and therefore ready to be written.
	while(!(UCSR0A & (1 << UDRE0)))
	{

	}
	// registre utilise pour stocker les données à transmettre ou reçues.
	UDR0 = c;
}

int main()
{
	uart_init();
	while(1)
	{
		uart_tx('Z');
		_delay_ms(1000);
	}
}