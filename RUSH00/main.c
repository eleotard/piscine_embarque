#include "rush00.h"

uint8_t mystatus = 0;

void	init_LED_SW1(void)
{
	RESETPIN(DDRD, PD2);
	SET(DDRB, PB0);
	SET(DDRB, PB1);
	SET(DDRB, PB2);
	SET(DDRB, PB4);
}

void	init_slave(void)
{
	TWAR = ADDRESS << 1;
	TWCR = (1 << TWEN) | (1 << TWEA);
}

void	slave_mode(void)
{
	uint8_t	is_press;

	SET(PORTB, PB4);
	uart_debug("Start slave ...\r\n");
	mystatus = 1;
	is_press = 0;
	CHANGECOLOR(1, COLORRED);
	while (1)
	{
		if ((PIND & (1 << PD2)) == 0)
		{
			CHANGECOLOR(1, COLORGREEN);
			TWCR = (1 << TWEN) | (1 << TWINT);
			is_press = 1;
		}
		else
			TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
		i2c_wait();
		if (is_press == 1)
			break ;
	}
	uart_debug("JE LEAVE LA BOUCLE EN TANT QUE SLAVE ...\r\n");
	SET(PORTB, PB2);
}

void	master_mode(void)
{
	uint8_t	status;

	SET(PORTB, PB0);
	uart_debug("Being master ... \r\n");
	i2c_start(TW_WRITE, ADDRESS);
	CHANGECOLOR(1, COLORRED);
	while (1) //check press button of slave
	{
		status = i2c_write(BUTTON_PRESS);
		if (status == TW_MT_DATA_NACK)
		{
			mystatus = 2;
			uart_debug("jai recu nack wtf\n");
			// _delay_ms(5000);
			break ;
		}
	}
	SET(PORTB, PB2);
	CHANGECOLOR(1, COLORGREEN);
	i2c_stop();
}

int	main(void)
{
	init_LED_SW1();
	uart_init();
	i2c_init();
	init_slave();
	while (1)
	{
		if (mystatus == 0 && (PIND & (1 << PD2)) == 0) // switch press == MASTER
			master_mode();
		//SLA+W received, ACK returned
		else if (mystatus == 0 && TW_STATUS == TW_SR_SLA_ACK) // SLAVE mode
			slave_mode();
		if (mystatus != 0)
		{
			uart_printstr(BGREEN);
			uart_printstr("GAME STARTING");
			uart_printstr(RESET);
			uart_newline();
			startgame(mystatus);
			_delay_ms(1000);
		}
	}
}
