#ifndef SDK_H_
#define SDK_H_

#include <stdbool.h>
#include <stdint.h>

#include "netvars.h"

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
	uint8_t r, g, b, a;
} Color8;

typedef struct {
	float r, g, b;
} Color;

typedef struct {
	float r, g, b, a;
} ColorA;

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

typedef struct Entity Entity;

typedef void (*RecvProxy)(RecvProxyData *data, void *outStruct, void *out);

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

typedef struct Collideable Collideable;

typedef struct {
	PAD(void *, 1)
	Vector *(*obbMins)(Collideable *); // 1
	Vector *(*obbMaxs)(Collideable *); // 2
} CollideableVMT;

struct Collideable {
	CollideableVMT *vmt;
};

typedef struct ClientClass ClientClass;

typedef struct Networkable Networkable;

typedef struct {
	PAD(void *, 1)
	void (*release)(Networkable *); // 1
	ClientClass *(*getClientClass)(Networkable *); // 2
	PAD(void *, 2)
	void (*onDataChanged)(Networkable *, int updateType); // 5
	void (*preDataUpdate)(Networkable *, int updateType); // 6
	void (*postDataUpdate)(Networkable *, int updateType); // 7
	PAD(void *, 1)
	bool (*isDormant)(Networkable *); // 9
	int (*getIndex)(Networkable *); // 10
	PAD(void *, 2)
	int (*setDestroyedOnRecreateEntities)(Networkable *); // 13
} NetworkableVMT;

struct Networkable {
	NetworkableVMT *vmt;
};

typedef enum {
	Team_None,
	Team_Spectators,
	Team_TT,
	Team_CT
} Team;

typedef enum {
	ObserverMode_None,
	ObserverMode_Deathcam,
	ObserverMode_Freezecam,
	ObserverMode_Fixed,
	ObserverMode_InEye,
	ObserverMode_Chase,
	ObserverMode_Roaming
} ObserverMode;

typedef enum {
	WeaponType_Knife,
	WeaponType_Pistol,
	WeaponType_SubMachinegun,
	WeaponType_Rifle,
	WeaponType_Shotgun,
	WeaponType_SniperRifle,
	WeaponType_Machinegun,
	WeaponType_C4,
	WeaponType_Placeholder,
	WeaponType_Grenade,
	WeaponType_Unknown,
	WeaponType_StackableItem,
	WeaponType_Fists,
	WeaponType_BreachCharge,
	WeaponType_BumpMine,
	WeaponType_Tablet,
	WeaponType_Melee
} WeaponType;

typedef struct {
	PAD(char, 32)
	int maxClip;
	PAD(char, 204)
	const char *name;
	PAD(char, 72)
	WeaponType type;
	PAD(char, 4)
	int price;
	PAD(char, 12)
	float cycleTime;
	PAD(char, 12)
	bool fullAuto;
	PAD(char, 3)
	int damage;
	float headshotMultiplier;
	float armorRatio;
	int bullets;
	float penetration;
	PAD(char, 8)
	float range;
	float rangeModifier;
	PAD(char, 16)
	bool silencer;
	PAD(char, 23)
	float maxSpeed;
	float maxSpeedAlt;
	PAD(char, 100)
	float recoilMagnitude;
	float recoilMagnitudeAlt;
	PAD(char, 16)
	float recoveryTimeStand;
} WeaponInfo;

