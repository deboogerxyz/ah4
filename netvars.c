#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interfaces.h"
#include "sdk.h"
#include "utils.h"

#include "netvars.h"

typedef struct NetVar NetVar;
struct NetVar {
	const char *name;
	int offset;

	NetVar *next;
};

#define NETVARS_LEN 2000
NetVar *netVars[NETVARS_LEN] = {0};

static void dump(const char *className, RecvTable *recvTable, int offset)
{
	for (int i = 0; i < recvTable->propCount; i++) {
		RecvProp *prop = &recvTable->props[i];
		if (!prop || isdigit(prop->name[0]))
			continue;

		if (!strcmp(prop->name, "baseclass"))
			continue;

		if (prop->type == 6 && prop->recvTable && prop->recvTable->name[0] == 'D')
			dump(className, prop->recvTable, prop->offset + offset);

		NetVar *netVar = malloc(sizeof(NetVar));
		if (!netVar)
			continue;

		char *name = malloc(128);
		if (!name)
			continue;

		snprintf(name, 128, "%s->%s", className, prop->name);

		netVar->name = name;
		netVar->offset = prop->offset + offset;

		int j = hash(netVar->name) % NETVARS_LEN;
		netVar->next = netVars[j];
		netVars[j] = netVar;
	}
}

int netvars_getOffset(const char *name)
{
	int i = hash(name) % NETVARS_LEN;

	NetVar *netVar = netVars[i];
	while (netVar && strcmp(netVar->name, name))
		netVar = netVar->next;

	if (!netVar)
		return -1;

	return netVar->offset;
}

void netvars_init(void)
{
	for (ClientClass *class = interfaces.client->vmt->getAllClasses(interfaces.client); class; class = class->next)
		if (class->recvTable)
			dump(class->name, class->recvTable, 0);
}

void netvars_cleanUp(void)
{
	for (int i = 0; i < NETVARS_LEN; i++) {
		NetVar *netVar = netVars[i];
		while (netVar) {
			NetVar *next = netVar->next;
			free(netVar);
			netVar = next;
		}
	}
}
