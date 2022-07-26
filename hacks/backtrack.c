#include <math.h>

#include "../libs/cvector.h"

#include "../interfaces.h"
#include "../memory.h"
#include "../netvars.h"
#include "../utils.h"

#include "backtrack.h"

#define TIME2TICKS(x) ((int)(0.5f + (x) / memory.globalVars->intervalPerTick))

typedef struct {
	Entity *entity;
	Vector headPos;
	float simTime;
} Record;

static ConVar *updateRate, *maxUpdateRate, *interp, *interpRatio, *minInterpRatio, *maxInterpRatio, *maxUnlag;
static Record *records[65] = {0};

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

		bool immunity = *(bool *)((char *)entity + netvars_getOffset("CCSPlayer->m_bGunGameImmunity"));
		if (immunity)
			continue;

		if (!entity->vmt->isAlive(entity))
			continue;

		Networkable *networkable = (Networkable *)(entity + 2);
		if (networkable->vmt->isDormant(networkable))
			continue;

		float simTime = *(float *)((char *)entity + netvars_getOffset("CBaseEntity->m_flSimulationTime"));

		if (!cvector_empty(records[i]) && cvector_end(records[i])->simTime == simTime)
			continue;

		Record record = {
			.entity  = entity,
			.headPos = Entity_getBonePosition(entity, 8),
			.simTime = simTime
		};

		cvector_push_back(records[i], record);

		while (cvector_size(records[i]) > TIME2TICKS(0.2f))
			cvector_erase(records[i], 0);

		for (int j = 0; j < cvector_size(records[i]); j++)
			if (!isValid(records[i][j].simTime))
				cvector_erase(records[i], j);
	}
}

void backtrack_run(UserCmd *cmd)
{
	if (!(cmd->buttons & IN_ATTACK))
		return;

	Entity *localPlayer = *memory.localPlayer;
	if (!localPlayer)
		return;

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

		bool immunity = *(bool *)((char *)entity + netvars_getOffset("CCSPlayer->m_bGunGameImmunity"));
		if (immunity)
			continue;

		if (!entity->vmt->isAlive(entity))
			continue;

		Networkable *networkable = (Networkable *)(entity + 2);
		if (networkable->vmt->isDormant(networkable))
			continue;

		if (cvector_empty(records[i]))
			continue;

		for (int j = 0; j < cvector_size(records[i]); j++) {
			Record *record = &records[i][j];
			if (!record)
				continue;

			if (!isValid(record->simTime))
				continue;

			Vector headPos = record->headPos;
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
		return;

	Record *record = &records[bestEntityIndex][bestRecordIndex];
	if (!record)
		return;

	cmd->tickCount = TIME2TICKS(record->simTime + getLerp());
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
}
