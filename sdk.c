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
			tick = *(int *)((char *)localPlayer + netvars_getOffset("CBasePlayer->m_nTickBase"));
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
