#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "interfaces.h"

#include "hooks.h"
#include "sdk.h"

#define HOOK(vmt, oldVMT, newVMT) \
        oldVMT = vmt; \
        int len = getTableLength((void **)vmt); \
        newVMT = malloc(len * sizeof(void *)); \
        if (!newVMT) \
        	return; \
        memcpy(newVMT, (void **)vmt - 2, len * sizeof(void *)); \
        *(void **)&vmt = newVMT + 2;

#define UNHOOK(vmt, oldVMT, newVMT) \
        if (!newVMT) \
        	return; \
        *(void **)&vmt = oldVMT; \
        free(newVMT);

ClientVMT *oldClientVMT, **newClientVMT;

static int getTableLength(void **vmt)
{
	int i = 0;

	while (vmt[i])
		i++;

	return i;
}

bool createMove(ClientMode *this, float inputSampleTime, UserCmd *cmd)
{
	return 0;
}

void frameStageNotify(Client *this, FrameStage stage)
{
	oldClientVMT->frameStageNotify(this, stage);
}

void hooks_init(void)
{
	HOOK(interfaces.client->vmt, oldClientVMT, newClientVMT)

	interfaces.client->vmt->frameStageNotify = frameStageNotify;
}

void hooks_cleanUp(void)
{
	UNHOOK(interfaces.client->vmt, oldClientVMT, newClientVMT)
}
