#include "sdk.h"

typedef struct {
	ClientMode *clientMode;
	void (*debugMsg)(const char *msg, ...);
	void (*setClantag)(const char *, const char *);
} Memory;

extern Memory memory;

void memory_init(void);
void memory_cleanUp(void);
