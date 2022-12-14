#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdbool.h>

#include "sdk.h"

typedef struct {
	int key;
	int mode;
	bool active;   // Do not save
	bool lastDown; // Do not save
} KeyBind;

typedef enum {
	LegitbotCategory_Pistols,
	LegitbotCategory_Rifles,
	LegitbotCategory_AWP,
	LegitbotCategory_Scout,
	LegitbotCategory_SMGs,
	LegitbotCategory_Shotguns,
	LegitbotCategory_Len,
} LegitbotCategory;

typedef struct {
	int enabled;
	int silent;
	int minShotsFired;
	int maxShotsFired;
	int visibleCheck;
	int smokeCheck;
	int flashCheck;
	int scopeCheck;
	float fov;
	float smooth;
	int bones[6];
} LegitbotConfig;

typedef struct {
	int enabled;
	int timeLimit;
} BacktrackConfig;

typedef enum {
	GlowCategory_Enemies,
	GlowCategory_Teammates,
	GlowCategory_DroppedC4,
	GlowCategory_PlantedC4,
	GlowCategory_Projectiles,
	GlowCategory_DroppedWeapons,
	GlowCategory_Len,
} GlowCategory;

typedef struct {
	int enabled;
	int healthBased;
	ColorA colorA;
} GlowConfig;

typedef enum {
	ChamsCategory_Enemies,
	ChamsCategory_Teammates,
	ChamsCategory_Len
} ChamsCategory;

typedef enum {
	ChamsSubCategory_Visible,
	ChamsSubCategory_Occluded,
	ChamsSubCategory_Len
} ChamsSubCategory;

typedef struct {
	int enabled;
	int material;
	int healthBased;
	ColorA colorA;
} ChamsConfig;

typedef struct {
	int disablePostProcessing;
	int disableShadows;
	int forceCrosshair;
	int grenadePrediction;
	int revealRadar;
	int revealRanks;
	int revealOverwatch;
} VisualsConfig;

typedef struct {
	int antiAfkKick;
	int fastDuck;
	int slideWalk;
	int bunnyHop;
	int jumpBug;
	KeyBind jumpBugKeyBind;
	int edgeJump;
	KeyBind edgeJumpKeyBind;
	int fixMovement;
} MiscConfig;

typedef struct {
	LegitbotConfig legitbot[LegitbotCategory_Len];
	BacktrackConfig backtrack;
	GlowConfig glow[GlowCategory_Len];
	ChamsConfig chams[ChamsCategory_Len][ChamsSubCategory_Len];
	VisualsConfig visuals;
	MiscConfig misc;
} Config;

extern Config config;

int config_getConfigs(char ***configs);
void config_reset(void);
void config_load(const char *name);
void config_save(const char *name);
void config_openDirectory(void);

#endif // CONFIG_H_
