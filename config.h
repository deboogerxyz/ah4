#define READ_BOOL(json, name, var) \
	{ cJSON *item = cJSON_GetObjectItem(json, name); \
	if (cJSON_IsBool(item)) \
		var = item->valueint; }

void config_load(const char *name);
