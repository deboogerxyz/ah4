#include <cjson/cJSON.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config.h"

#define READ_BOOL(json, name, var) \
	{ cJSON *item = cJSON_GetObjectItem(json, name); \
	if (cJSON_IsBool(item)) \
		var = item->valueint; }

#define READ_INT(json, name, var) \
	{ cJSON *item = cJSON_GetObjectItem(json, name); \
	if (cJSON_IsNumber(item)) \
		var = item->valueint; }

#define READ_FLOAT(json, name, var) \
	{ cJSON *item = cJSON_GetObjectItem(json, name); \
	if (cJSON_IsNumber(item)) \
		var = (float)item->valuedouble; }

#define READ_KEYBIND(json, name, var) \
	{ cJSON *keyBindJson = cJSON_GetObjectItem(json, name); \
	cJSON *key = cJSON_GetObjectItem(keyBindJson, "Key"); \
	if (cJSON_IsNumber(key)) \
		var.key = key->valueint; \
	cJSON *mode = cJSON_GetObjectItem(keyBindJson, "Mode"); \
	if (cJSON_IsNumber(mode)) \
		var.mode = mode->valueint; }

#define READ_COLORA(json, name, var) \
	{ int j = 0; \
	cJSON *colorAJson; \
	cJSON_ArrayForEach(colorAJson, cJSON_GetObjectItem(json, name)) { \
		if (cJSON_IsNumber(colorAJson)) \
			*(&var.r + j) = (float)colorAJson->valuedouble; \
		j++; \
	} }

#define READ_BONES(json, name, var) \
	{ int j = 0; \
	cJSON *boneJson; \
	cJSON_ArrayForEach(boneJson, cJSON_GetObjectItem(json, name)) { \
		if (cJSON_IsBool(boneJson)) \
			var[j] = boneJson->valueint; \
		j++; \
	} }

#define WRITE_KEYBIND(json, name, var) \
	{ cJSON *keyBindJson = cJSON_CreateObject(); \
	cJSON_AddNumberToObject(keyBindJson, "Key", var.key); \
	cJSON_AddNumberToObject(keyBindJson, "Mode", var.mode); \
	cJSON_AddItemToObject(json, name, keyBindJson); }

#define WRITE_COLORA(json, name, var) \
	{ cJSON *colorAJson = cJSON_AddArrayToObject(json, name); \
	for (int j = 0; j < 4; j++) \
		cJSON_AddItemToArray(colorAJson, cJSON_CreateNumber(*(&var.r + j))); } \

#define WRITE_BONES(json, name, var) \
	{ cJSON *bonesJson = cJSON_AddArrayToObject(json, name); \
	for (int j = 0; j < 6; j++) \
		cJSON_AddItemToArray(bonesJson, cJSON_CreateBool(var[j])); } \

Config config;

static char *getConfigDir(void)
{
	char *buf = malloc(PATH_MAX);
	if (!buf)
		return 0;

	char *dir = getenv("XDG_CONFIG_HOME");
	if (dir) {
		strncpy(buf, dir, PATH_MAX - 1);
		return buf;
	}

	dir = getenv("HOME");
	if (dir) {
		snprintf(buf, PATH_MAX, "%s/.config", dir);
		return buf;
	}

	struct passwd *pw = getpwuid(getuid());
	if (pw) {
		snprintf(buf, PATH_MAX, "%s/.config", pw->pw_dir);
		return buf;
	}

	return 0;
}

int config_getConfigs(char ***configs)
{
	char *dir = getConfigDir();
	if (!dir)
		return 0;

	char path[PATH_MAX];
	snprintf(path, PATH_MAX, "%s/ah4", dir);

	free(dir);

	DIR *d = opendir(path);
	if (!d)
		return 0;

	int len = 0;

	struct dirent *de;
	while ((de = readdir(d)))
		len++;

	*configs = malloc((len - 2) * sizeof(char *));

	int i = 0;

	seekdir(d, 0);
	while ((de = readdir(d))) {
		if (!strncmp(de->d_name, ".", PATH_MAX) || !strncmp(de->d_name, "..", PATH_MAX))
			continue;

		(*configs)[i] = malloc(PATH_MAX);
		strncpy((*configs)[i++], de->d_name, PATH_MAX);
	}

	closedir(d);

	return i;
}

