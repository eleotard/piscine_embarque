#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void	uart_init(void);
void	uart_tx(unsigned char c);
void	uart_printstr(const char *s);
char	uart_rx(void);
void	uart_putnbr(int64_t n);
void	uart_newline();

#define INPUT_SIZE 15

void set_timer_2() //bleu
{
	//mode 7- FAST PWM - 0 a 0xFF PAGE 164
	TCCR2A |= (1 << WGM21) | (1 << WGM20); 
	
	//prediv 1024 //PAGE 166
	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);

	//Clear OC2B on Compare Match, set OC2B at BOTTOM, (non-inverting mode).
	TCCR2A |= (1 << COM2B1); //PAGE 163
	OCR2B = 0;
}

void set_timer_0() //rouge et vert
{
	//mode 1 - Compte de 0 à 0xFF
	TCCR0A = 0;
	TCCR0B = 0;
	TCCR0A |= (1 << WGM00) | (1 << WGM01);

	//prediv 1024
	TCCR0B |= (1 << CS00);
	TCCR0B |= (1 << CS02);
	
	//OC0A/B = 0 lors de la détection d’égalité ;
	//OC0A/B = 1 lorsque le compteur passe à 0 (BOTTOM) (mode non-inversé)
	TCCR0A |= (1 << COM0B1);
	TCCR0A |= (1 << COM0A1);
	OCR0B = 0;
	OCR0A = 0;
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	OCR0B = r;
	OCR0A = g;
	OCR2B = b;
}

void init_rgb()
{
	set_timer_0();
	set_timer_2();
}

// int	check_char(char c, int i)
// {
// 	if (i == 0 && c != '#')
// 		return 1;
// 	else if (i != 0 && (!(c >= 'A' && c <= 'F') 
// 		&& !(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f')))
// 		return 1;
// 	else if (i == 7 && c != 'D')
// 		return 1;
// 	else if (i == 8 && (c != '6' && c != '7' && c != '8'))
// 		return 1;
// 	return 0;
// }

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

void	print_error_and_reset(char *color_str, int *i)
{
	(void)color_str;
	uart_newline();
	//uart_printstr("\x1b[2J\x1b[HERROR input: ");
	// if (*i >= INPUT_SIZE)
	// 	uart_printstr("ERROR input: too long");
	// else
	// {
	// 	uart_printstr("ERROR input: ");
	// 	uart_tx(color_str[*i]);
	// 	uart_printstr(" is not a correct caracter (must be in 0123456789ABCDEF or #)\n");
	// }
	uart_printstr("ERROR input.\n");
	uart_newline();
	*i = 0;
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
	set_rgb(red, green, blue);
}

void	init_process(char *color_str)
{
	//uart_printstr("\x1b[2J\x1b[HEnter a HEX RGB value, format #RRGGBB:\n");
	uart_printstr("Enter a HEX RGB value, format #RRGGBB:\n");
	reset_color_str(color_str);
}


int main()
{
	char	color_str[INPUT_SIZE]; //12 #FULLRAINBOW\n #221299D8\n //10
	uint8_t color_str_is_set = 0;
	int i = 0;

	DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);
	uart_init();
	init_rgb();
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
					print_error_and_reset(color_str, &i);
				else
					color_str_is_set = 1;
			}
			i++;
			if (i >= INPUT_SIZE) //bon
				print_error_and_reset(color_str, &i);
		}	
		else if (color_str_is_set)
		{
			convert_to_int_rgb(color_str);
			uart_newline();
			uart_printstr("The RGB color has been set :)\n\n");
			//_delay_ms(3000);
			color_str_is_set = 0;
			i = 0;
		}
	}
	return 0;
}
