#include "../libs/cvector.h"

#include "../config.h"
#include "../interfaces.h"
#include "../memory.h"
#include "../netvars.h"

#include "glow.h"

typedef struct {
	Entity *entity;
	int index;
} CustomGlowObject;

static CustomGlowObject *customGlowObjects = 0;

void glow_render(void)
{
	if (!interfaces.engine->vmt->isInGame(interfaces.engine))
		return;

	glow_clearCustomGlowObjects();

	int highestEntityIndex = interfaces.entityList->vmt->getHighestEntityIndex(interfaces.entityList);
	int maxClients         = interfaces.engine->vmt->getMaxClients(interfaces.engine);
	for (int i = maxClients + 1; i <= highestEntityIndex; i++) {
		Entity *entity = interfaces.entityList->vmt->getEntity(interfaces.entityList, i);
		if (!entity)
			continue;

		Networkable *networkable = (Networkable *)(entity + 2);

		if (networkable->vmt->isDormant(networkable))
			continue;

		ClassID classID = networkable->vmt->getClientClass(networkable)->classID;

		switch (classID) {
		case ClassID_BaseCSGrenadeProjectile:
		case ClassID_BreachChargeProjectile:
		case ClassID_BumpMineProjectile:
		case ClassID_DecoyProjectile:
		case ClassID_MolotovProjectile:
		case ClassID_SensorGrenadeProjectile:
		case ClassID_SmokeGrenadeProjectile:
		case ClassID_SnowballProjectile:
			if (!GlowObjectManager_hasGlow(entity)) {
				int j = GlowObjectManager_register(entity);
				if (!(j < 0)) {
					CustomGlowObject customGlowObject = {
						.entity = entity, 
						.index = j
					};

					cvector_push_back(customGlowObjects, customGlowObject);
				}
			}

			break;
		default: break;
		}
	}

	Entity *localPlayer = *memory.localPlayer;
	if (!localPlayer)
		return;

	for (int i = 0; i < memory.glowObjectManager->objects.size; i++) {
		GlowObject *glowObject = (GlowObject *)memory.glowObjectManager->objects.memory + i;

		Entity *entity = glowObject->entity;
		if (!entity || glowObject->nextFreeSlot != -2)
			continue;

		Networkable *networkable = (Networkable *)(entity + 2);

		if (networkable->vmt->isDormant(networkable))
			continue;

		ClassID classID = networkable->vmt->getClientClass(networkable)->classID;

		switch (classID) {
		case ClassID_CSPlayer:
			if (entity == localPlayer)
				break;

			if (*Entity_immunity(entity))
				break;

			if (!entity->vmt->isAlive(entity))
				break;

			GlowCategory glowCategory = memory.isOtherEnemy(localPlayer, entity) ? GlowCategory_Enemies : GlowCategory_Teammates;
			GlowConfig *glowConfig = &config.glow[glowCategory];
			if (!glowConfig)
				break;

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
		case ClassID_C4:
			glowConfig = &config.glow[GlowCategory_DroppedC4];
			if (!glowConfig)
				break;

			if (!glowConfig->enabled)
				break;

			glowObject->colorA = glowConfig->colorA;
			glowObject->alphaMax = 1;
			glowObject->occluded = true;
			glowObject->style = 0;

			break;
		case ClassID_PlantedC4:
			glowConfig = &config.glow[GlowCategory_PlantedC4];
			if (!glowConfig)
				break;

			if (!glowConfig->enabled)
				break;

			glowObject->colorA = glowConfig->colorA;
			glowObject->alphaMax = 1;
			glowObject->occluded = true;
			glowObject->style = 0;

			break;
		case ClassID_BaseCSGrenadeProjectile: // FALLTHROUGH
		case ClassID_BreachChargeProjectile:
		case ClassID_BumpMineProjectile:
		case ClassID_DecoyProjectile:
		case ClassID_MolotovProjectile:
		case ClassID_SensorGrenadeProjectile:
		case ClassID_SmokeGrenadeProjectile:
		case ClassID_SnowballProjectile:
			glowConfig = &config.glow[GlowCategory_Projectiles];
			if (!glowConfig)
				break;

			if (!glowConfig->enabled)
				break;

			glowObject->colorA = glowConfig->colorA;
			glowObject->alphaMax = 1;
			glowObject->occluded = true;
			glowObject->style = 0;

			break;
		default:
			glowConfig = &config.glow[GlowCategory_DroppedWeapons];
			if (!glowConfig)
				break;

			if (!glowConfig->enabled)
				break;

			if (!entity->vmt->isWeapon(entity))
				break;

			glowObject->colorA = glowConfig->colorA;
			glowObject->alphaMax = 1;
			glowObject->occluded = true;
			glowObject->style = 0;

			break;
		}
	}
}

void glow_clearCustomGlowObjects(void)
{
	for (int i = 0; i < cvector_size(customGlowObjects); i++) {
		CustomGlowObject *customGlowObject = &customGlowObjects[i];

		GlowObjectManager_unregister(customGlowObject->entity, customGlowObject->index);
	}

	cvector_free(customGlowObjects);
}
