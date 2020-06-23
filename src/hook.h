#ifndef H_HOOK
#define H_HOOK

#include "init.h"
#include "vmt.h"
#include "detour/detours.h"

#include "features/createmove.h"
#include "features/senddatagram.h"
#include "features/framestagenotify.h"
#include "features/luaimp.h"
#include "features/nethook.h"
#include "features/voicecodec.h"

namespace Hook
{
	VMTHook* ClientMode;
	VMTHook* HLClient;
	VMTHook* NetChannel;

	void Link()
	{
		ClientMode = new VMTHook(Interface::ClientMode);
		HLClient = new VMTHook(Interface::HLClient);
		NetChannel = new VMTHook(reinterpret_cast<INetChannel*>(Interface::EngineClient->GetNetChannelInfo()));
	}

	void Init()
	{
		OrigProcessMessages = (FnProcessMessages)Utils::PatternScan("engine.dll", "55 8B EC 83 EC 2C F7 05 ? ? ? ? ? ? ? ? 53 56 57 8B D9 BF ? ? ? ? 74 07 BE ? ? ? ? EB 0F");

		opuscodec->Init(0);
	}

	void MakeHook()
	{
		//-----------------------------------------------------------------------------
		// VTable hooks
		//-----------------------------------------------------------------------------
		OriginalCreateMove = (CreateMoveFn)ClientMode->hookFunction(21, HookCreateMove);
		OriginalSendDatagram = (SendDatagramFn)NetChannel->hookFunction(46, HookSendDatagram);
		OriginalFrameStageNotify = (FrameStageNotifyFn)HLClient->hookFunction(35, HookFrameStageNotify);

		/*DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)OrigProcessMessages, HookProcessMessages);
		DetourTransactionCommit();*/
		
		//-----------------------------------------------------------------------------
		// LUA: Netchannel bs
		//-----------------------------------------------------------------------------
		Interface::LuaState->PushSpecial(SPECIAL_GLOB);
		Interface::LuaState->PushString("ChangeName");
		Interface::LuaState->PushCFunction(ChangeName);
		Interface::LuaState->SetTable(-3);

		Interface::LuaState->PushSpecial(SPECIAL_GLOB);
		Interface::LuaState->PushString("VoiceMusic");
		Interface::LuaState->PushCFunction(VoiceMusic);
		Interface::LuaState->SetTable(-3);
		
		Interface::LuaState->PushSpecial(SPECIAL_GLOB);
		Interface::LuaState->PushString("NeverKick");
		Interface::LuaState->PushCFunction(NeverKick);
		Interface::LuaState->SetTable(-3);
	}

	void CleanUp()
	{
		ClientMode->unhookFunction(21);
		NetChannel->unhookFunction(46);
		HLClient->unhookFunction(35);

		/*DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)OrigProcessMessages, HookProcessMessages);
		DetourTransactionCommit();*/
	}
}

#endif