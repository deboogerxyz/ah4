#include <dlfcn.h>

#include "sdk.h"

#include "interfaces.h"

Interfaces interfaces;

static void *find(const char *moduleName, const char *name)
{
	void *(*createInterface)(const char *name, int *returnCode);

	*(void **)&createInterface = dlsym(dlopen(moduleName, RTLD_NOLOAD | RTLD_LAZY), "CreateInterface");
	if (!createInterface)
		return 0;

	void *interface = createInterface(name, 0);

	return interface;
}

void interfaces_init(void)
{
	interfaces.client         = find(CLIENT_SO, "VClient018");
	interfaces.cvar           = find(VSTDLIB_SO, "VEngineCvar007");
	interfaces.engine         = find(ENGINE_SO, "VEngineClient014");
	interfaces.engineTrace    = find(ENGINE_SO, "EngineTraceClient004");
	interfaces.entityList     = find(CLIENT_SO, "VClientEntityList003");
	interfaces.inputSystem    = find(INPUTSYSTEM_SO, "InputSystemVersion001");
	interfaces.materialSystem = find(MATERIALSYSTEM_SO, "VMaterialSystem080");
	interfaces.modelRender    = find(ENGINE_SO, "VEngineModel016");
	interfaces.gameMovement   = find(CLIENT_SO, "GameMovement001");
	interfaces.prediction     = find(CLIENT_SO, "VClientPrediction001");
	interfaces.studioRender   = find(STUDIORENDER_SO, "VStudioRender026");
}