typedef enum {
	WeaponID_None = 0,
	WeaponID_Deagle = 1,
	WeaponID_Elite,
	WeaponID_Fiveseven,
	WeaponID_Glock,
	WeaponID_Ak47 = 7,
	WeaponID_Aug,
	WeaponID_Awp,
	WeaponID_Famas,
	WeaponID_G3SG1,
	WeaponID_GalilAr = 13,
	WeaponID_M249,
	WeaponID_M4A1 = 16,
	WeaponID_Mac10,
	WeaponID_P90 = 19,
	WeaponID_ZoneRepulsor,
	WeaponID_Mp5sd = 23,
	WeaponID_Ump45,
	WeaponID_Xm1014,
	WeaponID_Bizon,
	WeaponID_Mag7,
	WeaponID_Negev,
	WeaponID_Sawedoff,
	WeaponID_Tec9,
	WeaponID_Taser,
	WeaponID_Hkp2000,
	WeaponID_Mp7,
	WeaponID_Mp9,
	WeaponID_Nova,
	WeaponID_P250,
	WeaponID_Shield,
	WeaponID_Scar20,
	WeaponID_Sg553,
	WeaponID_Ssg08,
	WeaponID_GoldenKnife,
	WeaponID_Knife,
	WeaponID_Flashbang = 43,
	WeaponID_HeGrenade,
	WeaponID_SmokeGrenade,
	WeaponID_Molotov,
	WeaponID_Decoy,
	WeaponID_IncGrenade,
	WeaponID_C4,
	WeaponID_Healthshot = 57,
	WeaponID_KnifeT = 59,
	WeaponID_M4a1_s,
	WeaponID_Usp_s,
	WeaponID_Cz75a = 63,
	WeaponID_Revolver,
	WeaponID_TaGrenade = 68,
	WeaponID_Axe = 75,
	WeaponID_Hammer,
	WeaponID_Spanner = 78,
	WeaponID_GhostKnife = 80,
	WeaponID_Firebomb,
	WeaponID_Diversion,
	WeaponID_FragGrenade,
	WeaponID_Snowball,
	WeaponID_BumpMine,
	WeaponID_Bayonet = 500,
	WeaponID_ClassicKnife = 503,
	WeaponID_Flip = 505,
	WeaponID_Gut,
	WeaponID_Karambit,
	WeaponID_M9Bayonet,
	WeaponID_Huntsman,
	WeaponID_Falchion = 512,
	WeaponID_Bowie = 514,
	WeaponID_Butterfly,
	WeaponID_Daggers,
	WeaponID_Paracord,
	WeaponID_SurvivalKnife,
	WeaponID_Ursus = 519,
	WeaponID_Navaja,
	WeaponID_NomadKnife,
	WeaponID_Stiletto = 522,
	WeaponID_Talon,
	WeaponID_SkeletonKnife = 525,
	WeaponID_GloveStuddedBrokenfang = 4725,
	WeaponID_GloveStuddedBloodhound = 5027,
	WeaponID_GloveT,
	WeaponID_GloveCT,
	WeaponID_GloveSporty,
	WeaponID_GloveSlick,
	WeaponID_GloveLeatherWrap,
	WeaponID_GloveMotorcycle,
	WeaponID_GloveSpecialist,
	WeaponID_GloveHydra
} WeaponID;

typedef enum {
	MoveType_NoClip = 8,
	MoveType_Ladder = 8,
} MoveType;

typedef struct {
	PAD(void *, 3)
	int *(*getHandle)(Entity *); // 3
	Collideable *(*getCollideable)(Entity *); // 4
	Networkable *(*getNetworkable)(Entity *); // 5
	PAD(void *, 6)
	Vector *(*getAbsoluteOrigin)(Entity *); // 12
	PAD(void *, 98)
	void (*setModelIndex)(Entity *, int); // 111
	PAD(void *, 10)
	bool (*getAttachment)(Entity *, int, Vector *origin); // 122
	PAD(void *, 5)
	Team (*getTeamNumber)(Entity *); // 128
	PAD(void *, 38)
	int (*getHealth)(Entity *); // 167
	PAD(void *, 40)
	bool (*isAlive)(Entity *); // 208
	PAD(void *, 1)
	bool (*isPlayer)(Entity *); // 210
	PAD(void *, 7)
	bool (*isWeapon)(Entity *); // 218
	PAD(void *, 112)
	Entity *(*getActiveWeapon)(Entity *); // 331
	PAD(void *, 16)
	Vector (*getEyePosition)(Entity *); // 348
	PAD(void *, 1)
	int (*getWeaponSubtype)(Entity *); // 350
	PAD(void *, 6)
	ObserverMode (*getObserverMode)(Entity *); // 357
	Entity *(*getObserverTarget)(Entity *); // 358
	PAD(void *, 50)
	Vector (*getAimPunch)(Entity *); // 409
	PAD(void *, 102)
	float (*getSpread)(Entity *); // 521
	PAD(void *, 1)
	WeaponType (*getWeaponType)(Entity *); // 523
	PAD(void *, 5)
	WeaponInfo *(*getWeaponInfo)(Entity *); // 529
	PAD(void *, 6)
	int (*getMuzzleAttachmentIndex1stPerson)(Entity *, Entity *viewModel); // 536
	int (*getMuzzleAttachmentIndex3stPerson)(Entity *); // 537
	PAD(void *, 13)
	float (*getInaccuracy)(Entity *); // 551
	void (*updateAccuracyPenalty)(Entity *); // 552
} EntityVMT;

typedef float Matrix3x4[3][4];

typedef struct {
	void *handle;
	char name[260];
	int loadFlags;
	int serverCount;
	int type;
	int flags;
	Vector mins, maxs;
} Model;

typedef struct Renderable Renderable;

