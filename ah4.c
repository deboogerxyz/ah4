#include <dlfcn.h>
#include <pthread.h>

#include "config.h"
#include "interfaces.h"
#include "memory.h"
#include "netvars.h"
#include "hooks.h"

static pthread_t thread;

static void *routine(void *arg)
{
	struct timespec time = {0, 100 * 1000000}; // 100 ms
	while (!dlopen(SERVERBROWSER_SO, RTLD_NOLOAD | RTLD_NOW))
		nanosleep(&time, &time);

	interfaces_init();
	memory_init();
	netvars_init();
	hooks_init();

	config_reset();

	return 0;
}

int __attribute__((constructor)) onLoad(void)
{
	pthread_create(&thread, 0, routine, 0);

	return 0;
}

void __attribute__((destructor)) onUnload(void)
{
	pthread_join(thread, 0);

	hooks_cleanUp();
	netvars_cleanUp();
}
