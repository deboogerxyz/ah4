#include "enginePrediction.h"

#include "../config.h"
#include "../keyBinds.h"
#include "../memory.h"
#include "../netvars.h"

#include "misc.h"

void misc_antiAfk(UserCmd *cmd)
{
	if (config.misc.antiAfk && cmd->commandNumber % 2)
		cmd->buttons |= 1 << 27;
}

void misc_bunnyHop(UserCmd *cmd)
{
	static bool wasOnGround = false;

	if (!config.misc.bunnyHop)
		return;

	Entity *localPlayer = *memory.localPlayer;
	if (!localPlayer)
		return;

	MoveType moveType = *Entity_moveType(localPlayer);

	if (!(*Entity_flags(localPlayer) & 1) && moveType != MoveType_Ladder && moveType != MoveType_NoClip && !wasOnGround)
		cmd->buttons &= ~IN_JUMP;

	wasOnGround = *Entity_flags(localPlayer) & 1;
}

void misc_jumpBug(UserCmd *cmd)
{
	if (!config.misc.jumpBug)
		return;

	if (!keyBinds_isOn(&config.misc.jumpBugKeyBind))
		return;

	Entity *localPlayer = *memory.localPlayer;
	if (!localPlayer)
		return;

	if (!localPlayer->vmt->isAlive(localPlayer))
		return;

	MoveType moveType = *Entity_moveType(localPlayer);
	if (moveType == MoveType_Ladder || moveType == MoveType_NoClip)
		return;

	int flags = *Entity_flags(localPlayer);
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

	if (!keyBinds_isOn(&config.misc.edgeJumpKeyBind))
		return;

	Entity *localPlayer = *memory.localPlayer;
	if (!localPlayer)
		return;

	if (!localPlayer->vmt->isAlive(localPlayer))
		return;

	MoveType moveType = *Entity_moveType(localPlayer);
	if (moveType == MoveType_Ladder || moveType == MoveType_NoClip)
		return;

	int flags = *Entity_flags(localPlayer);
	if ((enginePrediction_flags & 1) && !(flags & 1))
		cmd->buttons |= IN_JUMP;
}
