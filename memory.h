#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>

#include "sdk.h"

typedef int (*PollEvent)(SDL_Event *);
typedef void (*SwapWindow)(SDL_Window *);

typedef struct {
	ClientMode *clientMode;
	void (*debugMsg)(const char *msg, ...);
	void (*conColorMsg)(Color *, const char *msg, ...);
	PollEvent *pollEvent;
	SwapWindow *swapWindow;
	bool (*isOtherEnemy)(Entity *, Entity *);
	void (*setClantag)(const char *, const char *);
	GlobalVars *globalVars;
	MoveData *moveData;
	MoveHelper *moveHelper;
	int *predictionRandomSeed;
	Entity **localPlayer;
} Memory;

extern Memory memory;

void memory_init(void);
