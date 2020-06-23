#ifndef H_FEATURES_FRAMESTAGENOTIFY
#define H_FEATURES_FRAMESTAGENOTIFY

#include "../init.h"
#include "luaimp.h"

typedef void(__thiscall* FrameStageNotifyFn)(CHLClient*, ClientFrameStage_t);
FrameStageNotifyFn OriginalFrameStageNotify = nullptr;

void __fastcall HookFrameStageNotify(CHLClient* thisptr, int edx, ClientFrameStage_t stage)
{
	OriginalFrameStageNotify(thisptr, stage);
}

#endif