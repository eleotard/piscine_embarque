#ifndef RUSH00_H
# define RUSH00_H

#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <util/twi.h>
#include "Colors.h"

#define UART_BAUDRATE (uint32_t)115200
#define FREQ_SCL 100000 //100kHz <=> 100000 cycles/sec
#define PRESCALER 1
#define SLAVE_ADDR 0x38
#define CHANGECOLOR(x, y) PORTD = (x << y)
#define COLORRED 5
#define COLORGREEN 6
#define COLORBLUE 3
#define DEBUG 0
#define ADDRESS 0x18
#define BUTTON_PRESS 0x10
#define SET(PORT, PIN) PORT |= (1 << PIN)
#define RESETPIN(PORT, PIN) PORT &= ~(1 << PIN)

#define SLAVE 1
#define MASTER 2

#define LOST 1
#define WIN 2

// uart
void	uart_init(void);
void	uart_tx(unsigned char c);
void	uart_debug(const char *s);
void	uart_printstr(const char *s);
void	uart_putnbr(int64_t n);
void	uart_newline(void);

// i2c
void	i2c_init(void);
void	i2c_read(void);
uint8_t	i2c_write(unsigned char data);
void i2c_wait();
uint8_t	i2c_start(uint8_t read, uint8_t address);
void	i2c_stop(void);

// utils
void	int_to_hex_str(uint8_t data, char *data_str);

// game
// void countdown();
void	startgame(uint8_t mystatus);

#endif