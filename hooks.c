#include <GL/glew.h>
#include <stdlib.h>
#include <string.h>

#include "hacks/enginePrediction.h"
#include "hacks/backtrack.h"
#include "hacks/misc.h"

#include "gui.h"
#include "interfaces.h"
#include "memory.h"

#include "hooks.h"

#define HOOK(type, vmt) \
	do { \
		old##type = vmt; \
		int len = getTableLength((void **)vmt); \
		new##type = malloc(len * sizeof(void *)); \
		if (new##type) { \
			memcpy(new##type, vmt, len * sizeof(void *)); \
			vmt = new##type; \
		} \
	} while (0);

#define UNHOOK(type, vmt) \
	do { \
		if (old##type) \
			vmt = old##type; \
		if (new##type) \
			free(new##type); \
	} while(0);

static PollEvent oldPollEvent;
static SwapWindow oldSwapWindow;

static ClientVMT *oldClientVMT, *newClientVMT;
static ClientModeVMT *oldClientModeVMT, *newClientModeVMT;
static NetworkChannelVMT *oldNetworkChannelVMT, *newNetworkChannelVMT;

static int getTableLength(void **vmt)
{
	int i = 0;

	while (vmt[i])
		i++;

	return i;
}

static int pollEvent(SDL_Event *event)
{
	int result = oldPollEvent(event);

	if (result && nk_sdl_handle_event(event) && gui_isOpen)
		event->type = 0;

	return result;
}

static void swapWindow(SDL_Window *window)
{
	static struct nk_context *ctx;
	static struct nk_font_atlas *atlas;
	static SDL_GLContext oldGLctx, GLctx;

	if (!ctx) {
		oldGLctx = SDL_GL_GetCurrentContext();
		GLctx = SDL_GL_CreateContext(window);

		glewInit();

		ctx = nk_sdl_init(window);

		nk_sdl_font_stash_begin(&atlas);
		nk_sdl_font_stash_end();

		nk_style_load_all_cursors(ctx, atlas->cursors);
	}

	SDL_GL_MakeCurrent(window, GLctx);

	gui_handleToggle(ctx);
	gui_render(ctx, window);

	nk_sdl_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);

	SDL_GL_MakeCurrent(window, oldGLctx);

	nk_input_begin(ctx);
	oldSwapWindow(window);
	nk_input_end(ctx);
}

static int sendDatagram(NetworkChannel *networkChannel, void *datagram)
{
	if (datagram || !interfaces.engine->vmt->isInGame(interfaces.engine))
		return oldNetworkChannelVMT->sendDatagram(networkChannel, datagram);

	int oldInReliableState = networkChannel->inReliableState;
	int oldInSequenceNumber = networkChannel->inSequenceNumber;

	backtrack_addLatency(networkChannel);

	int result = oldNetworkChannelVMT->sendDatagram(networkChannel, datagram);

	networkChannel->inReliableState = oldInReliableState;
	networkChannel->inSequenceNumber = oldInSequenceNumber;

	return result;
}

static bool createMove(ClientMode *this, float inputSampleTime, UserCmd *cmd)
{
	bool result = oldClientModeVMT->createMove(this, inputSampleTime, cmd);
	if (!cmd->commandNumber)
		return result;

	sdk_getServerTime(cmd);

	NetworkChannel *networkChannel = interfaces.engine->vmt->getNetworkChannel(interfaces.engine);
	if (networkChannel) {
		if (newNetworkChannelVMT != networkChannel->vmt) {
			HOOK(NetworkChannelVMT, networkChannel->vmt)
			newNetworkChannelVMT->sendDatagram = sendDatagram;
		}

		backtrack_updateSequences(networkChannel);
	}

	enginePrediction_run(cmd);

	backtrack_run(cmd);
	misc_jumpBug(cmd);

	return 0;
}

static void frameStageNotify(Client *this, FrameStage stage)
{
	static bool once = false;
	if (!once) {
		backtrack_init();
		once = true;
	}

	backtrack_update(stage);

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

	NetworkChannel *networkChannel = interfaces.engine->vmt->getNetworkChannel(interfaces.engine);
	if (networkChannel)
		UNHOOK(NetworkChannelVMT, networkChannel->vmt)

	UNHOOK(ClientModeVMT, memory.clientMode->vmt)

	*memory.swapWindow = oldSwapWindow;
	*memory.pollEvent  = oldPollEvent;
}
