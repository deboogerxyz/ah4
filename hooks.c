#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "interfaces.h"

#include "hooks.h"
#include "sdk.h"

#define HOOK(vmt, oldVMT, newVMT) \
        { oldVMT = vmt; \
        int len = getTableLength((void **)vmt); \
        newVMT = malloc(len * sizeof(void *)); \
        if (newVMT) { \
        	memcpy(newVMT, vmt, len * sizeof(void *)); \
        	vmt = newVMT; \
	} }

#define UNHOOK(vmt, oldVMT, newVMT) \
        if (oldVMT) \
        	vmt = oldVMT; \
        if (newVMT) \
        	free(newVMT);

ClientVMT *oldClientVMT, *newClientVMT;
ClientModeVMT *oldClientModeVMT, *newClientModeVMT;

static int getTableLength(void **vmt)
{
	int i = 0;

	while (vmt[i])
		i++;

	return i;
}

bool createMove(ClientMode *this, float inputSampleTime, UserCmd *cmd)
{
	bool result = oldClientModeVMT->createMove(this, inputSampleTime, cmd);
	if (!cmd->commandNumber)
		return result;

	return 0;
}

void frameStageNotify(Client *this, FrameStage stage)
{
	oldClientVMT->frameStageNotify(this, stage);
}

void hooks_init(void)
{
	HOOK(interfaces.client->vmt, oldClientVMT, newClientVMT)
	newClientVMT->frameStageNotify = frameStageNotify;

	HOOK(memory.clientMode->vmt, oldClientModeVMT, newClientModeVMT)
	newClientModeVMT->createMove = createMove;
}

void hooks_cleanUp(void)
{
	UNHOOK(memory.clientMode->vmt, oldClientModeVMT, newClientModeVMT)
	UNHOOK(interfaces.client->vmt, oldClientVMT, newClientVMT)
}
