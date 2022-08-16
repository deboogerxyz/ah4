#include "../config.h"
#include "../interfaces.h"
#include "../memory.h"

void visuals_disablePostProcessing(FrameStage stage)
{
	static ConVar *conVar = 0;
	if (!conVar)
		conVar = interfaces.cvar->vmt->findVar(interfaces.cvar, "mat_postprocess_enable");

	if (!conVar)
		return;

	conVar->vmt->setInt(conVar, stage == RENDER_START && !config.visuals.disablePostProcessing);
}

void visuals_disableShadows(void)
{
	static ConVar *conVar = 0;
	if (!conVar)
		conVar = interfaces.cvar->vmt->findVar(interfaces.cvar, "cl_csm_enabled");

	if (!conVar)
		return;

	conVar->vmt->setInt(conVar, !config.visuals.disableShadows);
}

void visuals_forceCrosshair(FrameStage stage)
{
	static ConVar *conVar = 0;
	if (!conVar)
		conVar = interfaces.cvar->vmt->findVar(interfaces.cvar, "weapon_debug_spread_show");

	if (!conVar)
		return;

	Entity *localPlayer = *memory.localPlayer;

	bool shouldForce = stage == RENDER_START && config.visuals.forceCrosshair && localPlayer && localPlayer->vmt->isAlive(localPlayer) && !Entity_isScoped(localPlayer);

	conVar->vmt->setInt(conVar, shouldForce ? 3 : 0);
}

void visuals_grenadePrediction(void)
{
	static ConVar *conVar = 0;
	if (!conVar)
		conVar = interfaces.cvar->vmt->findVar(interfaces.cvar, "cl_grenadepreview");

	if (!conVar)
		return;

	conVar->vmt->setInt(conVar, config.visuals.grenadePrediction);
}

void visuals_revealRanks(UserCmd *cmd)
{
	if (config.visuals.revealRanks && cmd->buttons & IN_SCORE)
		interfaces.client->vmt->dispatchUserMessage(interfaces.client, 50, 0, 0, 0);
}
