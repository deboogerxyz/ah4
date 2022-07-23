#include "../interfaces.h"
#include "../memory.h"

#include "enginePrediction.h"

void enginePrediction_run(UserCmd *cmd)
{
	Entity *localPlayer = *memory.localPlayer;
	if (!localPlayer)
		return;

	*memory.predictionRandomSeed = 0;

	float oldCurrentTime = memory.globalVars->currentTime;
	float oldFrameTime   = memory.globalVars->frameTime;

	memory.globalVars->currentTime = sdk_getServerTime(cmd);
	memory.globalVars->frameTime   = memory.globalVars->intervalPerTick;

	interfaces.gameMovement->vmt->startTrackPredictionErrors(interfaces.gameMovement, localPlayer);
	memory.moveHelper->vmt->setHost(memory.moveHelper, localPlayer);
	interfaces.prediction->vmt->setupMove(interfaces.prediction, localPlayer, cmd, memory.moveHelper, memory.moveData);
	interfaces.gameMovement->vmt->processMovement(interfaces.gameMovement, localPlayer, memory.moveData);
	interfaces.prediction->vmt->finishMove(interfaces.prediction, localPlayer, cmd, memory.moveData);
	interfaces.gameMovement->vmt->finishTrackPredictionErrors(interfaces.gameMovement, localPlayer);
	memory.moveHelper->vmt->setHost(memory.moveHelper, 0);

	*memory.predictionRandomSeed = -1;

	memory.globalVars->currentTime = oldCurrentTime;
	memory.globalVars->frameTime   = oldFrameTime;

	Entity *activeWeapon = localPlayer->vmt->getActiveWeapon(localPlayer);
	if (!activeWeapon)
		return;

	activeWeapon->vmt->updateAccuracyPenalty(activeWeapon);
}
