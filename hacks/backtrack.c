#include <math.h>

#include "../libs/cvector.h"

#include "../config.h"
#include "../interfaces.h"
#include "../memory.h"
#include "../utils.h"

#include "backtrack.h"

#define TIME2TICKS(x) ((int)(0.5f + (x) / memory.globalVars->intervalPerTick))

typedef struct {
	int reliableState;
	int sequenceNumber;
	float serverTime;
} IncomingSequence;

static ConVar *updateRate, *maxUpdateRate, *interp, *interpRatio, *minInterpRatio, *maxInterpRatio, *maxUnlag;
static Record *records[65] = {0};
static IncomingSequence *inSequences = {0};

static float getLerp(void)
{
	NetworkChannel *networkChannel = interfaces.engine->vmt->getNetworkChannel(interfaces.engine);
	if (!networkChannel)
		return 0;

	float ratio = CLAMP(interpRatio->vmt->getFloat(interpRatio),
	                    minInterpRatio->vmt->getFloat(minInterpRatio),
			    maxInterpRatio->vmt->getFloat(maxInterpRatio));

	return MAX(interp->vmt->getFloat(interp),
	           (ratio / (maxUpdateRate ? maxUpdateRate->vmt->getFloat(maxUpdateRate) : updateRate->vmt->getFloat(updateRate))));
}

static bool isValid(float simTime)
{
	NetworkChannel *networkChannel = interfaces.engine->vmt->getNetworkChannel(interfaces.engine);
	if (!networkChannel)
		return false;

	int deadTime = (int)(sdk_getServerTime(0) - maxUnlag->vmt->getFloat(maxUnlag));
	if (simTime < deadTime)
		return false;

	float latencyOut = networkChannel->vmt->getLatency(networkChannel, 0);
	float latencyIn  = networkChannel->vmt->getLatency(networkChannel, 1);

	float delta = CLAMP(latencyOut + latencyIn + getLerp(), 0, maxUnlag->vmt->getFloat(maxUnlag)) - (sdk_getServerTime(0) - simTime);

	return fabs(delta) <= 0.2f;
}

void backtrack_update(FrameStage stage)
{
	if (!config.backtrack.enabled)
		return;

	if (stage != RENDER_START)
		return;

	if (!interfaces.engine->vmt->isInGame(interfaces.engine))
		return;

	Entity *localPlayer = *memory.localPlayer;
	if (!localPlayer)
		return;

	int maxClients = interfaces.engine->vmt->getMaxClients(interfaces.engine);
	for (int i = 1; i <= maxClients; i++) {
		Entity *entity = interfaces.entityList->vmt->getEntity(interfaces.entityList, i);
		if (!entity)
			continue;

		if (!memory.isOtherEnemy(localPlayer, entity))
			continue;

		if (*Entity_immunity(entity))
			continue;

		if (!entity->vmt->isAlive(entity))
			continue;

		Networkable *networkable = (Networkable *)(entity + 2);
		if (networkable->vmt->isDormant(networkable))
			continue;

		float simTime = *Entity_simTime(entity);

		if (!cvector_empty(records[i]) && cvector_end(records[i])->simTime == simTime)
			continue;

		Record record = {
			.entity  = entity,
			.simTime = simTime
		};

		Renderable *renderable = (Renderable *)(entity + 1);
		renderable->vmt->setupBones(renderable, record.boneMatrix, 256, 256, 0);

		cvector_push_back(records[i], record);

		while (cvector_size(records[i]) > TIME2TICKS((float)config.backtrack.timeLimit / 1000.0f))
			cvector_erase(records[i], 0);

		for (int j = 0; j < cvector_size(records[i]); j++)
			if (!isValid(records[i][j].simTime))
				cvector_erase(records[i], j);
	}
}

