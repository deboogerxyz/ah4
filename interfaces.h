#include "sdk.h"

typedef struct {
	Client *client;
	Cvar *cvar;
	Engine *engine;
} Interfaces;

extern Interfaces interfaces;

void interfaces_init(void);
