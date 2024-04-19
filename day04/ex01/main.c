#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>


#define UART_BAUDRATE (uint32_t)115200
#define FREQ_SCL 100000 //100kHz <=> 100000 cycles/sec
#define PRESCALER 1
#define SLA	0x38

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
	//set debit binaire twi
	TWBR = ((F_CPU / FREQ_SCL) - 16) / (2 * PRESCALER);
	//clear le bit, juste apres la transmission de la START condition est envoyee
	TWCR |= (1 << TWINT);
	//init acces maitre(START)//transmit a START CONDITION
	TWCR |= (1 << TWSTA);
	//active le fonctionnement i2c/twi et active interface twi 
	TWCR |= (1 << TWEN);
}

void	i2c_read(void)
{
	
}

void	i2c_write(unsigned char data)
{
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1 << TWINT)));
	TWDR = data;
	if (TWSR == 0x28) //data has been transmitted
	{
		uart_tx(data);
		uart_printstr(" - recu\n");
	}
}

void	send_slave_command()
{
	_delay_ms(10);
	i2c_write(0xAC);
	i2c_write(0x33);
	i2c_write(0x00);
	_delay_ms(80);
}

void	i2c_start()
{
	while (!(TWCR & (1 << TWINT)));
	//uart_tx(TWSR + 24);
	if (TWSR == 0x08)
		uart_printstr("A bien fait le start\n");
	TWDR = SLA << 1;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TWSR == 0x18) //SLA+W has been transmitted; ACK has been received
		uart_printstr("A bien fait le write dans SLA\n");
	send_slave_command();
}

void	i2c_stop()
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

int main()
{
	uart_init();
	i2c_init();
	i2c_start();
	i2c_read();
	i2c_stop();
	while (1)
	{	}
	return 1;
}