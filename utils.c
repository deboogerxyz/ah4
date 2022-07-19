#include <string.h>

#include "utils.h"

unsigned int hash(const char *data)
{
	unsigned int hashed = 0x811C9DC5;

	for (int i = 0; i < strlen(data); i++) {
		hashed ^= data[i];
		hashed *= 0x1000193;
	}

	return hashed;
}
