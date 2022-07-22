#include "sdk.h"

typedef struct {
	ClientMode *clientMode;
	void (*debugMsg)(const char *msg, ...);
	void (*conColorMsg)(Color *, const char *msg, ...);
	void (*setClantag)(const char *, const char *);
	Entity **localPlayer;
} Memory;

extern Memory memory;

void memory_init(void);
