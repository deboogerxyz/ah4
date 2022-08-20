#include "interfaces.h"
#include "memory.h"
#include "netvars.h"
#include "utils.h"

#include "sdk.h"

float sdk_getServerTime(UserCmd *cmd)
{
	static int tick;
	static UserCmd *lastCmd;

	if (cmd) {
		Entity *localPlayer = *memory.localPlayer;
		if (localPlayer && (!lastCmd || lastCmd->hasBeenPredicted))
			tick = *Entity_tickBase(localPlayer);
		else
			tick++;

		lastCmd = cmd;
	}

	return tick * memory.globalVars->intervalPerTick;
}

Vector Vector_add(Vector a, Vector b)
{
	Vector c = {
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z
	};

	return c;
}

Vector Vector_sub(Vector a, Vector b)
{
	Vector c = {
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z
	};

	return c;
}

Vector Vector_div(Vector v, float f)
{
	Vector a = {
		.x = v.x / f,
		.y = v.y / f,
		.z = v.z / f
	};

	return a;
}

Vector Vector_toAngle(Vector v)
{
	Vector a = {
		.x = RAD2DEG(atan2(-v.z, hypot(v.x, v.y))),
		.y = RAD2DEG(atan2(v.y, v.x)),
		.z = 0.0f
	};

	return a;
}

Vector Vector_normalize(Vector v)
{
	Vector a = {
		.x = isfinite(v.x) ? remainder(v.x, 360.0f) : 0.0f,
		.y = isfinite(v.y) ? remainder(v.y, 360.0f) : 0.0f,
		.z = 0.0f
	};

	return a;
}

Vector Vector_calculateAngle(Vector start, Vector end, Vector angle)
{
	Vector a = Vector_sub(end, start);
	Vector b = Vector_sub(Vector_toAngle(a), angle);

	return Vector_normalize(b);
}

bool Vector_isNull(Vector v)
{
	return !v.x && !v.y && !v.z;
}

Vector Matrix3x4_origin(Matrix3x4 m)
{
	Vector v = {
		.x = m[0][3],
		.y = m[1][3],
		.z = m[2][3]
	};

	return v;
}

Vector Entity_getBonePosition(Entity *entity, int bone)
{
	Matrix3x4 m[256] = {0};

	Renderable *renderable = (Renderable *)(entity + 1);
	renderable->vmt->setupBones(renderable, m, 256, 256, 0);

	return Matrix3x4_origin(m[bone]);
}

bool Entity_canSee(Entity *entity, Entity *other, Vector pos)
{
	Vector eyePos = entity->vmt->getEyePosition(entity);

	Ray ray = {
		.start = eyePos,
		.delta = Vector_sub(pos, eyePos),
		.isRay = true,
		.isSwept = !Vector_isNull(ray.delta)
	};

	TraceFilter filter = {
		.skip = entity
	};
	TraceFilter_init(&filter);

	Trace trace;
	interfaces.engineTrace->vmt->traceRay(interfaces.engineTrace, &ray, 0x46004009, &filter, &trace);

	return trace.entity == other || trace.fraction > 0.97f;
}

bool GlowObjectManager_hasGlow(Entity *entity)
{
	for (int i = 0; i < memory.glowObjectManager->objects.size; i++) {
		GlowObject *glowObject = (GlowObject *)memory.glowObjectManager->objects.memory + i;

		if (glowObject->nextFreeSlot == -2 && glowObject->entity == entity)
			return true;
	}

	return false;
}

int GlowObjectManager_register(Entity *entity)
{
	int i = memory.glowObjectManager->firstFreeSlot;
	if (i == -1)
		return i;

	GlowObject *glowObject = (GlowObject *)memory.glowObjectManager->objects.memory + i;

	memory.glowObjectManager->firstFreeSlot = glowObject->nextFreeSlot;

	glowObject->entity = entity;
	glowObject->fullBloom = false;
	glowObject->fullBloomStencil = 0;
	glowObject->splitScreenSlot = -1;
	glowObject->nextFreeSlot = -2;

	return i;

}

