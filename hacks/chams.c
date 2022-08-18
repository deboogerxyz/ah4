#include "../config.h"
#include "../interfaces.h"
#include "../memory.h"

#include "chams.h"

static Material *normal, *flat, *pearl, *metal, *glow;

static Material *getMaterial(int i)
{
	switch (i) {
	default:
	case 0: return normal;
	case 1: return flat;
	case 2: return pearl;
	case 3: return metal;
	case 4: return glow;
	}
}

static void initMaterials(void)
{
	normal = interfaces.materialSystem->vmt->createMaterial(interfaces.materialSystem, "normal", memory.keyValuesFromString("VertexLitGeneric", 0, 0));
	flat   = interfaces.materialSystem->vmt->createMaterial(interfaces.materialSystem, "flat", memory.keyValuesFromString("UnlitGeneric", 0, 0));
	pearl  = interfaces.materialSystem->vmt->createMaterial(interfaces.materialSystem, "pearl", memory.keyValuesFromString("VertexLitGeneric", "$ambientonly 1 $phong 1 $pearlescent 3 $basemapalphaphongmask 1", 0));
	metal  = interfaces.materialSystem->vmt->createMaterial(interfaces.materialSystem, "metal", memory.keyValuesFromString("VertexLitGeneric", "$basetexture white $ignorez 0 $envmap env_cubemap $normalmapalphaenvmapmask 1 $envmapcontrast 1 $nofog 1 $model 1 $nocull 0 $selfillum 1 $halfambert 1 $znearer 0 $flat 1", 0));
	glow  = interfaces.materialSystem->vmt->createMaterial(interfaces.materialSystem, "glow", memory.keyValuesFromString("VertexLitGeneric", "$additive 1 $envmap models/effects/cube_white $envmapfresnel 1 $alpha 0.8", 0));
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

	ChamsCategory chamsCategory = memory.isOtherEnemy(localPlayer, entity) ? ChamsCategory_Enemies : ChamsCategory_Teammates;

	ChamsConfig *occludedConfig = &config.chams[chamsCategory][ChamsSubCategory_Occluded];
	ChamsConfig *visibleConfig = &config.chams[chamsCategory][ChamsSubCategory_Visible];

	if (!occludedConfig->enabled && !visibleConfig->enabled)
		return false;

	if (occludedConfig->enabled) {
		if (!visibleConfig->enabled)
			oldModelRenderVMT->drawModelExecute(interfaces.modelRender, ctx, state, info, customBoneToWorld);

		Material *material = getMaterial(occludedConfig->material);

		material->vmt->setMaterialVarFlag(material, IGNOREZ, true);

		ColorA colorA;
		if (occludedConfig->healthBased) {
			colorA = ColorA_fromHealth(*Entity_health(entity));
			colorA.a = occludedConfig->colorA.a;
		} else {
			colorA = occludedConfig->colorA;
		}

		if (material == glow) {
			MaterialVar *materialVar = material->vmt->findVar(material, "$envmaptint", 0, true);
			materialVar->vmt->setVectorValue(materialVar, colorA.r, colorA.g, colorA.b);
		} else {
			material->vmt->colorModulate(material, colorA.r, colorA.g, colorA.b);
		}
		material->vmt->alphaModulate(material, colorA.a);

		interfaces.studioRender->vmt->forcedMaterialOverride(interfaces.studioRender, material, OverrideType_Normal, -1);
		oldModelRenderVMT->drawModelExecute(interfaces.modelRender, ctx, state, info, customBoneToWorld);
		interfaces.studioRender->vmt->forcedMaterialOverride(interfaces.studioRender, 0, OverrideType_Normal, -1);
	}

	if (visibleConfig->enabled) {
		Material *material = getMaterial(visibleConfig->material);

		material->vmt->setMaterialVarFlag(material, IGNOREZ, false);

		ColorA colorA;
		if (visibleConfig->healthBased) {
			colorA = ColorA_fromHealth(*Entity_health(entity));
			colorA.a = visibleConfig->colorA.a;
		} else {
			colorA = visibleConfig->colorA;
		}

		if (material == glow) {
			MaterialVar *materialVar = material->vmt->findVar(material, "$envmaptint", 0, true);
			materialVar->vmt->setVectorValue(materialVar, colorA.r, colorA.g, colorA.b);
		} else {
			material->vmt->colorModulate(material, colorA.r, colorA.g, colorA.b);
		}
		material->vmt->alphaModulate(material, colorA.a);

		interfaces.studioRender->vmt->forcedMaterialOverride(interfaces.studioRender, material, OverrideType_Normal, -1);
		oldModelRenderVMT->drawModelExecute(interfaces.modelRender, ctx, state, info, customBoneToWorld);
	}

	return true;
}
