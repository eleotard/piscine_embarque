#include "rush00.h"

void	int_to_hex_str(uint8_t data, char *data_str)
{
	data_str[0] = "0123456789ABCDEF"[(data >> 4) & 0x0F];
	data_str[1] = "0123456789ABCDEF"[data & 0x0F];
	data_str[2] = '\0';
}