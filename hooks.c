#include <stdlib.h>
#include <string.h>

#include "hacks/enginePrediction.h"

#include "interfaces.h"
#include "memory.h"

#include "hooks.h"

#define HOOK(type, vmt) \
	{ old##type = vmt; \
	int len = getTableLength((void **)vmt); \
	new##type = malloc(len * sizeof(void *)); \
	if (new##type) { \
		memcpy(new##type, vmt, len * sizeof(void *)); \
		vmt = new##type; \
	} }

#define UNHOOK(type, vmt) \
	if (old##type) \
		vmt = old##type; \
	if (new##type) \
		free(new##type);

static PollEvent oldPollEvent;
static SwapWindow oldSwapWindow;

static ClientVMT *oldClientVMT, *newClientVMT;
static ClientModeVMT *oldClientModeVMT, *newClientModeVMT;

static int getTableLength(void **vmt)
{
	int i = 0;

	while (vmt[i])
		i++;

	return i;
}

static int pollEvent(SDL_Event *event)
{
	return oldPollEvent(event);
}

static void swapWindow(SDL_Window *window)
{
	oldSwapWindow(window);
}

static bool createMove(ClientMode *this, float inputSampleTime, UserCmd *cmd)
{
	bool result = oldClientModeVMT->createMove(this, inputSampleTime, cmd);
	if (!cmd->commandNumber)
		return result;

	sdk_getServerTime(cmd);

	enginePrediction_run(cmd);

	return 0;
}

static void frameStageNotify(Client *this, FrameStage stage)
{
	oldClientVMT->frameStageNotify(this, stage);
}

void hooks_init(void)
{
	oldPollEvent = *memory.pollEvent;
	*memory.pollEvent = pollEvent;

	oldSwapWindow = *memory.swapWindow;
	*memory.swapWindow = swapWindow;

	HOOK(ClientVMT, interfaces.client->vmt)
	newClientVMT->frameStageNotify = frameStageNotify;

	HOOK(ClientModeVMT, memory.clientMode->vmt)
	newClientModeVMT->createMove = createMove;
}

void hooks_cleanUp(void)
{
	UNHOOK(ClientVMT, interfaces.client->vmt)
	UNHOOK(ClientModeVMT, memory.clientMode->vmt)

	*memory.swapWindow = oldSwapWindow;
	*memory.pollEvent  = oldPollEvent;
}
