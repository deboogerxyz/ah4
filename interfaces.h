#include "sdk.h"

typedef struct {
	Client *client;
	Engine *engine;
} Interfaces;

extern Interfaces interfaces;

void interfaces_init(void);
