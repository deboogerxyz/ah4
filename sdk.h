#ifndef SDK_H_
#define SDK_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	float x, y, z;
} Vector;

typedef struct {
	char pad0[8];
	union {
		float f;
		long i;
		const char *s;
		void *p;
		Vector v;
		int64_t i64;
	} val;
} RecvProxyData;

typedef void (*RecvProxy)(RecvProxyData *, void *, void *);

typedef struct RecvTable RecvTable;
typedef struct {
	const char *name;
	int type;
	char pad0[36];
	RecvProxy recvProxy;
	char pad1[8];
	RecvTable *recvTable;
	int offset;
	char pad2[16];
} RecvProp;

struct RecvTable {
	RecvProp *props;
	int propCount;
	char pad0[8];
	const char *name;
	char pad1[2];
};

typedef enum {
	ClassID_BaseCSGrenadeProjectile = 9,
	ClassID_BreachChargeProjectile = 29,
	ClassID_BumpMineProjectile = 33,
	ClassID_C4,
	ClassID_Chicken = 36,
	ClassID_CSPlayer = 40,
	ClassID_CSPlayerResource,
	ClassID_CSRagdoll = 42,
	ClassID_Deagle = 46,
	ClassID_DecoyProjectile = 48,
	ClassID_Drone,
	ClassID_Dronegun,
	ClassID_DynamicProp = 52,
	ClassID_EconEntity = 53,
	ClassID_EconWearable,
	ClassID_Hostage = 97,
	ClassID_Inferno = 100,
	ClassID_Healthshot = 104,
	ClassID_Cash,
	ClassID_Knife = 107,
	ClassID_KnifeGG,
	ClassID_MolotovProjectile = 114,
	ClassID_AmmoBox = 125,
	ClassID_LootCrate,
	ClassID_RadarJammer,
	ClassID_WeaponUpgrade,
	ClassID_PlantedC4,
	ClassID_PropDoorRotating = 143,
	ClassID_SensorGrenadeProjectile = 153,
	ClassID_SmokeGrenadeProjectile = 157,
	ClassID_SnowballPile = 160,
	ClassID_SnowballProjectile,
	ClassID_Tablet = 172,
	ClassID_Aug = 232,
	ClassID_Awp,
	ClassID_Elite = 239,
	ClassID_FiveSeven = 241,
	ClassID_G3sg1,
	ClassID_Glock = 245,
	ClassID_P2000,
	ClassID_P250 = 258,
	ClassID_Scar20 = 261,
	ClassID_Sg553 = 265,
	ClassID_Ssg08 = 267,
	ClassID_Tec9 = 269
} ClassID;

typedef struct ClientClass ClientClass;
struct ClientClass {
	char pad0[16];
	const char *name;
	RecvTable *recvTable;
	ClientClass *next;
	ClassID classID;
};

typedef enum {
	UNDEFINED = -1,
	START,
	NET_UPDATE_START,
	NET_UPDATE_POSTDATAUPDATE_START,
	NET_UPDATE_POSTDATAUPDATE_END,
	NET_UPDATE_END,
	RENDER_START,
	RENDER_END
} FrameStage;

typedef struct Client Client;

typedef struct {
	void (*pad0[8])();
	ClientClass *(*getAllClasses)(Client *);
	void (*pad1[28])();
	void (*frameStageNotify)(Client *, FrameStage stage);
	int (*dispatchUserMessage)(Client *, int type, int flags, int size, void *data);
} ClientVMT;

struct Client {
	ClientVMT *vmt;
};

typedef void *UserCmd;

typedef struct ClientMode ClientMode;

typedef struct {
	void (*pad0[18])();
	bool (*shouldDrawFog)(ClientMode *);
	void (*overrideView)(ClientMode *);
	void (*pad1[5])();
	bool (*createMove)(ClientMode *, float, UserCmd *cmd);
	void (*pad2[19])();
	void (*doPostScreenEffects)(ClientMode *, void *);
} ClientModeVMT;

struct ClientMode {
	ClientModeVMT *vmt;
};

#endif /* SDK_H_ */