typedef struct {
	Vector origin;
	Vector angles;
	PAD(char, 4)
	const Renderable *renderable;
	const Model *model;
	const Matrix3x4 *modelToWorld;
	const Matrix3x4 *lightingOffset;
	const Vector *lightingOrigin;
	int flags;
	int entityIndex;
} ModelRenderInfo;

typedef struct {
	PAD(void *, 5)
	bool (*shouldDraw)(Renderable *); // 5
	PAD(void *, 2)
	const Model *(*getModel)(Renderable *); // 8
	PAD(void *, 4)
	bool (*setupBones)(Renderable *, Matrix3x4 *out, int max, int mask, float currentTime); // 13
	PAD(void *, 18)
	const Matrix3x4 *(*toWorldTransform)(Renderable *); // 32
} RenderableVMT;

struct Renderable {
	RenderableVMT *vmt;
};

struct Entity {
	EntityVMT *vmt;
};

struct ClientClass {
	Entity *(*createFunction)(int, int);
	Entity *(*createEventFunction)(void);
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
	ClientClass *(*getAllClasses)(Client *); // 8
	PAD(void *, 28)
	void (*frameStageNotify)(Client *, FrameStage stage); // 37
	int (*dispatchUserMessage)(Client *, int type, int flags, int size, void *data); // 38
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
	bool (*shouldDrawFog)(ClientMode *); // 18
	void (*overrideView)(ClientMode *); // 19
	PAD(void *, 5)
	bool (*createMove)(ClientMode *, float, UserCmd *cmd); // 25
	PAD(void *, 19)
	void (*doPostScreenEffects)(ClientMode *, void *); // 45
} ClientModeVMT;

struct ClientMode {
	ClientModeVMT *vmt;
};

typedef struct {
	PAD(char, 16)
	bool isTrackIRAvailable;
	bool isMouseInitialized;
	bool isMouseActive;
	PAD(char, 162)
	bool IsCameraInThirdPerson;
	bool cameraMovingWithMouse;
	Vector cameraOffset;
} Input;

typedef struct ConVar ConVar;

typedef struct {
	PAD(void *, 15)
	float (*getFloat)(ConVar *); // 15
	int (*getInt)(ConVar *); // 16
	void (*setString)(ConVar *, const char *); // 17
	void (*setFloat)(ConVar *, float); // 18
	void (*setInt)(ConVar *, int); // 19
} ConVarVMT;

struct ConVar {
	ConVarVMT *vmt;
	PAD(char, 40)
	void (*changeCallback)(void);
	ConVar *parent;
	const char *defaultValue;
	char *string;
};

typedef struct Cvar Cvar;

typedef struct {
	PAD(void *, 15)
	ConVar *(*findVar)(Cvar *, const char *name); // 15
} CvarVMT;

struct Cvar {
	CvarVMT *vmt;
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

typedef struct {
	uint64_t caseID;
	uint32_t headerPrefixLength;
	uint32_t lockFirstPersonAccountID;
	bool anonymousPlayerIdentity;
	PAD(char, 23)
} DemoPlaybackParameters;

typedef struct NetworkChannel NetworkChannel;

typedef struct {
	PAD(void *, 1)
	const char *(*getServerAddress)(NetworkChannel *); // 1
	PAD(void *, 7)
	float (*getLatency)(NetworkChannel *, int flow); // 9
	PAD(void *, 37)
	int (*sendDatagram)(NetworkChannel *, void *datagram); // 47
} NetworkChannelVMT;

struct NetworkChannel {
	NetworkChannelVMT *vmt;
	PAD(char, 36)
	int outSequenceNumber;
	int inSequenceNumber;
	int outSequenceNumberAcknowledge;
	int outReliableState;
	int inReliableState;
	int chokedPackets;
};

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
	NetworkChannel *(*getNetworkChannel)(Engine *); // 78
	PAD(void *, 3)
	bool (*isPlayingDemo)(Engine *); // 82
	PAD(void *, 9)
	bool (*isTakingScreenshot)(Engine *); // 92
	PAD(void *, 8)
	float (*getScreenAspectRatio)(Engine *, int w, int h); // 101
	PAD(void *, 6)
	void *(*executeClientCmd)(Engine *, const char *); // 108
	PAD(void *, 4)
	void *(*clientCmdUnrestricted)(Engine *, const char *, bool fromConsoleOrKeybind); // 113
	PAD(void *, 105)
	DemoPlaybackParameters *(*getDemoPlaybackParameters)(Engine *); // 219
	PAD(void *, 5)
	bool (*isVoiceRecording)(Engine *); // 225
} EngineVMT;

