#ifndef NETVARS_H_
#define NETVARS_H_

#include "sdk.h"

int netvars_getOffset(const char *name);

#define NETVARS_DECL(name, type) type *Entity_##name(Entity *entity);
#define NETVARS_IMPL(name, className, varName, offset, type) \
	type * \
	Entity_##name(Entity *entity) \
	{ \
		static int netvarOffset = 0; \
		if (!netvarOffset) \
			netvarOffset = netvars_getOffset(className "->" varName); \
		return (type *)((char *)entity + netvarOffset + offset); \
	}

void netvars_init(void);
void netvars_cleanUp(void);

#endif // NETVARS_H_
