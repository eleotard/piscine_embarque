#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>


#define UART_BAUDRATE (uint32_t)115200
#define FREQ_SCL 100000 //100kHz <=> 100000 cycles/sec
#define PRESCALER 1
#define SLA	0x38


void	int_to_hex_str(uint8_t data, char *data_str)
{
	const char hex_digits[] = "0123456789ABCDEF";

	data_str[0] = hex_digits[(data >> 4) & 0x0F];
	data_str[1] = hex_digits[data & 0x0F];
	data_str[2] = '\0';
}

void	uart_init()
{
	uint32_t ubrr;

	ubrr = (F_CPU / (16 * UART_BAUDRATE));
	UBRR0L = (uint8_t)(ubrr & 0xFF);
	UBRR0H = (uint8_t)(ubrr >> 8);
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
	UCSR0B |= (1 << RXCIE0);
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

//TWCR: registre de controle: 
	//ACTIVE
	//INITIER UN ACCES MAITRE AVEC START
	//GENERER UN ACCUSE DE RECEPTION
	//STOP

//TWSR: registre d'etat
	//doit etre egal a 8
//TWAR: registre d'adresses TWI (esclave)
	//doc du capteur: adresse: 0x38 en hex, a indiquer dans twar
//TWDR: registre de donnees

void	i2c_init()
{
	TWBR = ((F_CPU / FREQ_SCL) - 16) / (2 * PRESCALER);
}

void	i2c_read(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)));
    if (TWSR == 0x50)
        uart_printstr("Data byte has been recieved; ACK has been received\n");
    else
        uart_printstr("ERROR: Data byte has not been recieved\n");
}

void	i2c_write(unsigned char data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TWSR == 0x28) //data has been transmitted,ACK returned
		uart_printstr("Data byte has been transmitted; ACK has been received\n");
	else if (TWSR == 0x18)
		uart_printstr("SLA+W has been transmitted; ACK has been received\n");
	else if(TWSR == 0x40)
		uart_printstr("SLA+R has been transmitted; ACK has been received\n");
	else
		uart_printstr("ERROR - (data or +R +W)\n");
}

void	i2c_start()
{
	TWCR |= (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TWSR == 0x08)
		uart_printstr("A START condition has been transmitted\n");
	else if (TWSR == 0x10)
		uart_printstr("A repeated START condition has been transmitted\n");
	else
		uart_printstr("ERROR -(START condition)\n");
}

void	i2c_stop()
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

#include <stdbool.h>
int main()
{
	uart_init();
	i2c_init();

	// DDRD = 0x00;
	// DDRB = 0xFF;
	// bool hello = 0, world = 0, lasthello = 0, lastworld = 0;
	while(1)
	{
		i2c_start();
		i2c_write(add);
		// hello = (!(PIND & (1 << PD2)));
		// world = (!(PIND & (1 << PD4)));

		// // Increment or decrement the value based on button presses
		// if ((world != lastworld && !world) || (hello != lasthello && !hello))
		// {
		// 	if (hello != lasthello)
		// 	{
		// 		uart_printstr("hello!\n");
		// 		PORTB = 0b00010010;
		// 	}
		// 	if (world != lastworld)
		// 	{
		// 		PORTB = 0b00010001;
		// 		uart_printstr("world!\n");

		// 	}
		// 	_delay_ms(50); // Debounce delay
		// }
		// lasthello = hello;
		// lastworld = world;

	}
	return 1;
}