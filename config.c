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

#include "memory.h"

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

	if (!sb.st_size)
		return;

	char *buf = mmap(0, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (!buf) {
		close(fd);
		return;
	}

	cJSON *json = cJSON_Parse(buf);
	if (json) {
		// ...

		cJSON_Delete(json);
	}

	munmap(buf, sb.st_size);
	close(fd);
}
