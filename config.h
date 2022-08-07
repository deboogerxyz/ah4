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

typedef struct {
	bool enabled;
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

typedef struct {
	bool jumpBug;
	KeyBind jumpBugKeyBind;
	bool edgeJump;
	KeyBind edgeJumpKeyBind;
} MiscConfig;

typedef struct {
	BacktrackConfig backtrack;
	GlowConfig glow[GlowCategory_Len];
	MiscConfig misc;
} Config;

extern Config config;

int config_getConfigs(char ***configs);
void config_reset(void);
void config_load(const char *name);
void config_save(const char *name);
void config_openDirectory(void);

#endif // CONFIG_H_
