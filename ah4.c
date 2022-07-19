#include "interfaces.h"
#include "memory.h"
#include "netvars.h"
#include "hooks.h"

int __attribute__((constructor)) onLoad(void)
{
	interfaces_init();
	memory_init();
	netvars_init();
	hooks_init();

	return 0;
}

void __attribute__((destructor)) onUnload(void)
{
	hooks_cleanUp();
	netvars_cleanUp();
}
