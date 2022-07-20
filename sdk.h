#ifndef SDK_H_
#define SDK_H_

#include <stdbool.h>
#include <stdint.h>

#define CLIENT_SO             "csgo/bin/linux64/client_client.so"
#define ENGINE_SO             "engine_client.so"
#define FILESYSTEM_SO         "filesystem_stdio_client.so"
#define INPUTSYSTEM_SO        "inputsystem_client.so"
#define LOCALIZE_SO           "localize_client.so"
#define MATERIALSYSTEM_SO     "materialsystem_client.so"
#define PANORAMA_SO           "panorama_gl_client.so"
#define SERVERBROWSER_SO      "bin/linux64/serverbrowser_client.so"
#define SOUNDEMITTERSYSTEM_SO "soundemittersystem_client.so"
#define STUDIORENDER_SO       "studiorender_client.so"
#define TIER0_SO              "libtier0_client.so"
#define VGUI2_SO              "vgui2_client.so"
#define VGUIMATSURFACE_SO     "vguimatsurface_client.so"
#define VPHYSICS_SO           "vphysics_client.so"
#define VSTDLIB_SO            "libvstdlib_client.so"

#define CAT(a, b)    a##b
#define PADNAME(x)   CAT(pad, x)
#define PAD(type, n) type PADNAME(__LINE__)[n];

typedef struct {
	float x, y, z;
} Vector;

typedef struct RecvProp RecvProp;

typedef struct {
	const RecvProp *recvProp;
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
struct RecvProp {
	const char *name;
	int type;
	int flags;
	int stringBufferSize;
	int insideArray;
	const void *extraData;
	RecvProp *arrayProp;
	void *arrayLengthProxy;
	RecvProxy proxy;
	void *dataTableProxy;
	RecvTable *recvTable;
	int offset;
	int elementStride;
	int elementCount;
	const char *parrentArrayPropName;
} ;

struct RecvTable {
	RecvProp *props;
	int propCount;
	void *decoder;
	const char *name;
	bool isInitialized;
	bool isInMainList;
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

typedef void *Entity;

typedef struct ClientClass ClientClass;
struct ClientClass {
	Entity *(*createFunction)(int, int);
	void *createEventFunction;
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
	PAD(void *, 8)
	ClientClass *(*getAllClasses)(Client *);
	PAD(void *, 28)
	void (*frameStageNotify)(Client *, FrameStage stage);
	int (*dispatchUserMessage)(Client *, int type, int flags, int size, void *data);
} ClientVMT;

struct Client {
	ClientVMT *vmt;
};

typedef enum {
	IN_ATTACK = 1 << 0,
	IN_JUMP = 1 << 1,
	IN_DUCK = 1 << 2,
	IN_FORWARD = 1 << 3,
	IN_BACK = 1 << 4,
	IN_USE = 1 << 5,
	IN_MOVELEFT = 1 << 9,
	IN_MOVERIGHT = 1 << 10,
	IN_ATTACK2 = 1 << 11,
	IN_SCORE = 1 << 16,
	IN_BULLRUSH = 1 << 22
} UserCmdButtons;

typedef struct {
	void *vmt;
	int commandNumber;
	int tickCount;
	Vector viewAngles;
	Vector aimDirection;
	float forwardMove;
	float sideMove;
	float upMove;
	UserCmdButtons buttons;
	int impulse;
	int weaponSelect;
	int weaponSubtype;
	int randomSeed;
	short mousedx;
	short mousedy;
	char hasBeenPredicted;
} UserCmd;

typedef struct ClientMode ClientMode;

typedef struct {
	PAD(void *, 18)
	bool (*shouldDrawFog)(ClientMode *);
	void (*overrideView)(ClientMode *);
	PAD(void *, 5)
	bool (*createMove)(ClientMode *, float, UserCmd *cmd);
	PAD(void *, 19)
	void (*doPostScreenEffects)(ClientMode *, void *);
} ClientModeVMT;

struct ClientMode {
	ClientModeVMT *vmt;
};

typedef struct {
	uint64_t version;
	uint64_t xuid;
	char name[128];
	int userID;
	char guid[33];
	uint32_t friendsID;
	char friendsName[128];
	bool fakePlayer;
	bool hltv;
	int customFiles[4];
	uint8_t filesDownloaded;
} PlayerInfo;

typedef float Matrix4x4[4][4];

typedef struct Engine Engine;

typedef struct {
	PAD(void *, 5)
	void (*getScreenSize)(Engine *, int *w, int *h); // 5
	PAD(void *, 2)
	bool (*getPlayerInfo)(Engine *, int entityIndex, PlayerInfo *); // 8
	int (*getPlayerForUserID)(Engine *, int); // 9
	PAD(void *, 2)
	int (*getLocalPlayer)(Engine *); // 12
	PAD(void *, 5)
	void (*getViewAngles)(Engine *, Vector *); // 18
	void (*setViewAngles)(Engine *, const Vector *); // 19
	int (*getMaxClients)(Engine *); // 20
	PAD(void *, 5)
	bool (*isInGame)(Engine *); // 26
	bool (*isConnected)(Engine *); // 27
	PAD(void *, 6)
	bool (*cullBox)(Engine *, const Vector *mins, const Vector *maxs); // 34
	PAD(void *, 2)
	const Matrix4x4 *(*worldToScreenMatrix)(Engine *); // 37
	PAD(void *, 5)
	void *(*getBSPTreeQuery)(Engine *); // 43
	PAD(void *, 9)
	const char *(*getLevelName)(Engine *); // 53
	PAD(void *, 24)
	void *(*getNetworkChannel)(Engine *); // 78
	PAD(void *, 13)
	bool (*isTakingScreenshot)(Engine *); // 92
	PAD(void *, 15)
	void *(*executeClientCmd)(Engine *, const char *); // 108
	PAD(void *, 4)
	void *(*clientCmdUnrestricted)(Engine *, const char *, bool fromConsoleOrKeybind); // 113
	PAD(void *, 111)
	bool (*isVoiceRecording)(Engine *); // 225
} EngineVMT;

struct Engine {
	EngineVMT *vmt;
};

#endif // SDK_H_
