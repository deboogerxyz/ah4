#include "../config.h"
#include "../interfaces.h"
#include "../memory.h"

#include "chams.h"

static Material *normal;

static void initMaterials(void)
{
	normal = interfaces.materialSystem->vmt->findMaterial(interfaces.materialSystem, "debug/debugambientcube", 0, true, 0);
}

bool chams_render(void *ctx, void *state, ModelRenderInfo *info, Matrix3x4 *customBoneToWorld, ModelRenderVMT *oldModelRenderVMT)
{
	static bool once = false;
	if (!once) {
		initMaterials();
		once = true;
	}

	if (!info->renderable)
		return false;

	Entity *localPlayer = *memory.localPlayer;
	if (!localPlayer)
		return false;

	Entity *entity = interfaces.entityList->vmt->getEntity(interfaces.entityList, info->entityIndex);
	if (!entity || entity == localPlayer || !entity->vmt->isPlayer(entity) || !entity->vmt->isAlive(entity))
		return false;

	Networkable *networkable = (Networkable *)(entity + 2);
	if (networkable->vmt->isDormant(networkable))
		return false;

	Material *material = normal;

	ChamsCategory chamsCategory = memory.isOtherEnemy(localPlayer, entity) ? ChamsCategory_Enemies : ChamsCategory_Teammates;

	ChamsConfig *occludedConfig = &config.chams[chamsCategory][ChamsSubCategory_Occluded];
	ChamsConfig *visibleConfig = &config.chams[chamsCategory][ChamsSubCategory_Visible];

	if (!occludedConfig->enabled && !visibleConfig->enabled)
		return false;

	if (occludedConfig->enabled) {
		if (!visibleConfig->enabled)
			oldModelRenderVMT->drawModelExecute(interfaces.modelRender, ctx, state, info, customBoneToWorld);

		material->vmt->setMaterialVarFlag(material, IGNOREZ, true);

		ColorA colorA;
		if (occludedConfig->healthBased) {
			colorA = ColorA_fromHealth(*Entity_health(entity));
			colorA.a = occludedConfig->colorA.a;
		} else {
			colorA = occludedConfig->colorA;
		}

		material->vmt->colorModulate(material, colorA.r, colorA.g, colorA.b);
		material->vmt->alphaModulate(material, colorA.a);

		interfaces.studioRender->vmt->forcedMaterialOverride(interfaces.studioRender, material, OverrideType_Normal, -1);
		oldModelRenderVMT->drawModelExecute(interfaces.modelRender, ctx, state, info, customBoneToWorld);
		interfaces.studioRender->vmt->forcedMaterialOverride(interfaces.studioRender, 0, OverrideType_Normal, -1);
	}

	if (visibleConfig->enabled) {
		material->vmt->setMaterialVarFlag(material, IGNOREZ, false);

		ColorA colorA;
		if (visibleConfig->healthBased) {
			colorA = ColorA_fromHealth(*Entity_health(entity));
			colorA.a = visibleConfig->colorA.a;
		} else {
			colorA = visibleConfig->colorA;
		}

		material->vmt->colorModulate(material, colorA.r, colorA.g, colorA.b);
		material->vmt->alphaModulate(material, colorA.a);

		interfaces.studioRender->vmt->forcedMaterialOverride(interfaces.studioRender, material, OverrideType_Normal, -1);
		oldModelRenderVMT->drawModelExecute(interfaces.modelRender, ctx, state, info, customBoneToWorld);
	}

	return true;
}