struct Engine {
	EngineVMT *vmt;
};

typedef struct {
	Vector start;
	PAD(char, 4)
	Vector delta;
	PAD(char, 44)
	bool isRay;
	bool isSwept;
} Ray;

typedef struct TraceFilter TraceFilter;

typedef struct {
	bool (*shouldHitEntity)(TraceFilter *, Entity *, int); // 0
	int (*getTraceType)(TraceFilter *); // 1
} TraceFilterVMT;

struct TraceFilter {
	TraceFilterVMT *vmt;
	Entity *skip;
};

typedef struct {
	const char *name;
	short surfaceProps;
	unsigned short flags;
} Surface;

typedef struct {
	Vector start;
	Vector end;
	PAD(char, 20)
	float fraction;
	int contents;
	unsigned short dispFlags;
	bool allSolid;
	bool startSolid;
	PAD(char, 4)
	Surface surface;
	int hitGroup;
	PAD(char, 4)
	Entity *entity;
	int hitbox;
} Trace;

typedef struct EngineTrace EngineTrace;

typedef struct {
	PAD(void *, 5)
	void (*traceRay)(EngineTrace *, const Ray *, unsigned int mask, const TraceFilter *, Trace *); // 5
} EngineTraceVMT;

struct EngineTrace {
	EngineTraceVMT *vmt;
};

typedef struct EntityList EntityList;

typedef struct {
	PAD(void *, 3)
	Entity *(*getEntity)(EntityList *, int i); // 3
	Entity *(*getEntityFromHandle)(EntityList *, int *handle); // 4
	PAD(void *, 1)
	int (*getHighestEntityIndex)(EntityList *); // 6
} EntityListVMT;

struct EntityList {
	EntityListVMT *vmt;
};

typedef struct InputSystem InputSystem;

typedef struct {
	PAD(void *, 15)
	bool (*isButtonDown)(InputSystem *, int buttonCode); // 15
	PAD(void *, 23)
	void (*resetInputState)(InputSystem *); // 39
	PAD(void *, 5)
	bool (*virtualKeyToButtonCode)(InputSystem *, int virtualKeyCode); // 45
} InputSystemVMT;

struct InputSystem {
	InputSystemVMT *vmt;
};

typedef struct MaterialVar MaterialVar;

typedef struct {
	PAD(void *, 4)
	void (*setValue)(MaterialVar *, float); // 4
	PAD(void *, 7)
	void (*setVectorValue)(MaterialVar *, float x, float y, float z); // 12
	PAD(void *, 13)
	void (*setVectorComponentValue)(MaterialVar *, float value, int component); // 26
} MaterialVarVMT;

struct MaterialVar {
	MaterialVarVMT *vmt;
};

typedef enum {
	NO_DRAW = 1 << 2,
	IGNOREZ = 1 << 15,
	WIREFRAME = 1 << 28
} MaterialVarFlag;

typedef struct Material Material;

typedef struct {
	const char *(*getName)(Material *); // 0
	const char *(*getTextureGroupName)(Material *); // 1
	PAD(void *, 9)
	MaterialVar *(*findVar)(Material *, const char *name, bool *found, bool complain); // 11
	PAD(void *, 15)
	void (*alphaModulate)(Material *, float alpha); // 27
	void (*colorModulate)(Material *, float r, float g, float b); // 28
	void (*setMaterialVarFlag)(Material *, MaterialVarFlag flag, bool on); // 29
	PAD(void *, 40)
	bool (*isPrecached)(Material *); // 70
} MaterialVMT;

struct Material {
	MaterialVMT *vmt;
};

typedef void *KeyValues;

typedef struct MaterialSystem MaterialSystem;

typedef struct {
	PAD(void *, 83)
	Material *(*createMaterial)(MaterialSystem *, const char *materialName, KeyValues *keyValues); // 83
	Material *(*findMaterial)(MaterialSystem *, const char *materialName, const char *textureGroupName, bool complain, const char *complainPrefix); // 84
	PAD(void *, 1)
	short (*firstMaterial)(MaterialSystem *); // 86
	short (*nextMaterial)(MaterialSystem *, short handle); // 87
	short (*invalidMaterial)(MaterialSystem *); // 88
	Material *(*getMaterial)(MaterialSystem *, short handle); // 89
} MaterialSystemVMT;

struct MaterialSystem {
	MaterialSystemVMT *vmt;
};

typedef struct ModelRender ModelRender;

typedef struct {
	PAD(void *, 21)
	void (*drawModelExecute)(ModelRender *, void *ctx, void *state, ModelRenderInfo *info, Matrix3x4 *customBoneToWorld); // 21
} ModelRenderVMT;

