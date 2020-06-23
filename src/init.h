#ifndef H_INIT
#define H_INIT

#include "include.h"
#include "utils.h"

#include "sdk/cdll.h"
#include "sdk/globalvarsbase.h"
#include "sdk/clientmode.h"
#include "sdk/baseclientstate.h"
#include "icvar.h"
#include "GarrysMod/Lua/Interface.h"

int (WINAPIV * __vsnprintf)(char *, size_t, const char*, va_list) = _vsnprintf;

namespace Interface
{
	IVEngineClient* EngineClient;
	INetChannel* NetChannel;
	IBaseClientDLL* BaseClientDLL;
	IClientMode* ClientMode;
	CBaseClientState* ClientState;
	ICvar* CVar;

	CHLClient* HLClient;
	CGlobalVarsBase* GlobalVars;
	GarrysMod::Lua::ILuaBase* LuaState;

	void Link()
	{
		//-----------------------------------------------------------------------------
		// Interfaces
		//-----------------------------------------------------------------------------
		EngineClient = Utils::GetInterface<IVEngineClient>("engine.dll", "VEngineClient013");
		BaseClientDLL = Utils::GetInterface<IBaseClientDLL>("client.dll", "VClient017");
		CVar = Utils::GetInterface<ICvar>("vstdlib.dll", "VEngineCvar004");

		//-----------------------------------------------------------------------------
		// Derived Interfaces/Classes | Hacky ways to get the pointer to
		//-----------------------------------------------------------------------------
		HLClient = reinterpret_cast<CHLClient*>(BaseClientDLL);
		NetChannel = reinterpret_cast<INetChannel*>(EngineClient->GetNetChannelInfo());
		GlobalVars = **(CGlobalVarsBase***)((*(DWORD**)BaseClientDLL)[0] + 0x55);
		ClientMode = **(IClientMode***)((*(DWORD**)BaseClientDLL)[10] + 0x5);
		ClientState = *(CBaseClientState**)((*(DWORD**)EngineClient)[84] + 1);
	}
}

#define TICK_INTERVAL			(Interface::GlobalVars->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )
#define ROUND_TO_TICKS( t )		( TICK_INTERVAL * TIME_TO_TICKS( t ) )
#define TICK_NEVER_THINK		(-1)

#define Bits2Bytes(b) ((b+7)>>3)

#define IN_ATTACK		(1 << 0)
#define IN_JUMP			(1 << 1)
#define IN_DUCK			(1 << 2)
#define IN_FORWARD		(1 << 3)
#define IN_BACK			(1 << 4)
#define IN_USE			(1 << 5)
#define IN_CANCEL		(1 << 6)
#define IN_LEFT			(1 << 7)
#define IN_RIGHT		(1 << 8)
#define IN_MOVELEFT		(1 << 9)
#define IN_MOVERIGHT	(1 << 10)
#define IN_ATTACK2		(1 << 11)
#define IN_RUN			(1 << 12)
#define IN_RELOAD		(1 << 13)
#define IN_ALT1			(1 << 14)
#define IN_ALT2			(1 << 15)
#define IN_SCORE		(1 << 16)   // Used by client.dll for when scoreboard is held down
#define IN_SPEED		(1 << 17)	// Player is holding the speed key
#define IN_WALK			(1 << 18)	// Player holding walk key
#define IN_ZOOM			(1 << 19)	// Zoom key for HUD zoom
#define IN_WEAPON1		(1 << 20)	// weapon defines these bits
#define IN_WEAPON2		(1 << 21)	// weapon defines these bits
#define IN_BULLRUSH		(1 << 22)
#define IN_GRENADE1		(1 << 23)	// grenade 1
#define IN_GRENADE2		(1 << 24)	// grenade 2
#define	IN_ATTACK3		(1 << 25)

#define NET_MESSAGE_BITS 6
#define MAX_USERCMD_LOSS 10

#endif