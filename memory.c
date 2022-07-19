#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <link.h>
#include <limits.h>
#include <unistd.h>

#include "interfaces.h"

#include "memory.h"

#define DT_THISPROCNUM 0

typedef struct LinkMap LinkMap;
struct LinkMap {
	ElfW(Addr) addr;
	const char *name;
	ElfW(Dyn) *ld;
	LinkMap *next;
	LinkMap *prev;
	LinkMap *real;
	Lmid_t ns;
	struct libname_list *libraryNames;
	ElfW(Dyn) *info[DT_NUM + DT_THISPROCNUM + DT_VERSIONTAGNUM + DT_EXTRANUM + DT_VALNUM + DT_ADDRNUM];
	const ElfW(Phdr) *phdr;
	// ...
};

Memory memory;

static void *find(const char *moduleName, const char *pattern)
{
	LinkMap *linkMap = dlopen(moduleName, RTLD_NOLOAD | RTLD_NOW);

	char *start = (char *)linkMap->real->addr;
	char *end   = start + linkMap->real->phdr[0].p_memsz;

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
	void *tier0 = dlopen("libtier0_client.so", RTLD_NOLOAD | RTLD_NOW);
	*(void **)&memory.debugMsg = dlsym(tier0, "Msg");
	dlclose(tier0);

	*(void **)&memory.clientMode = *(void **)relativeToAbsolute(relativeToAbsolute(*((void **)interfaces.client->vmt + 10) + 12) + 4);
	*(void **)&memory.setClantag = relativeToAbsolute(find("engine_client.so", "\xE8????\xE9????\x66\x0F\x1F\x44??\x48\x8B\x7D\xB0") + 1);

}
