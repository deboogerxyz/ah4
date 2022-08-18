#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>

#include "sdk.h"

typedef int (*PollEvent)(SDL_Event *);
typedef void (*SwapWindow)(SDL_Window *);

typedef struct {
	ClientMode *clientMode;
	Input *input;
	void (*debugMsg)(const char *msg, ...);
	void (*conColorMsg)(Color8 *, const char *msg, ...);
	PollEvent *pollEvent;
	SwapWindow *swapWindow;
	bool (*isOtherEnemy)(Entity *, Entity *);
	void (*setClantag)(const char *, const char *);
	bool (*lineGoesThroughSmoke)(Vector, Vector, short);
	GlobalVars *globalVars;
	GlowObjectManager *glowObjectManager;
	void *demoFileEndReached;
	int *predictionRandomSeed;
	MoveData *moveData;
	MoveHelper *moveHelper;
	KeyValues *(*keyValuesFromString)(const char *name, const char *value, const char **);
	KeyValues *(*keyValuesFindKey)(KeyValues *, const char *name, bool create);
	void (*keyValuesSetString)(KeyValues *, const char *value);
	Entity **localPlayer;
} Memory;

extern Memory memory;

void memory_init(void);
