#include "memory.h"
#include "netvars.h"

#include "sdk.h"

float sdk_getServerTime(UserCmd *cmd)
{
	static int tick;
	static UserCmd *lastCmd;

	if (cmd) {
		Entity *localPlayer = *memory.localPlayer;
		if (localPlayer && (!lastCmd || lastCmd->hasBeenPredicted))
			tick = *(int *)(localPlayer + netvars_getOffset("CBasePlayer->m_nTickBase"));
		else
			tick++;

		lastCmd = cmd;
	}

	return tick * memory.globalVars->intervalPerTick;
}
