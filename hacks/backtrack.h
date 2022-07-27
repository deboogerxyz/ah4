#include "../sdk.h"

void backtrack_update(FrameStage stage);
void backtrack_run(UserCmd *cmd);
void backtrack_addLatency(NetworkChannel *);
void backtrack_updateSequences(NetworkChannel *);
void backtrack_init(void);
void backtrack_cleanUp(void);