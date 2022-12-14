#include <GL/glew.h>
#include <stdlib.h>
#include <string.h>

#include "hacks/backtrack.h"
#include "hacks/chams.h"
#include "hacks/enginePrediction.h"
#include "hacks/glow.h"
#include "hacks/legitbot.h"
#include "hacks/misc.h"
#include "hacks/skinChanger.h"
#include "hacks/visuals.h"

#include "config.h"
#include "gui.h"
#include "interfaces.h"
#include "memory.h"
#include "sdk.h"
#include "utils.h"

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
static EngineVMT *oldEngineVMT, *newEngineVMT;
static ModelRenderVMT *oldModelRenderVMT, *newModelRenderVMT;

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

	if (gui_isOpen) {
		gui_render(ctx, window);

		nk_sdl_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
	}

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

	void *rbp;
	__asm__ volatile ("mov %%rbp, %0" : "=r" (rbp));

	bool *sendPacket = *(bool **)rbp - 24;
	(void)sendPacket;

	sdk_getServerTime(cmd);

	NetworkChannel *networkChannel = interfaces.engine->vmt->getNetworkChannel(interfaces.engine);
	if (networkChannel) {
		if (newNetworkChannelVMT != networkChannel->vmt) {
			HOOK(NetworkChannelVMT, networkChannel->vmt)
			newNetworkChannelVMT->sendDatagram = sendDatagram;
		}

		backtrack_updateSequences(networkChannel);
	}

	Vector currentAngles = cmd->viewAngles;

	misc_antiAfkKick(cmd);
	misc_fastDuck(cmd);
	misc_slideWalk(cmd);
	misc_bunnyHop(cmd);
	visuals_revealRanks(cmd);

	enginePrediction_run(cmd);

	legitbot_run(cmd);
	backtrack_run(cmd);
	misc_jumpBug(cmd);
	misc_edgeJump(cmd);

	cmd->viewAngles = Vector_normalize(cmd->viewAngles);
	misc_fixMovement(cmd, currentAngles.y);

	cmd->viewAngles.x = CLAMP(cmd->viewAngles.x, -89, 89);
	cmd->viewAngles.y = CLAMP(cmd->viewAngles.y, -180, 180);
	cmd->viewAngles.z = 0;

	return 0;
}

static void doPostScreenEffects(ClientMode *this, void *param)
{
	glow_render();

	oldClientModeVMT->doPostScreenEffects(this, param);
}

static void frameStageNotify(Client *this, FrameStage stage)
{
	static bool once = false;
	if (!once) {
		backtrack_init();
		once = true;
	}

	visuals_disablePostProcessing(stage);
	visuals_disableShadows();
	visuals_forceCrosshair(stage);
	visuals_grenadePrediction();
	backtrack_update(stage);

//	if (stage == NET_UPDATE_POSTDATAUPDATE_START)
//		skinChanger_run();

	oldClientVMT->frameStageNotify(this, stage);
}

static DemoPlaybackParameters *getDemoPlaybackParameters(Engine *this)
{
	DemoPlaybackParameters *oldParams = oldEngineVMT->getDemoPlaybackParameters(this);

	if (!oldParams || !config.visuals.revealOverwatch || __builtin_return_address(0) == memory.demoFileEndReached)
		return oldParams;

	static DemoPlaybackParameters newParams;
	newParams = *oldParams;
	newParams.anonymousPlayerIdentity = false;

	return &newParams;
}

static void drawModelExecute(ModelRender *this, void *ctx, void *state, ModelRenderInfo *info, Matrix3x4 *customBoneToWorld)
{
	if (StudioRender_isForcedMaterialOverride()) {
		oldModelRenderVMT->drawModelExecute(this, ctx, state, info, customBoneToWorld);
		return;
	}

	if (!chams_render(ctx, state, info, customBoneToWorld, oldModelRenderVMT))
		oldModelRenderVMT->drawModelExecute(this, ctx, state, info, customBoneToWorld);

	interfaces.studioRender->vmt->forcedMaterialOverride(interfaces.studioRender, 0, OverrideType_Normal, -1);
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
	newClientModeVMT->doPostScreenEffects = doPostScreenEffects;

	HOOK(EngineVMT, interfaces.engine->vmt)
	newEngineVMT->getDemoPlaybackParameters = getDemoPlaybackParameters;

	HOOK(ModelRenderVMT, interfaces.modelRender->vmt)
	newModelRenderVMT->drawModelExecute = drawModelExecute;
}

void hooks_cleanUp(void)
{
	UNHOOK(ClientVMT, interfaces.client->vmt)

	NetworkChannel *networkChannel = interfaces.engine->vmt->getNetworkChannel(interfaces.engine);
	if (networkChannel)
		UNHOOK(NetworkChannelVMT, networkChannel->vmt)

	UNHOOK(ClientModeVMT, memory.clientMode->vmt)
	UNHOOK(EngineVMT, interfaces.engine->vmt)
	UNHOOK(ModelRenderVMT, interfaces.modelRender->vmt)

	*memory.swapWindow = oldSwapWindow;
	*memory.pollEvent  = oldPollEvent;
}
