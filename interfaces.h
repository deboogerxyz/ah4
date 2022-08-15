#include "sdk.h"

typedef struct {
	Client *client;
	Cvar *cvar;
	Engine *engine;
	EngineTrace *engineTrace;
	EntityList *entityList;
	InputSystem *inputSystem;
	GameMovement *gameMovement;
	Prediction *prediction;
} Interfaces;

extern Interfaces interfaces;

void interfaces_init(void);
