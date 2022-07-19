#include "sdk.h"

typedef struct {
	Client *client;
} Interfaces;

extern Interfaces interfaces;

void interfaces_init(void);
