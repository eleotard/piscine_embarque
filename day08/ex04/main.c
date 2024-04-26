#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void	uart_init(void);
void	uart_tx(unsigned char c);
void	uart_printstr(const char *s);
char	uart_rx(void);
void	uart_putnbr(int64_t n);
void	uart_newline();

void 	SPI_init();
void 	SPI_masterTransmitByte(uint8_t data);
void	send_colordata_to_D6(uint8_t r, uint8_t g, uint8_t b);
void	send_colordata_to_D7(uint8_t r, uint8_t g, uint8_t b);
void	send_colordata_to_D8(uint8_t r, uint8_t g, uint8_t b);

#define INPUT_SIZE 15

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	send_colordata_to_D6(r, g, b);
	send_colordata_to_D7(r, g, b);
	send_colordata_to_D8(r, g, b);
}

void wheel(uint8_t pos)
{
	pos = 255 - pos;
	if (pos < 85)
	{
		set_rgb(255 - pos * 3, 0, pos * 3); //rouge au vert
	}
	else if (pos < 170)
	{
		pos = pos - 85;
		set_rgb(0, pos * 3, 255 - pos * 3); //vert  au bleu
	}
	else
	{
		pos = pos - 170;
		set_rgb(pos * 3, 255 - pos * 3, 0); // bleu au rouge
	}
}

uint8_t ft_strlen(char *str)
{
	uint8_t i = 0;
	while (str[i])
		i++;
	return i;
}

uint8_t	ft_strcmp(char *s1, char *s2)
{
	uint8_t i = 0;
	while ((s1[i] && s2[i]) && (i < INPUT_SIZE - 1))
	{
		if (s1[i] != s2[i])
			return 1;
		i++;
	}
	return 0;
}

int	check_input(char *color_str)
{
	uint8_t str_len = ft_strlen(color_str);
	if ((str_len != 12) && str_len != 9)
		return 1;
	if (str_len == 12 && !ft_strcmp(color_str, "#FULLRAINBOW"))
		return 0;
	if (color_str[0] != '#')
		return 1;
	for (int i = 1; i < 9; i++)
	{
		char c = color_str[i];
		if (!(c >= 'A' && c <= 'F') && !(c >= '0' && c <= '9') 
			&& !(c >= 'a' && c <= 'f'))
			return 1;
	}
	if (color_str[7] != 'D')
		return 1;
	if (color_str[8] != '6' && color_str[8] != '7' && color_str[8] != '8')
		return 1;
	return 0;
}

void	reset_color_str(char *color_str)
{
	int index = 0;
	while (++index < INPUT_SIZE)
		color_str[index] = '\0';
}

void	print_error()
{
	uart_newline();
	uart_printstr("ERROR input.\n");
	uart_newline();
}

void	fill_color(char c1, char c2, uint8_t *color)
{
	if (c1 >= '0' && c1 <= '9')
        *color = c1 - '0';
	else if ((c1 >= 'A' && c1 <= 'F') || (c1 >= 'a' && c1 <= 'f'))
        *color = c1 - 'A' + 10;
	*color = *color << 4;
	if (c2 >= '0' && c2 <= '9')
        *color |= c2 - '0';
	else if ((c1 >= 'A' && c1 <= 'F') || (c1 >= 'a' && c1 <= 'f'))
        *color |= c2 - 'A' + 10;
}

void	convert_to_int_rgb(char *color_str)
{
	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;

	fill_color(color_str[1], color_str[2], &red);
	fill_color(color_str[3], color_str[4], &green);
	fill_color(color_str[5], color_str[6], &blue);
	if (color_str[8] == '6')
		send_colordata_to_D6(red, green, blue);
	else if (color_str[8] == '7')
		send_colordata_to_D7(red, green, blue);
	else if (color_str[8] == '8')
		send_colordata_to_D8(red, green, blue);
}

void	init_process(char *color_str)
{
	//uart_printstr("\x1b[2J\x1b[HEnter a HEX RGB value, format #RRGGBB:\n");
	uart_printstr("Enter a HEX RGB value, format #RRGGBB:\n");
	reset_color_str(color_str);
}

void	full_rainbow()
{
	uint8_t i = 0;
	uint8_t pos = 0;
	while (1)
	{
		wheel(pos);
		pos++;
		if (pos == 255)
		{
			pos = 0;
			i++;
		}
		if (i > 5)
			break;
	}
	send_colordata_to_D6(0, 0, 0);
	send_colordata_to_D7(0, 0, 0);
	send_colordata_to_D8(0, 0, 0);
}

int main()
{
	char	color_str[INPUT_SIZE]; //12 #FULLRAINBOW\n #221299D8\n //10
	uint8_t color_str_is_set = 0;
	int i = 0;

	uart_init();
	SPI_init();
	while (1)
	{
		if (!color_str_is_set)
		{
			if (i == 0)
				init_process(color_str);
			color_str[i] = uart_rx();
			uart_tx(color_str[i]);
			if (color_str[i] == '\r')
			{
				color_str[i] = '\0';
				if (check_input(color_str))
					print_error();
				else
					color_str_is_set = 1;
				i = -1;
			}
			i++;
			if (i >= INPUT_SIZE)
			{
				print_error();
				i = 0;
			}
		}	
		else if (color_str_is_set)
		{
			uart_newline();
			uart_printstr("The RGB color has been set :)\n\n");
			if (ft_strlen(color_str) == 12) //FULLRAINBOW
				full_rainbow();
			else
				convert_to_int_rgb(color_str);
			color_str_is_set = 0;
			i = 0;
		}
	}
	return 0;
}