Record *backtrack_getClosestRecord(UserCmd *cmd)
{
	if (!config.backtrack.enabled)
		return 0;

	Entity *localPlayer = *memory.localPlayer;
	if (!localPlayer)
		return 0;

	if (!localPlayer->vmt->isAlive(localPlayer))
		return 0;

	Vector eyePos     = localPlayer->vmt->getEyePosition(localPlayer);
	Vector aimPunch   = localPlayer->vmt->getAimPunch(localPlayer);
	Vector viewAngles = Vector_add(cmd->viewAngles, aimPunch);

	Entity *bestEntity = 0;
	int bestEntityIndex = 0, bestRecordIndex = 0;
	float bestFov = 255.0f;

	int maxClients = interfaces.engine->vmt->getMaxClients(interfaces.engine);
	for (int i = 1; i <= maxClients; i++) {
		Entity *entity = interfaces.entityList->vmt->getEntity(interfaces.entityList, i);
		if (!entity)
			continue;

		if (!memory.isOtherEnemy(localPlayer, entity))
			continue;

		if (*Entity_immunity(entity))
			continue;

		if (!entity->vmt->isAlive(entity))
			continue;

		if (cvector_empty(records[i]))
			continue;

		for (int j = 0; j < cvector_size(records[i]); j++) {
			Record *record = &records[i][j];
			if (!record)
				continue;

			if (!isValid(record->simTime))
				continue;

			Vector headPos = Matrix3x4_origin(record->boneMatrix[8]);
			Vector angle   = Vector_calculateAngle(eyePos, headPos, viewAngles);

			float fov = hypotf(angle.x, angle.y);

			if (fov < bestFov) {
				bestFov         = fov;
				bestEntity      = entity;
				bestEntityIndex = i;
				bestRecordIndex = j;
			}
		}
	}

	if (!bestEntity)
		return 0;

	return &records[bestEntityIndex][bestRecordIndex];
}

void backtrack_run(UserCmd *cmd)
{
	if (!config.backtrack.enabled)
		return;

	if (!(cmd->buttons & IN_ATTACK))
		return;

	Record *record = backtrack_getClosestRecord(cmd);
	if (!record)
		return;

	cmd->tickCount = TIME2TICKS(record->simTime + getLerp());
}

void backtrack_addLatency(NetworkChannel *networkChannel)
{
	if (!maxUnlag)
		return;

	if (!config.backtrack.enabled)
		return;

	if (config.backtrack.timeLimit <= 200)
		return;

	float serverTime = sdk_getServerTime(0);
	float outLatency = networkChannel->vmt->getLatency(networkChannel, 0);
	float latency    = MAX(0, CLAMP((float)config.backtrack.timeLimit / 1000.0f - 0.2f, 0, maxUnlag->vmt->getFloat(maxUnlag)) - outLatency);

	for (int i = cvector_size(inSequences) - 1; i >= 0; i--)
		if (serverTime - inSequences[i].serverTime >= latency) {
			networkChannel->inReliableState  = inSequences[i].reliableState;
			networkChannel->inSequenceNumber = inSequences[i].sequenceNumber;
			break;
		}
}

void backtrack_updateSequences(NetworkChannel *networkChannel)
{
	static int lastInSequenceNumber = 0;

	if (networkChannel->inSequenceNumber != lastInSequenceNumber) {
		lastInSequenceNumber = networkChannel->inSequenceNumber;

		IncomingSequence inSequence = {
			.reliableState  = networkChannel->inReliableState,
			.sequenceNumber = networkChannel->inSequenceNumber,
			.serverTime     = sdk_getServerTime(0)
		};

		cvector_push_back(inSequences, inSequence);
	}

	while (cvector_size(inSequences) > 2048)
		cvector_erase(inSequences, 0);
}

void backtrack_init(void)
{
	updateRate     = interfaces.cvar->vmt->findVar(interfaces.cvar, "cl_updaterate");
	maxUpdateRate  = interfaces.cvar->vmt->findVar(interfaces.cvar, "sv_maxupdaterate");
	interp         = interfaces.cvar->vmt->findVar(interfaces.cvar, "cl_interp");
	interpRatio    = interfaces.cvar->vmt->findVar(interfaces.cvar, "cl_interp_ratio");
	minInterpRatio = interfaces.cvar->vmt->findVar(interfaces.cvar, "sv_client_min_interp_ratio");
	maxInterpRatio = interfaces.cvar->vmt->findVar(interfaces.cvar, "sv_client_max_interp_ratio");
	maxUnlag       = interfaces.cvar->vmt->findVar(interfaces.cvar, "sv_maxunlag");
}

void backtrack_cleanUp(void)
{
	for (int i = 0; i < 65; i++)
		cvector_free(records[i]);

	cvector_free(inSequences);
}
