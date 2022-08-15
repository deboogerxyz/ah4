#include "../config.h"
#include "../interfaces.h"
#include "../memory.h"
#include "../sdk.h"

static LegitbotCategory getCategory(WeaponID weaponID)
{
	switch (weaponID) {
	case WeaponID_Glock: // FALLTHROUGH
	case WeaponID_Hkp2000:
	case WeaponID_Usp_s:
	case WeaponID_Elite:
	case WeaponID_P250:
	case WeaponID_Tec9:
	case WeaponID_Fiveseven:
	case WeaponID_Cz75a:
	case WeaponID_Deagle:
	case WeaponID_Revolver:
		return LegitbotCategory_Pistols;
	case WeaponID_GalilAr: // FALLTHROUGH
	case WeaponID_Famas:
	case WeaponID_Ak47:
	case WeaponID_M4A1:
	case WeaponID_M4a1_s:
	case WeaponID_Sg553:
	case WeaponID_Aug:
	case WeaponID_G3SG1:
	case WeaponID_Scar20:
	case WeaponID_M249:
	case WeaponID_Negev:
		return LegitbotCategory_Rifles;
	case WeaponID_Awp:
		return LegitbotCategory_AWP;
	case WeaponID_Ssg08:
		return LegitbotCategory_Scout;
	case WeaponID_Mac10: // FALLTHROUGH
	case WeaponID_Mp9:
	case WeaponID_Mp7:
	case WeaponID_Mp5sd:
	case WeaponID_Ump45:
	case WeaponID_P90:
	case WeaponID_Bizon:
		return LegitbotCategory_SMGs;
	case WeaponID_Nova: // FALLTHROUGH
	case WeaponID_Xm1014:
	case WeaponID_Sawedoff:
	case WeaponID_Mag7:
		return LegitbotCategory_Shotguns;
	default:
		return -1;
	}
}

void legitbot_run(UserCmd *cmd)
{
	if (!(cmd->buttons & IN_ATTACK))
		return;

	Entity *localPlayer = *memory.localPlayer;
	if (!localPlayer)
		return;

	if (!localPlayer->vmt->isAlive(localPlayer))
		return;

	if (*Entity_waitForNoAttack(localPlayer))
		return;

	if (*Entity_nextAttack(localPlayer) > sdk_getServerTime(0))
		return;

	Entity *activeWeapon = localPlayer->vmt->getActiveWeapon(localPlayer);
	if (!activeWeapon)
		return;

	WeaponID weaponID = *Entity_itemIndex(activeWeapon);

	LegitbotCategory legitbotCategory = getCategory(weaponID);
	if (legitbotCategory < 0)
		return;

	LegitbotConfig *legitbotConfig = &config.legitbot[legitbotCategory];

	if (!legitbotConfig->enabled)
		return;

	if (legitbotConfig->flashCheck && *Entity_flashDuration(localPlayer) > 75)
		return;

	if (legitbotConfig->scopeCheck && !*Entity_isScoped(localPlayer) && activeWeapon->vmt->getWeaponType(activeWeapon) == WeaponType_SniperRifle)
		return;

	WeaponInfo *weaponInfo = activeWeapon->vmt->getWeaponInfo(activeWeapon);
	if (!weaponInfo)
		return;

	if (*Entity_shotsFired(localPlayer) > 0 && !weaponInfo->fullAuto)
		return;

	Vector eyePos     = localPlayer->vmt->getEyePosition(localPlayer);
	Vector aimPunch   = localPlayer->vmt->getAimPunch(localPlayer);
	Vector viewAngles = Vector_add(cmd->viewAngles, aimPunch);

	float bestFov = legitbotConfig->fov;
	Vector bestAngle = {0};

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

		for (int j = 0; j < 8; j++) {
			if (!legitbotConfig->bones[j])
				continue;

			Vector bonePos = Entity_getBonePosition(entity, 8 - j);
			Vector angle   = Vector_calculateAngle(eyePos, bonePos, viewAngles);

			float fov = hypotf(angle.x, angle.y);

			if (!(fov < bestFov))
				continue;

			if (legitbotConfig->smokeCheck && memory.lineGoesThroughSmoke(eyePos, bonePos, 1))
				continue;

			if (legitbotConfig->visibleCheck && !Entity_canSee(localPlayer, entity, bonePos))
				continue;

			bestFov   = fov;
			bestAngle = angle;
		}
	}

	if (Vector_isNull(bestAngle))
		return;

	if (!legitbotConfig->silent && legitbotConfig->smooth > 1)
		bestAngle = Vector_div(bestAngle, legitbotConfig->smooth);

	cmd->viewAngles = Vector_add(cmd->viewAngles, bestAngle);

	if (!legitbotConfig->silent)
		interfaces.engine->vmt->setViewAngles(interfaces.engine, &cmd->viewAngles);
}
