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

void	send_slave_command()
{
	//clear pour continuer le transfert
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TWSR == 0x18) //SLA+W has been transmitted; ACK has been received
	{
		uart_printstr("bien fait le write dans SLA\n");
		TWDR = 0x33;
	}
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TWSR == 0x28) //data has been transmitted
	{
		uart_printstr("1e partie de la commande recue\n");
		TWDR = 0x00;
	}
	TWCR = (1<<TWINT) | (1<<TWEN);
	_delay_ms(80);
}

//initialise l'interface i2c sur le MCU
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

//demarre une communication entre le le MCU et le CAPTEUR
//TWSR: registre d'etat
void	i2c_start()
{
	//wait tant que cest pas active
	while (!(TWCR & (1 << TWINT)));
	uart_tx(TWSR + 24);
	if (TWSR == 0x08) //A START condition has been transmitted
	{
		// uart_tx(TWSR + 24);
		uart_printstr("A bien fait le start\n");
		//permet de rentrer en MT("MASTER") mode
		TWDR = SLA << 1;
	}
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TWSR == 0x18) //SLA+W has been transmitted; ACK has been received
	{
		uart_printstr("A bien fait le write dans SLA\n");
	}
	//send_slave_command();
}

//stoppe la communication
void	i2c_stop()
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

int main()
{
	uart_init();
	i2c_init();
	i2c_start();
	i2c_stop();
	while (1)
	{}
	return 1;
}