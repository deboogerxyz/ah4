#include "sdk.h"

typedef struct {
	Client *client;
	Cvar *cvar;
	Engine *engine;
	EngineTrace *engineTrace;
	EntityList *entityList;
	InputSystem *inputSystem;
	MaterialSystem *materialSystem;
	ModelRender *modelRender;
	GameMovement *gameMovement;
	Prediction *prediction;
	StudioRender *studioRender;
} Interfaces;

extern Interfaces interfaces;

void interfaces_init(void);