void config_reset(void)
{
	for (int i = 0; i < LegitbotCategory_Len; i++) {
		config.legitbot[i].enabled = false;
		config.legitbot[i].silent = false;
		config.legitbot[i].visibleCheck = true;
		config.legitbot[i].smokeCheck = true;
		config.legitbot[i].flashCheck = true;
		config.legitbot[i].scopeCheck = true;
		config.legitbot[i].fov = 0;
		config.legitbot[i].smooth = 1;
		for (int j = 0; j < 6; j++)
			config.legitbot[i].bones[j] = true;
	}

	config.backtrack.enabled = false;
	config.backtrack.timeLimit = 0;

	for (int i = 0; i < GlowCategory_Len; i++) {
		config.glow[i].enabled = false;
		config.glow[i].healthBased = false;
		config.glow[i].colorA.r = 1;
		config.glow[i].colorA.g = 1;
		config.glow[i].colorA.b = 1;
		config.glow[i].colorA.a = 1;
	}

	for (int i = 0; i < ChamsCategory_Len; i++) {
		for (int j = 0; j < ChamsSubCategory_Len; j++) {
			config.chams[i][j].enabled = false;
			config.chams[i][j].healthBased = false;
			config.chams[i][j].colorA.r = 1;
			config.chams[i][j].colorA.g = 1;
			config.chams[i][j].colorA.b = 1;
			config.chams[i][j].colorA.a = 1;
		}
	}

	config.visuals.disablePostProcessing = false;
	config.visuals.disableShadows = false;
	config.visuals.forceCrosshair = false;
	config.visuals.grenadePrediction = false;
	config.visuals.revealRadar = false;
	config.visuals.revealRanks = false;
	config.visuals.revealOverwatch = false;

	config.misc.antiAfkKick = false;
	config.misc.bunnyHop = false;
	config.misc.jumpBug = false;
	config.misc.jumpBugKeyBind.key = 0;
	config.misc.jumpBugKeyBind.mode = 0;
	config.misc.edgeJump = false;
	config.misc.edgeJumpKeyBind.key = 0;
	config.misc.edgeJumpKeyBind.mode = 0;
	config.misc.fixMovement = true;
}

