#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <link.h>
#include <limits.h>
#include <unistd.h>

#include "interfaces.h"
#include "utils.h"

#include "memory.h"

typedef struct Module Module;
struct Module {
	const char *name;
	void *addr;
	size_t len;

	Module *next;
};

#define MODULES_LEN 200
Module *modules[MODULES_LEN] = {0};

Memory memory;

static int callback(struct dl_phdr_info *info, size_t size, void *addr)
{
	Module *module = malloc(sizeof(Module));
	if (!module)
		return 1;

	module->name = info->dlpi_name;
	module->addr = (void *)(info->dlpi_addr + info->dlpi_phdr[0].p_vaddr);
	module->len  = info->dlpi_phdr[0].p_memsz;

	int i = hash(info->dlpi_name) % MODULES_LEN;
	module->next = modules[i];
	modules[i] = module;

	return 0;
}

static void getAbsolutePath(char *dest, const char *src)
{
	snprintf(dest, PATH_MAX, "%s/%s", getcwd(0, 0), src);
}

static void *find(const char *moduleName, const char *pattern)
{
	char buf[PATH_MAX];
	getAbsolutePath(buf, moduleName);

	int i = hash(buf) % MODULES_LEN;

	Module *module = modules[i];
	while (module && strcmp(module->name, buf))
		module = module->next;

	if (!module)
		return 0;

	char *start = (char *)module->addr;
	char *end   = start + module->len;

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
	dl_iterate_phdr(callback, 0);

	void *tier0 = dlopen("libtier0_client.so", RTLD_NOLOAD | RTLD_NOW);
	*(void **)&memory.debugMsg = dlsym(tier0, "Msg");
	dlclose(tier0);

	*(void **)&memory.clientMode = *(void **)relativeToAbsolute(relativeToAbsolute(*((void **)interfaces.client->vmt + 10) + 12) + 4);
	*(void **)&memory.setClantag = relativeToAbsolute(find("bin/linux64/engine_client.so", "\xE8????\xE9????\x66\x0F\x1F\x44??\x48\x8B\x7D\xB0") + 1);
}

void memory_cleanUp(void)
{
	for (int i = 0; i < MODULES_LEN; i++) {
		Module *module = modules[i];
		while (module) {
			Module *next = module->next;
			free(module);
			module = next;
		}
	}
}
