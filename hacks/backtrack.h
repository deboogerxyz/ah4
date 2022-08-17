#include "../sdk.h"

typedef struct {
	Entity *entity;
	Matrix3x4 boneMatrix[256];
	float simTime;
} Record;

void backtrack_update(FrameStage stage);
Record *backtrack_getClosestRecord(UserCmd *cmd);
void backtrack_run(UserCmd *cmd);
void backtrack_addLatency(NetworkChannel *);
void backtrack_updateSequences(NetworkChannel *);
void backtrack_init(void);
void backtrack_cleanUp(void);
