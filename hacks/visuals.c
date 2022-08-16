#include "../config.h"
#include "../interfaces.h"

void visuals_disablePostProcessing(FrameStage stage)
{
	static ConVar *conVar;
	if (!conVar)
		conVar = interfaces.cvar->vmt->findVar(interfaces.cvar, "mat_postprocess_enable");

	if (!conVar)
		return;

	conVar->vmt->setInt(conVar, stage == RENDER_START && !config.visuals.disablePostProcessing);
}

void visuals_revealRanks(UserCmd *cmd)
{
	if (config.visuals.revealRanks && cmd->buttons & IN_SCORE)
		interfaces.client->vmt->dispatchUserMessage(interfaces.client, 50, 0, 0, 0);
}