void config_load(const char *name)
{
	if (!*name)
		return;

	char *dir = getConfigDir();
	if (!dir)
		return;

	char path[PATH_MAX];
	snprintf(path, PATH_MAX, "%s/ah4/%s", dir, name);

	free(dir);

	int fd = open(path, O_RDONLY, S_IRUSR | S_IWUSR);

	struct stat sb;
	if (fstat(fd, &sb) < 0)
		return;

	if (!sb.st_size) {
		close(fd);
		return;
	}

	char *buf = mmap(0, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (!buf) {
		close(fd);
		return;
	}

	cJSON *json = cJSON_Parse(buf);
	if (json) {
		config_reset();

		int i = 0;
		cJSON *legitbotJson;
		cJSON_ArrayForEach(legitbotJson, cJSON_GetObjectItem(json, "Legitbot")) {
			READ_BOOL(legitbotJson, "Enabled", config.legitbot[i].enabled)
			READ_BOOL(legitbotJson, "Silent", config.legitbot[i].silent)
			READ_BOOL(legitbotJson, "Visible check", config.legitbot[i].visibleCheck)
			READ_BOOL(legitbotJson, "Smoke check", config.legitbot[i].smokeCheck)
			READ_BOOL(legitbotJson, "Flash check", config.legitbot[i].flashCheck)
			READ_BOOL(legitbotJson, "Scope check", config.legitbot[i].flashCheck)
			READ_FLOAT(legitbotJson, "FOV", config.legitbot[i].fov)
			READ_FLOAT(legitbotJson, "Smooth", config.legitbot[i].smooth)
			READ_BONES(legitbotJson, "Bones", config.legitbot[i].bones)

			i++;
		}

		cJSON *backtrackJson = cJSON_GetObjectItem(json, "Backtrack");
		READ_BOOL(backtrackJson, "Enabled", config.backtrack.enabled)
		READ_INT(backtrackJson, "Time limit", config.backtrack.timeLimit)

		i = 0;
		cJSON *glowJson;
		cJSON_ArrayForEach(glowJson, cJSON_GetObjectItem(json, "Glow")) {
			READ_BOOL(glowJson, "Enabled", config.glow[i].enabled)
			READ_BOOL(glowJson, "Health based", config.glow[i].healthBased)
			READ_COLORA(glowJson, "Color", config.glow[i].colorA)

			i++;
		}

		i = 0;
		cJSON *chamsJson;
		cJSON_ArrayForEach(chamsJson, cJSON_GetObjectItem(json, "Chams")) {
			cJSON *visible = cJSON_GetObjectItem(chamsJson, "Visible");

			READ_BOOL(visible, "Enabled", config.chams[i][ChamsSubCategory_Visible].enabled)
			READ_BOOL(visible, "Health based", config.chams[i][ChamsSubCategory_Visible].healthBased)
			READ_COLORA(visible, "Color", config.chams[i][ChamsSubCategory_Visible].colorA)

			cJSON *occluded = cJSON_GetObjectItem(chamsJson, "Occluded");

			READ_BOOL(occluded, "Enabled", config.chams[i][ChamsSubCategory_Occluded].enabled)
			READ_BOOL(occluded, "Health based", config.chams[i][ChamsSubCategory_Occluded].healthBased)
			READ_COLORA(occluded, "Color", config.chams[i][ChamsSubCategory_Occluded].colorA)

			i++;
		}

		cJSON *visualsJson = cJSON_GetObjectItem(json, "Visuals");
		READ_BOOL(visualsJson, "Disable post-processing", config.visuals.disablePostProcessing)
		READ_BOOL(visualsJson, "Disable shadows", config.visuals.disableShadows)
		READ_BOOL(visualsJson, "Force crosshair", config.visuals.forceCrosshair)
		READ_BOOL(visualsJson, "Grenade prediction", config.visuals.grenadePrediction)
		READ_BOOL(visualsJson, "Reveal radar", config.visuals.revealRadar)
		READ_BOOL(visualsJson, "Reveal ranks", config.visuals.revealRanks)
		READ_BOOL(visualsJson, "Reveal Overwatch", config.visuals.revealOverwatch)

		cJSON *miscJson = cJSON_GetObjectItem(json, "Misc");
		READ_BOOL(miscJson, "Anti AFK kick", config.misc.antiAfkKick)
		READ_BOOL(miscJson, "Bunny hop", config.misc.bunnyHop)
		READ_BOOL(miscJson, "Jump bug", config.misc.jumpBug)
		READ_KEYBIND(miscJson, "Jump bug key bind", config.misc.jumpBugKeyBind);
		READ_BOOL(miscJson, "Edge jump", config.misc.edgeJump)
		READ_KEYBIND(miscJson, "Edge jump key bind", config.misc.edgeJumpKeyBind);
		READ_BOOL(miscJson, "Fix movement", config.misc.fixMovement)

		cJSON_Delete(json);
	}

	munmap(buf, sb.st_size);
	close(fd);
}

void config_save(const char *name)
{
	if (!*name)
		return;

	char *dir = getConfigDir();
	if (!dir)
		return;

	char path[PATH_MAX];
	snprintf(path, PATH_MAX, "%s/ah4", dir);

	mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);

	snprintf(path, PATH_MAX, "%s/ah4/%s", dir, name);

	free(dir);

	FILE *f = fopen(path, "w");
	if (!f)
		return;

	cJSON *json = cJSON_CreateObject();
	if (json) {
		cJSON *legitbotJson = cJSON_AddArrayToObject(json, "Legitbot");
		for (int i = 0; i < LegitbotCategory_Len; i++) {
			cJSON *legitbotSubJson = cJSON_CreateObject();

			cJSON_AddBoolToObject(legitbotSubJson, "Enabled", config.legitbot[i].enabled);
			cJSON_AddBoolToObject(legitbotSubJson, "Silent", config.legitbot[i].silent);
			cJSON_AddBoolToObject(legitbotSubJson, "Visible check", config.legitbot[i].visibleCheck);
			cJSON_AddBoolToObject(legitbotSubJson, "Smoke check", config.legitbot[i].smokeCheck);
			cJSON_AddBoolToObject(legitbotSubJson, "Flash check", config.legitbot[i].flashCheck);
			cJSON_AddBoolToObject(legitbotSubJson, "Scope check", config.legitbot[i].scopeCheck);
			cJSON_AddNumberToObject(legitbotSubJson, "FOV", config.legitbot[i].fov);
			cJSON_AddNumberToObject(legitbotSubJson, "Smooth", config.legitbot[i].smooth);
			WRITE_BONES(legitbotSubJson, "Bones", config.legitbot[i].bones);

			cJSON_AddItemToArray(legitbotJson, legitbotSubJson);
		}

		cJSON *backtrackJson = cJSON_CreateObject();
		cJSON_AddBoolToObject(backtrackJson, "Enabled", config.backtrack.enabled);
		cJSON_AddNumberToObject(backtrackJson, "Time limit", config.backtrack.timeLimit);
		cJSON_AddItemToObject(json, "Backtrack", backtrackJson);

		cJSON *glowJson = cJSON_AddArrayToObject(json, "Glow");
		for (int i = 0; i < GlowCategory_Len; i++) {
			cJSON *glowSubJson = cJSON_CreateObject();

			cJSON_AddBoolToObject(glowSubJson, "Enabled", config.glow[i].enabled);
			cJSON_AddBoolToObject(glowSubJson, "Health based", config.glow[i].healthBased);
			WRITE_COLORA(glowSubJson, "Color", config.glow[i].colorA)

			cJSON_AddItemToArray(glowJson, glowSubJson);
		}
		
		cJSON *chamsJson = cJSON_AddArrayToObject(json, "Chams");
		for (int i = 0; i < ChamsCategory_Len; i++) {
			cJSON *chamsSubJson = cJSON_CreateObject();

			cJSON *visible = cJSON_CreateObject();

			cJSON_AddBoolToObject(visible, "Enabled", config.chams[i][ChamsSubCategory_Visible].enabled);
			cJSON_AddBoolToObject(visible, "Health based", config.chams[i][ChamsSubCategory_Visible].healthBased);
			WRITE_COLORA(visible, "Color", config.chams[i][ChamsSubCategory_Visible].colorA)

			cJSON_AddItemToObject(chamsSubJson, "Visible", visible);

			cJSON *occluded = cJSON_CreateObject();

			cJSON_AddBoolToObject(occluded, "Enabled", config.chams[i][ChamsSubCategory_Occluded].enabled);
			cJSON_AddBoolToObject(occluded, "Health based", config.chams[i][ChamsSubCategory_Occluded].healthBased);
			WRITE_COLORA(occluded, "Color", config.chams[i][ChamsSubCategory_Occluded].colorA)

			cJSON_AddItemToObject(chamsSubJson, "Occluded", occluded);

			cJSON_AddItemToArray(chamsJson, chamsSubJson);
		}

		cJSON *visualsJson = cJSON_CreateObject();
		cJSON_AddBoolToObject(visualsJson, "Disable post-processing", config.visuals.disablePostProcessing);
		cJSON_AddBoolToObject(visualsJson, "Disable shadows", config.visuals.disableShadows);
		cJSON_AddBoolToObject(visualsJson, "Force crosshair", config.visuals.forceCrosshair);
		cJSON_AddBoolToObject(visualsJson, "Grenade prediction", config.visuals.grenadePrediction);
		cJSON_AddBoolToObject(visualsJson, "Reveal radar", config.visuals.revealRadar);
		cJSON_AddBoolToObject(visualsJson, "Reveal ranks", config.visuals.revealRanks);
		cJSON_AddBoolToObject(visualsJson, "Reveal Overwatch", config.visuals.revealOverwatch);
		cJSON_AddItemToObject(json, "Visuals", visualsJson);

		cJSON *miscJson = cJSON_CreateObject();
		cJSON_AddBoolToObject(miscJson, "Anti AFK kick", config.misc.antiAfkKick);
		cJSON_AddBoolToObject(miscJson, "Bunny hop", config.misc.bunnyHop);
		cJSON_AddBoolToObject(miscJson, "Jump bug", config.misc.jumpBug);
		WRITE_KEYBIND(miscJson, "Jump bug key bind", config.misc.jumpBugKeyBind)
		cJSON_AddBoolToObject(miscJson, "Edge jump", config.misc.edgeJump);
		WRITE_KEYBIND(miscJson, "Edge jump key bind", config.misc.edgeJumpKeyBind)
		cJSON_AddBoolToObject(miscJson, "Fix movement", config.misc.fixMovement);
		cJSON_AddItemToObject(json, "Misc", miscJson);

		fprintf(f, cJSON_Print(json));
		fclose(f);

		cJSON_Delete(json);
	}
}

void config_openDirectory(void)
{
	char *dir = getConfigDir();
	if (!dir)
		return;

	char path[PATH_MAX];
	snprintf(path, PATH_MAX, "%s/ah4", dir);

	mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);

	char cmd[512];
	snprintf(cmd, 512, "xdg-open \"%s/ah4\" &", dir);

	free(dir);

	system(cmd);
}
