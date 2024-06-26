#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/*SPI == Serial Peripheral Interface,
data transfer protocol for microcontrollers*/

#define SPI_DDR DDRB
#define SS      PINB2 //Slave Select Line
#define MOSI    PINB3 //Master Out Slave In line
#define MISO    PINB4 //Master In Slave Out line
#define SCK     PINB5 //Shift Clock

void	set_ADC()
{
	//Select the high reference voltage: use of AVCC as a reference
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1 << REFS1);
	//Adjust for 8bits results
	ADMUX |= (1 << ADLAR);
	//on selectionne la ligne d entree du potentiometre rv1, la A0
	ADMUX &= ~(1 << MUX0);
	ADMUX &= ~(1 << MUX1);
	ADMUX &= ~(1 << MUX2);
	ADMUX &= ~(1 << MUX3);
	//clock prescaler
	ADCSRA |= (1 << ADPS2);
	ADCSRA |= (1 << ADPS1);
	ADCSRA |= (1 << ADPS0); 
	//activer les operations ADC
	ADCSRA |= (1 << ADEN);
}

void SPI_init()
{
    // set CS, MOSI and SCK to output, miso is an input
    SPI_DDR |= (1 << SS) | (1 << MOSI) | (1 << SCK);
    // enable SPI, set as master, and clock to fosc/128
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
}

void SPI_masterTransmitByte(uint8_t data)
{
    // load data into data register
    SPDR = data;
    // Wait for transmission complete //polling
	//termine quand SPIF passe a 1
    while(!(SPSR & (1 << SPIF)));
}

void	send_colordata_to_D6(uint8_t r, uint8_t g, uint8_t b)
{
	for (uint8_t i = 0; i < 4; i++)
		SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(b);
	SPI_masterTransmitByte(g);
	SPI_masterTransmitByte(r);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	for (uint8_t j = 0; j < 4; j++)
		SPI_masterTransmitByte(255);
}

void	send_colordata_to_D7(uint8_t r, uint8_t g, uint8_t b)
{
	for (uint8_t i = 0; i < 4; i++)
		SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(b);
	SPI_masterTransmitByte(g);
	SPI_masterTransmitByte(r);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	for (uint8_t j = 0; j < 4; j++)
		SPI_masterTransmitByte(255);
}

void	send_colordata_to_D8(uint8_t r, uint8_t g, uint8_t b)
{
	for (uint8_t i = 0; i < 4; i++)
		SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(b);
	SPI_masterTransmitByte(g);
	SPI_masterTransmitByte(r);
	for (uint8_t j = 0; j < 4; j++)
		SPI_masterTransmitByte(255);
}


void	set_average(uint8_t nb)
{
	if (nb >= (33 * 255 / 100) && (nb < (66 * 255 / 100)))
	{
		send_colordata_to_D6(127, 0, 255);
		send_colordata_to_D7(0, 0, 0);
		send_colordata_to_D8(0, 0, 0);
	}
	else if ((nb > (66 * 255 / 100)) && (nb <= (99 * 255 / 100)))
	{
		send_colordata_to_D6(127, 0, 255);
		send_colordata_to_D7(127, 0, 255);
		send_colordata_to_D8(0, 0, 0);
	}
	else if (nb == 255)
	{
		send_colordata_to_D6(127, 0, 255);
		send_colordata_to_D7(127, 0, 255);
		send_colordata_to_D8(127, 0, 255);
	}
}

void	ft_ADC()
{
	ADCSRA |= (1 << ADSC);
	while ((ADCSRA & (1 << ADSC)));
}

int main()
{
	SPI_init();
	set_ADC();
	while (1)
	{
		ft_ADC();
		set_average(ADCH);
	}
}