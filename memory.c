#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <link.h>
#include <limits.h>
#include <unistd.h>

#include "interfaces.h"
#include "sdk.h"

#include "memory.h"

typedef struct LinkMap LinkMap;
struct LinkMap {
	ElfW(Addr) addr;
	const char *name;
	ElfW(Dyn) *ld;
	LinkMap *next;
	LinkMap *prev;
	LinkMap *real;
	long int ns;
	struct libname_list *moduleName;
	ElfW(Dyn) *info[DT_NUM + DT_VERSIONTAGNUM + DT_EXTRANUM + DT_VALNUM + DT_ADDRNUM];
	const ElfW(Phdr) *phdr;
	// ...
};

Memory memory;

static void *find(const char *moduleName, const char *pattern)
{
	LinkMap *linkMap = dlopen(moduleName, RTLD_NOLOAD | RTLD_NOW);
	if (!linkMap)
		return 0;

	char *start = (char *)linkMap->addr;
	char *end   = start + linkMap->phdr[0].p_memsz;

	dlclose(linkMap);

	const char *it = start;
	const char *patternIt = pattern;

	while (it < end && *patternIt)
		if (*it == *patternIt || *patternIt == '?') {
			it++;
			patternIt++;
		} else {
			it = ++start;
			patternIt = pattern;
		}

	if (!*patternIt)
		return start;

	return 0;
}

static void *relativeToAbsolute(void *addr)
{
	return addr + 4 + *(int *)addr;
}

void memory_init(void)
{
	void *tier0 = dlopen(TIER0_SO, RTLD_NOLOAD | RTLD_NOW);
	*(void **)&memory.debugMsg    = dlsym(tier0, "Msg");
	*(void **)&memory.conColorMsg = dlsym(tier0, "_Z11ConColorMsgRK5ColorPKcz");
	dlclose(tier0);

	void *sdl2 = dlopen("libSDL2-2.0.so.0", RTLD_LAZY | RTLD_NOLOAD);
	memory.pollEvent  = relativeToAbsolute(dlsym(sdl2, "SDL_PollEvent") + 2);
	memory.swapWindow = relativeToAbsolute(dlsym(sdl2, "SDL_GL_SwapWindow") + 2);
	dlclose(sdl2);

	memory.clientMode            = *(void **)relativeToAbsolute(relativeToAbsolute(*((void **)interfaces.client->vmt + 10) + 12) + 4);
	memory.input                 = **(void ***)relativeToAbsolute(*((void **)interfaces.client->vmt + 16) + 3);
	*(void **)&memory.isOtherEnemy = relativeToAbsolute(find(CLIENT_SO, "\xE8????\x84\xC0\x44\x89\xE2") + 1);
	*(void **)&memory.setClantag = relativeToAbsolute(find(ENGINE_SO, "\xE8????\xE9????\x66\x0F\x1F\x44??\x48\x8B\x7D\xB0") + 1);
	*(void **)&memory.globalVars = *(void **)relativeToAbsolute(*((void **)interfaces.client->vmt + 11) + 16);
	*(void **)&memory.glowObjectManager = (void *)relativeToAbsolute(relativeToAbsolute(find(CLIENT_SO, "\xE8????\x4C\x89\xE7\x8B\x70\x20") + 1) + 12);
	memory.moveData              = **(void ***)relativeToAbsolute(find(CLIENT_SO, "\x4C\x8B\x2D????\x0F\xB6\x93") + 3);
	memory.moveHelper            = **(void ***)relativeToAbsolute(find(CLIENT_SO, "\x48\x8B\x05????\x44\x89\x85????\x48\x8B\x38") + 3);
	*(void **)&memory.predictionRandomSeed = *(int **)relativeToAbsolute(find(CLIENT_SO, "\x41\x8D\x56\xFF\x31\xC9") - 14);
	memory.localPlayer           = relativeToAbsolute(find(CLIENT_SO, "\x83\xFF\xFF\x48\x8B\x05") + 6);
}
