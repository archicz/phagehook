#ifndef H_FEATURES_CREATEMOVE
#define H_FEATURES_CREATEMOVE

#include "../init.h"

typedef bool(__thiscall* CreateMoveFn)(IClientMode* thisptr, float flInputSampleTime, CUserCmd* cmd);
CreateMoveFn OriginalCreateMove;

bool __fastcall HookCreateMove(IClientMode* thisptr, void* edx, float flInputSampleTime, CUserCmd* cmd)
{
	//-----------------------------------------------------------------------------
	// bSendPacket
	//-----------------------------------------------------------------------------

	DWORD dwAddr;
	__asm
	{
		mov dwAddr, ebp
	}
	bool& bSendPacket = *(***(bool****)(dwAddr)-0x1);

	Interface::LuaState->PushSpecial(0);
	Interface::LuaState->GetField(-1, "bSendPacket");
	bSendPacket = Interface::LuaState->GetBool(-1);
	Interface::LuaState->Pop(2);

	return OriginalCreateMove(thisptr, flInputSampleTime, cmd);
};

#endif