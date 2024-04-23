#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdbool.h>

#define UART_BAUDRATE (uint32_t)115200

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

void uart_putnbr(int n)
{
    if (n < 0) {
        uart_tx('-');
        n = -n;
    }
    if (n >= 10) {
        uart_putnbr(n / 10);
    }
    uart_tx('0' + n % 10);
}


int main()
{
	uart_init();
	DDRB |= 0x17;
	DDRD &= ~(1 << PD2);
	
	bool is_pressed = 0;
	uint16_t count = 0;
	if (eeprom_read_byte(0) == 255)
        eeprom_write_word(0, count);
    while (1)
    {
        if (!(PIND & (1 << PD2)))
        {
            if (!is_pressed)
            {
                is_pressed = 1;
                if (eeprom_read_word(0) < 15)
                    eeprom_write_word(0, (eeprom_read_word(0) + 1));
                else
                    eeprom_write_word(0, (uint16_t)0);
                uart_putnbr(eeprom_read_word(0));
                uart_printstr("\n");
				PORTB = (eeprom_read_word(0) & 0x7);
				if (eeprom_read_word(0) >> 3)
					PORTB |= (1 << PB4);
            }
        }
		else
			is_pressed = 0;
	}
	return 1;

	//regarder quelle est la taille de l eeprom (1024 octets)
	//ladresse 0 (NULL) existe 

	//commence tjrs a ladresse 0
	//ETAPES
	//mettre un id qui est lie au buffer
	//stock la longueur du buffer (en OCTET probablement)
	//stock le buffer
	
	//si je vois un id occupe je lis la len et je me decale de la len

	//si id == 0 l emplacement est libre
	//apres avoir place un truc je met derriere l id a 0 et la len de l espace restant 

	//2 octets pour l id
	//2 octets pour la len
	//minimum 4 octets

}