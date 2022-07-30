#include <string.h>

#include "../config.h"
#include "../interfaces.h"
#include "../memory.h"
#include "../netvars.h"
#include "../sdk.h"

#include "skinChanger.h"

const char *skinChanger_weapons[] = {
	"Knife",
	"Glove",
	"AK-47",
	"AUG",
	"AWP",
	"CZ75 Auto",
	"Desert Eagle",
	"Dual Berettas",
	"FAMAS",
	"Five-SeveN",
	"G3SG1",
	"Galil AR",
	"Glock-18",
	"M249",
	"M4A1-S",
	"M4A4",
	"MAC-10",
	"MAG-7",
	"MP5-SD",
	"MP7",
	"MP9",
	"Negev",
	"Nova",
	"P2000",
	"P250",
	"P90",
	"PP-Bizon",
	"R8 Revolver",
	"Sawed-Off",
	"SCAR-20",
	"SSG 08",
	"SG 553",
	"Tec-9",
	"UMP-45",
	"USP-S",
	"XM1014"
};

void skinChanger_run(void)
{
	Entity *localPlayer = *memory.localPlayer;
	if (!localPlayer)
		return;

	Entity *activeWeapon = localPlayer->vmt->getActiveWeapon(localPlayer);
	if (!activeWeapon)
		return;

	int *itemIndex  = (int *)((char *)activeWeapon + netvars_getOffset("CBaseAttributableItem->m_iItemDefinitionIndex"));
	int *paintKit   = (int *)((char *)activeWeapon + netvars_getOffset("CBaseAttributableItem->m_nFallbackPaintKit"));
	int *seed       = (int *)((char *)activeWeapon + netvars_getOffset("CBaseAttributableItem->m_nFallbackSeed"));
	int *statTrak   = (int *)((char *)activeWeapon + netvars_getOffset("CBaseAttributableItem->m_nFallbackStatTrak"));
	int *quality    = (int *)((char *)activeWeapon + netvars_getOffset("CBaseAttributableItem->m_iEntityQuality"));
	float *wear     = (float *)((char *)activeWeapon + netvars_getOffset("CBaseAttributableItem->m_flFallbackWear"));
	char *name      = (char *)((char *)activeWeapon + netvars_getOffset("CBaseAttributableItem->m_szCustomName"));
	int *itemIDHigh = (int *)((char *)activeWeapon + netvars_getOffset("CBaseAttributableItem->m_iItemIDHigh"));
	int *accountID  = (int *)((char *)activeWeapon + netvars_getOffset("CBaseAttributableItem->m_iAccountID"));

	Networkable *networkable = (Networkable *)(localPlayer + 2);
	int i = networkable->vmt->getIndex(networkable);

	PlayerInfo playerInfo;
	interfaces.engine->vmt->getPlayerInfo(interfaces.engine, i, &playerInfo);

	*paintKit = 37;
	*statTrak = 1337;
	*quality = 3;
	*wear = 0;
	*accountID = playerInfo.xuid;
	strncpy(name, "ah4 on top", 32);

	*itemIDHigh = -1;
}

void skinChanger_forceUpdate(void)
{
//	interfaces.cvar->vmt->findVar(interfaces.cvar, "cl_fullupdate")->changeCallback();
}
