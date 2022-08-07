#include "../config.h"
#include "../interfaces.h"
#include "../memory.h"
#include "../netvars.h"

#include "glow.h"

void glow_render(void)
{
	if (!interfaces.engine->vmt->isInGame(interfaces.engine))
		return;

	Entity *localPlayer = *memory.localPlayer;
	if (!localPlayer)
		return;

	for (int i = 0; i < memory.glowObjectManager->objects.size; i++) {
		GlowObject *glowObject = (GlowObject *)memory.glowObjectManager->objects.memory + i;

		Entity *entity = glowObject->entity;
		if (!entity || glowObject->nextFreeSlot != -2)
			continue;

		Networkable *networkable = (Networkable *)(entity + 2);
		ClassID classID = networkable->vmt->getClientClass(networkable)->classID;

		switch (classID) {
		case ClassID_CSPlayer:
			if (entity == localPlayer)
				break;

			bool immunity = *(bool *)((char *)entity + netvars_getOffset("CCSPlayer->m_bGunGameImmunity"));
			if (immunity)
				break;

			if (!entity->vmt->isAlive(entity))
				break;

			if (networkable->vmt->isDormant(networkable))
				break;

			GlowCategory glowCategory = memory.isOtherEnemy(localPlayer, entity) ? GlowCategory_Enemies : GlowCategory_Teammates;
			GlowConfig *glowConfig = &config.glow[glowCategory];

			if (!glowConfig->enabled)
				break;


			if (glowConfig->healthBased) {
				glowObject->colorA   = ColorA_fromHealth(entity->vmt->getHealth(entity));
				glowObject->colorA.a = glowConfig->colorA.a;
			} else {
				glowObject->colorA = glowConfig->colorA;
			}

			glowObject->alphaMax = 1;
			glowObject->occluded = true;
			glowObject->style = 0;

			break;
		default:
			break;
		}
	}
}
