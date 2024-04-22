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


//check calibre et busy 

void	i2c_read(void)
{
	//(1 << TWEA) sert a dire que le maitre doit envoyer un 
	//signal ACK apres chaque reception de data
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)));
    if (TW_STATUS == TW_MR_DATA_ACK)
        uart_printstr("Data byte has been recieved; ACK has been received\n");
    else
        uart_printstr("ERROR: Data byte has not been recieved\n");
}

void	i2c_write(unsigned char data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TW_STATUS == TW_MT_DATA_ACK) //0x28
		uart_printstr("Data byte has been transmitted; ACK has been received\n");
	else if (TW_STATUS == TW_MT_SLA_ACK) //0x18
		uart_printstr("SLA+W has been transmitted; ACK has been received\n");
	else if(TW_STATUS == TW_MR_SLA_ACK) //0x40
		uart_printstr("SLA+R has been transmitted; ACK has been received\n");
	else
		uart_printstr("ERROR - (data or +R +W)\n");
}

void	i2c_start()
{
	TWCR = 0;
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
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

void	print_hex_value(uint8_t *data)
{
	char	data_str[7][3];

	for (uint8_t i = 0; i < 7; i++)
	{
		int_to_hex_str(data[i], data_str[i]);
		uart_printstr(data_str[i]);
		uart_tx(' ');
	}
	uart_tx('\b');
}

int main()
{
	uint8_t	data[7];

	uart_init();
	_delay_ms(100);
	i2c_init();
	while(1)
	{
		i2c_start();
		i2c_write(0x38 << 1);
		_delay_ms(10);
		i2c_write(0xAC);
		i2c_write(0x33); //arguments de la commande
		i2c_write(0x00); //arg2
		_delay_ms(80);
		//i2c_stop();
		i2c_start();
		i2c_write((0x38 << 1) | 1);
		for (int i = 0; i < 7; i++)
		{
			i2c_read();
			data[i] = TWDR;
		}
		i2c_stop();

		print_hex_value(data);
		uart_printstr("\n\n");
		_delay_ms(2000);
	}
	return 1;
}