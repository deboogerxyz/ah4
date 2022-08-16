#include "../config.h"
#include "../interfaces.h"

void visuals_revealRanks(UserCmd *cmd)
{
	if (config.visuals.revealRanks && cmd->buttons & IN_SCORE)
		interfaces.client->vmt->dispatchUserMessage(interfaces.client, 50, 0, 0, 0);
}
