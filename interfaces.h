#include "sdk.h"

typedef struct {
	Client *client;
	Cvar *cvar;
	Engine *engine;
	EntityList *entityList;
	GameMovement *gameMovement;
} Interfaces;

extern Interfaces interfaces;

void interfaces_init(void);
