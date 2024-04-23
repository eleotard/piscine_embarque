#include "rush00.h"

#define CYCLE 6
#define SLEEP 1
#define DELAY (uint32_t)((F_CPU * SLEEP) / CYCLE)

int	wait1sec(uint8_t mystatus)
{
	(void)mystatus;

	bool start = 0;
	for (uint32_t i = 0; i <= DELAY; ++i)
	{
		if (!(PIND & (1 << PD4)))
			start = 1;
	}
	return (start);
}

int	masterToSlave(uint8_t mystatus)
{
	(void)mystatus;
	CHANGECOLOR(1, COLORRED);
	uart_printstr("YOU CHEATED AS A MASTER\n");
	return (1);
}

int	slaveToMaster(uint8_t mystatus)
{
	(void)mystatus;
	CHANGECOLOR(1, COLORRED);
	uart_printstr("YOU CHEATED BITCH\n");
	return (1);
}

void	countdown(uint8_t mystatus)
{
	static const char	*colors[4] = {GREEN, YELLOW, MAGENTA, RED};
	int8_t				i;

	(void)mystatus;
	i = 2;
	DDRD |= (1 << 3) | (1 << 5) | (1 << 6);
	while (i >= 0)
	{
		if (wait1sec(mystatus) == 1)
		{
			uart_printstr(IRED);
			uart_printstr("YOU LOST");
			uart_printstr(RESET);
			uart_printstr("\n");
			CHANGECOLOR(1, COLORRED);
			if (mystatus == SLAVE)
				slaveToMaster(mystatus);
			if (mystatus == MASTER)
				masterToSlave(mystatus);
		};
		uart_printstr("Countdown: ");
		uart_printstr(colors[i]);
		uart_putnbr(i);
		uart_printstr(RESET);
		uart_printstr("\n");
		if (i == 3)
			PORTB = (1 << PB4);
		else
			PORTB = (1 << i);
		--i;
	}
	CHANGECOLOR(1, COLORBLUE);
	uart_printstr("GO!\n");
}

void	gamepress(uint8_t mystatus)
{
	uint8_t	status;

	if (mystatus == SLAVE)
	{
		uart_printstr(BBLUE);
		uart_printstr("<<<<<<<I AM THE SLAVEEEEEEEEEEEEE>>>>>>>");
		uart_printstr(RESET);
		uart_printstr("\n");
		while (1)
		{
			if ((PIND & (1 << PD4)) == 0)
			{
				CHANGECOLOR(1, COLORGREEN);
				uart_printstr("YOU WON\n");
				TWCR = (1 << TWEN) | (1 << TWINT);
				i2c_wait();
				return ;
			}
			else
			{
				i2c_wait();
				if (TW_STATUS == 0xA0) //STO
				{
					TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
					break ;
				}
				TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
			}
		};
	}
	else if (mystatus == MASTER)
	{
		uart_printstr(BCYAN);
		uart_printstr("<<<<<<<I AM THE MASTERRRRRRRRRRRRRRRRRR>>>>>>>");
		uart_printstr(RESET);
		uart_printstr("\n");
		i2c_start(TW_WRITE, ADDRESS);
		while (1) //check press button of slave
		{
			status = i2c_write(BUTTON_PRESS);
			if (status == TW_MT_DATA_NACK)
				break ;
			if (!(PIND & (1 << PD4)))
			{
				CHANGECOLOR(1, COLORGREEN);
				uart_printstr("YOU WON\n");
				i2c_stop();
				i2c_start(0, ADDRESS);
				i2c_write(0x11);
				i2c_stop();
				return ;
			}
		}
		i2c_stop();
	}
	CHANGECOLOR(1, COLORRED);
	uart_printstr(">>>>>>YOU LOST\n");
}

void	startgame(uint8_t mystatus)
{
	uart_printstr("Game started\n");
	countdown(mystatus);
	//
	gamepress(mystatus);
}