struct ModelRender {
	ModelRenderVMT *vmt;
};

typedef struct {
	const float realTime;
	int frameCount;
	const float absoluteFrameTime;
	PAD(char, 4)
	float currentTime;
	float frameTime;
	const int maxClients;
	const int tickCount;
	const float intervalPerTick;
} GlobalVars;

typedef struct {
	int nextFreeSlot;
	Entity *entity;
	ColorA colorA;
	PAD(char, 5)
	float alphaMax;
	PAD(char, 4)
	bool occluded;
	bool unoccluded;
	bool fullBloom;
	int fullBloomStencil;
	int style;
	int splitScreenSlot;
} GlowObject;

typedef struct {
	void *memory;
	int count;
	int growsize;
	int size;
	void *elements;
} UtlVector;

typedef struct {
	UtlVector objects;
	int firstFreeSlot;
} GlowObjectManager;

typedef void *MoveData;

typedef struct GameMovement GameMovement;

typedef struct {
	PAD(void *, 2)
	void (*processMovement)(GameMovement *, Entity *, MoveData *); // 2
	PAD(void *, 1)
	void (*startTrackPredictionErrors)(GameMovement *, Entity *); // 4
	void (*finishTrackPredictionErrors)(GameMovement *, Entity *); // 5
} GameMovementVMT;

struct GameMovement {
	GameMovementVMT *vmt;
};

typedef struct MoveHelper MoveHelper;

typedef struct {
	PAD(void *, 1)
	void (*setHost)(MoveHelper *, Entity *); // 1
} MoveHelperVMT;

struct MoveHelper {
	MoveHelperVMT *vmt;
};

typedef struct Prediction Prediction;

typedef struct {
	PAD(void *, 21)
	void (*setupMove)(Prediction *, Entity *, UserCmd *, MoveHelper *, MoveData *); // 21
	void (*finishMove)(Prediction *, Entity *, UserCmd *, MoveData *); // 22
} PredictionVMT;

struct Prediction {
	PredictionVMT *vmt;
};

typedef enum {
	OverrideType_Normal,
	OverrideType_BuildShadows,
	OverrideType_DepthWrite,
	OverrideType_CustomMaterial,
	OverrideType_SsaoDepthWrite
} OverrideType;

typedef struct StudioRender StudioRender;

typedef struct {
	PAD(void *, 33)
	void (*forcedMaterialOverride)(StudioRender *, Material *, OverrideType type, int i); // 33
} StudioRenderVMT;

struct StudioRender {
	StudioRenderVMT *vmt;
	PAD(char, 592)
	Material *materialOverride;
	PAD(char, 24)
	OverrideType overrideType;
};

float sdk_getServerTime(UserCmd *cmd);
Vector Vector_add(Vector a, Vector b);
Vector Vector_sub(Vector a, Vector b);
Vector Vector_div(Vector v, float f);
Vector Vector_toAngle(Vector v);
Vector Vector_normalize(Vector v);
Vector Vector_calculateAngle(Vector start, Vector end, Vector angle);
bool Vector_isNull(Vector v);
Vector Matrix3x4_origin(Matrix3x4 m);
Vector Entity_getBonePosition(Entity *entity, int bone);
bool Entity_canSee(Entity *entity, Entity *other, Vector pos);
bool GlowObjectManager_hasGlow(Entity *entity);
int GlowObjectManager_register(Entity *entity);
void GlowObjectManager_unregister(Entity *entity, int i);
void TraceFilter_init(TraceFilter *filter);
Color Color_fromHealth(int health);
ColorA ColorA_fromHealth(int health);
bool StudioRender_isForcedMaterialOverride(void);

NETVARS_DECL(moveType, MoveType)
NETVARS_DECL(simTime, float)
NETVARS_DECL(viewModel, int)
NETVARS_DECL(health, int)
NETVARS_DECL(flags, int)
NETVARS_DECL(spottedByMask, long)
NETVARS_DECL(flashDuration, float)
NETVARS_DECL(tickBase, int)
NETVARS_DECL(velocity, Vector)
NETVARS_DECL(isScoped, char)
NETVARS_DECL(immunity, char)
NETVARS_DECL(shotsFired, int)
NETVARS_DECL(waitForNoAttack, char)
NETVARS_DECL(nextAttack, float)
NETVARS_DECL(itemIndex, short)
NETVARS_DECL(aimPunchAngle, Vector)
NETVARS_DECL(viewPunchAngle, Vector)

#endif // SDK_H_
