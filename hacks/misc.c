#include "enginePrediction.h"

#include "../config.h"
#include "../keyBinds.h"
#include "../memory.h"
#include "../netvars.h"
#include "../utils.h"

#include "misc.h"

void misc_antiAfkKick(UserCmd *cmd)
{
	if (config.misc.antiAfkKick && cmd->commandNumber % 2)
		cmd->buttons |= 1 << 27;
}

void misc_fastDuck(UserCmd *cmd)
{
	if (config.misc.fastDuck)
		cmd->buttons |= IN_BULLRUSH;
}

void misc_slideWalk(UserCmd *cmd)
{
	Entity *localPlayer = *memory.localPlayer;
	if (!localPlayer)
		return;

	MoveType moveType = *Entity_moveType(localPlayer);

	if (config.misc.slideWalk && moveType != MoveType_Ladder)
		cmd->buttons ^= IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT;
}

void misc_bunnyHop(UserCmd *cmd)
{
	static bool wasOnGround = false;

	if (!config.misc.bunnyHop)
		return;

	if (config.misc.jumpBug && keyBinds_isOn(&config.misc.jumpBugKeyBind))
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

void
misc_fixMovement(UserCmd *cmd, float yaw)
{
	if (!config.misc.fixMovement)
		return;

	float oldYaw = yaw + (yaw < 0 ? 360 : 0);
	float newYaw = cmd->viewAngles.y + (cmd->viewAngles.y < 0 ? 360 : 0);
	float delta  = newYaw < oldYaw ? fabsf(newYaw - oldYaw) : 360 - fabsf(newYaw - oldYaw);

	delta = 360 - delta;

	float forwardMove = cmd->forwardMove;
	float sideMove    = cmd->sideMove;

	cmd->forwardMove = cos(DEG2RAD(delta)) * forwardMove + cos(DEG2RAD(delta + 90)) * sideMove;
	cmd->sideMove    = sin(DEG2RAD(delta)) * forwardMove + sin(DEG2RAD(delta + 90)) * sideMove;
}
