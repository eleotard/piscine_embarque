#include <avr/io.h>

#define SPI_DDR DDRB
#define SS      PINB2 //Slave Select Line
#define MOSI    PINB3 //Master Out Slave In line
#define MISO    PINB4 //Master In Slave Out line
#define SCK     PINB5 //Shift Clock

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