void GlowObjectManager_unregister(Entity *entity, int i)
{
	GlowObject *glowObject = (GlowObject *)memory.glowObjectManager->objects.memory + i;

	glowObject->nextFreeSlot = memory.glowObjectManager->firstFreeSlot;
	glowObject->entity = entity;
	glowObject->occluded = false;
	glowObject->unoccluded = false;

	memory.glowObjectManager->firstFreeSlot = i;
}

static bool shouldHitEntity(TraceFilter *this, Entity *entity, int unused)
{
	return entity != this->skip;
}

static int getTraceType(TraceFilter *this)
{
	return 0;
}

void TraceFilter_init(TraceFilter *filter)
{
	static TraceFilterVMT vmt = {
		.shouldHitEntity = shouldHitEntity,
		.getTraceType    = getTraceType
	};

	filter->vmt = &vmt;
}

Color Color_fromHealth(int health)
{
	health = MIN(health, 100);

	Color color = {
		.r = MIN(2.0f * (100 - health) / 100.0f, 1),
		.g = MIN(2.0f * health / 100.0f, 1),
		.b = 0
	};

	return color;
}

ColorA ColorA_fromHealth(int health)
{
	health = MIN(health, 100);

	ColorA colorA = {
		.r = MIN(2.0f * (100 - health) / 100.0f, 1),
		.g = MIN(2.0f * health / 100.0f, 1),
		.b = 0,
		.a = 1
	};

	return colorA;
}

bool StudioRender_isForcedMaterialOverride(void)
{
	Material *materialOverride = interfaces.studioRender->materialOverride;
	OverrideType overrideType = interfaces.studioRender->overrideType;

	if (!materialOverride)
		return overrideType == OverrideType_DepthWrite || overrideType == OverrideType_SsaoDepthWrite;

	return !strncmp("dev/glow", materialOverride->vmt->getName(materialOverride), strlen("dev/glow"));
}

NETVARS_IMPL(moveType, "CBaseEntity", "m_nRenderMode", 1, MoveType)
NETVARS_IMPL(simTime, "CBaseEntity", "m_flSimulationTime", 0, float)
NETVARS_IMPL(viewModel, "CBasePlayer", "m_hViewModel[0]", 0, int)
NETVARS_IMPL(health, "CBasePlayer", "m_iHealth", 0, int)
NETVARS_IMPL(flags, "CBasePlayer", "m_fFlags", 0, int)
NETVARS_IMPL(spottedByMask, "CBaseEntity", "m_bSpottedByMask", 0, long)
NETVARS_IMPL(flashDuration, "CCSPlayer", "m_flFlashMaxAlpha", -8, float)
NETVARS_IMPL(tickBase, "CBasePlayer", "m_nTickBase", 0, int)
NETVARS_IMPL(velocity, "CBasePlayer", "m_vecVelocity[0]", 0, Vector)
NETVARS_IMPL(isScoped, "CCSPlayer", "m_bIsScoped", 0, char)
NETVARS_IMPL(immunity, "CCSPlayer", "m_bGunGameImmunity", 0, char)
NETVARS_IMPL(shotsFired, "CCSPlayer", "m_iShotsFired", 0, int)
NETVARS_IMPL(waitForNoAttack, "CCSPlayer", "m_bWaitForNoAttack", 0, char)
NETVARS_IMPL(nextAttack, "CBaseCombatCharacter", "m_flNextAttack", 0, float)
NETVARS_IMPL(itemIndex, "CBaseAttributableItem", "m_iItemDefinitionIndex", 0, short)
NETVARS_IMPL(aimPunchAngle, "CBasePlayer", "m_aimPunchAngle", 0, Vector)
NETVARS_IMPL(viewPunchAngle, "CBasePlayer", "m_viewPunchAngle", 0, Vector)
