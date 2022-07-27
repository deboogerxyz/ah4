#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdbool.h>

typedef struct {
	int key;
} KeyBind;

typedef struct {
	bool enabled;
	int timeLimit;
} BacktrackConfig;

typedef struct {
	bool jumpBug;
	KeyBind jumpBugKeyBind;
	bool edgeJump;
	KeyBind edgeJumpKeyBind;
} MiscConfig;

typedef struct {
	BacktrackConfig backtrack;
	MiscConfig misc;
} Config;

extern Config config;

int config_getConfigs(char ***configs);
void config_load(const char *name);
void config_save(const char *name);

#endif // CONFIG_H_
