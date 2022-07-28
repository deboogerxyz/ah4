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

#define READ_KEYBIND(json, name, var) \
	{ cJSON *keyBindJson = cJSON_GetObjectItem(json, name); \
	cJSON *key = cJSON_GetObjectItem(keyBindJson, "Key"); \
	if (cJSON_IsNumber(key)) \
		var.key = key->valueint; \
	cJSON *mode = cJSON_GetObjectItem(keyBindJson, "Mode"); \
	if (cJSON_IsNumber(mode)) \
		var.mode = mode->valueint; }

#define WRITE_KEYBIND(json, name, var) \
	{ cJSON *keyBindJson = cJSON_CreateObject(); \
	cJSON_AddNumberToObject(keyBindJson, "Key", var.key); \
	cJSON_AddNumberToObject(keyBindJson, "Mode", var.mode); \
	cJSON_AddItemToObject(json, name, keyBindJson); }

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
	config.backtrack.enabled = false;
	config.backtrack.timeLimit = 0;

	config.misc.jumpBug = 0;
	config.misc.jumpBugKeyBind.key = 0;
	config.misc.jumpBugKeyBind.mode = 0;
	config.misc.edgeJump = 0;
	config.misc.edgeJumpKeyBind.key = 0;
	config.misc.edgeJumpKeyBind.mode = 0;
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

		cJSON *backtrackJson = cJSON_GetObjectItem(json, "Backtrack");
		READ_BOOL(backtrackJson, "Enabled", config.backtrack.enabled)
		READ_INT(backtrackJson, "Time limit", config.backtrack.timeLimit)

		cJSON *miscJson = cJSON_GetObjectItem(json, "Misc");
		READ_BOOL(miscJson, "Jump bug", config.misc.jumpBug)
		READ_KEYBIND(miscJson, "Jump bug key bind", config.misc.jumpBugKeyBind);
		READ_BOOL(miscJson, "Edge jump", config.misc.edgeJump)
		READ_KEYBIND(miscJson, "Edge jump key bind", config.misc.edgeJumpKeyBind);

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
		cJSON *backtrackJson = cJSON_CreateObject();
		cJSON_AddBoolToObject(backtrackJson, "Enabled", config.backtrack.enabled);
		cJSON_AddNumberToObject(backtrackJson, "Time limit", config.backtrack.timeLimit);
		cJSON_AddItemToObject(json, "Backtrack", backtrackJson);

		cJSON *miscJson = cJSON_CreateObject();
		cJSON_AddBoolToObject(miscJson, "Jump bug", config.misc.jumpBug);
		WRITE_KEYBIND(miscJson, "Jump bug key bind", config.misc.jumpBugKeyBind)
		cJSON_AddBoolToObject(miscJson, "Edge jump", config.misc.edgeJump);
		WRITE_KEYBIND(miscJson, "Edge jump key bind", config.misc.edgeJumpKeyBind)
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

	char cmd[512];
	snprintf(cmd, 512, "xdg-open \"%s/ah4\" &", dir);

	free(dir);

	system(cmd);
}
