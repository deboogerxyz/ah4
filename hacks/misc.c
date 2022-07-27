#include "enginePrediction.h"

#include "../config.h"
#include "../keyBinds.h"
#include "../memory.h"
#include "../netvars.h"

#include "misc.h"

void misc_jumpBug(UserCmd *cmd)
{
	if (!config.misc.jumpBug)
		return;

	if (!keyBinds_isOn(config.misc.jumpBugKeyBind))
		return;

	Entity *localPlayer = *memory.localPlayer;
	if (!localPlayer)
		return;

	if (!localPlayer->vmt->isAlive(localPlayer))
		return;

	int flags = *(int *)((char *)localPlayer + netvars_getOffset("CBasePlayer->m_fFlags"));

	if (flags & 1) {
		if (!(enginePrediction_flags & 1))
			cmd->buttons |= IN_DUCK;
		cmd->buttons &= ~IN_JUMP;
	}
}

void misc_edgeJump(UserCmd *cmd)
{
	if (!config.misc.edgeJump)
		return;

	if (!keyBinds_isOn(config.misc.edgeJumpKeyBind))
		return;

	Entity *localPlayer = *memory.localPlayer;
	if (!localPlayer)
		return;

	if (!localPlayer->vmt->isAlive(localPlayer))
		return;

	int flags = *(int *)((char *)localPlayer + netvars_getOffset("CBasePlayer->m_fFlags"));

	if ((enginePrediction_flags & 1) && !(flags & 1))
		cmd->buttons |= IN_JUMP;
}
