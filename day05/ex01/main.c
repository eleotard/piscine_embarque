#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdbool.h>

#define UART_BAUDRATE (uint32_t)115200
#define MAX_COUNT 15


#define SELECTED_COUNT_ADDR ((const uint8_t *)0)
#define ADDR_COUNT_1 ((const uint8_t *)8)
#define ADDR_COUNT_2 ((const uint8_t *)16)
#define ADDR_COUNT_3 ((const uint8_t *)24)
#define ADDR_COUNT_4 ((const uint8_t *)32)


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
	uart_tx('\n');
}

void	init_counts()
{
	uint8_t count_1 = eeprom_read_byte(ADDR_COUNT_1) % (MAX_COUNT + 1);
	eeprom_write_byte(ADDR_COUNT_1, count_1);
	uint8_t count_2 = eeprom_read_byte(ADDR_COUNT_2) % (MAX_COUNT + 1);
	eeprom_write_byte(ADDR_COUNT_2, count_2);
	uint8_t count_3 = eeprom_read_byte(ADDR_COUNT_3) % (MAX_COUNT + 1);
	eeprom_write_byte(ADDR_COUNT_3, count_3);
	uint8_t count_4 = eeprom_read_byte(ADDR_COUNT_4) % (MAX_COUNT + 1);
	eeprom_write_byte(ADDR_COUNT_4, count_4);
	
	uint8_t test = eeprom_read_byte(SELECTED_COUNT_ADDR);
	if (test != 8 && test != 16 && test != 24 && test != 32)
		eeprom_write_byte(SELECTED_COUNT_ADDR, ADDR_COUNT_1);
}


void	light_leds()
{
	PORTB = (eeprom_read_byteeeprom_read_byte(SELECTED_COUNT_ADDR)) & 0x7);
	if (eeprom_read_byte(eeprom_read_byte(SELECTED_COUNT_ADDR)) >> 3)
		PORTB |= (1 << PB4);
}

int main()
{
	uart_init();
	DDRB |= 0x17; //10111
	DDRD &= ~(1 << PD2);
	DDRD &= ~(1 << PD4);
	
	bool sw1_is_pressed = 0;
	bool sw2_is_pressed = 0;
    while (1)
    {
        if (!(PIND & (1 << PD2)))
        {
            if (!sw1_is_pressed)
            {
                sw1_is_pressed = 1;
                if (eeprom_read_byte(eeprom_read_byte(SELECTED_COUNT_ADDR)) < MAX_COUNT)
                    eeprom_write_byte(eeprom_read_byte(SELECTED_COUNT_ADDR),
						(eeprom_read_byte(eeprom_read_byte(SELECTED_COUNT_ADDR)) + 1));
                else
                    eeprom_write_byte(eeprom_read_byte(SELECTED_COUNT_ADDR), (uint16_t)0);
                uart_putnbr(eeprom_read_byte(eeprom_read_byte(SELECTED_COUNT_ADDR)));
				light_leds();
            }
        }
		else
			sw1_is_pressed = 0;
		if (!(PIND & (1 << PD4)))
		{
			if (!sw2_is_pressed)
			{
				sw2_is_pressed = 1;
				uint8_t test = eeprom_read_byte(SELECTED_COUNT_ADDR);
				if (test == ADDR_COUNT_4)
					eeprom_write_byte(SELECTED_COUNT_ADDR, ADDR_COUNT_1);
				else
					eeprom_write_byte(SELECTED_COUNT_ADDR, test + 8);
			}
		}
		else
			sw2_is_pressed = 0;
	}
	return 1;
}


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