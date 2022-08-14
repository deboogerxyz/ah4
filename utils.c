#include <string.h>

#include "utils.h"

unsigned int hash(const char *data)
{
	unsigned int hashed = 0x811C9DC5;

	while (*data) {
		hashed ^= *data++;
		hashed *= 0x1000193;
	}

	return hashed;
}
