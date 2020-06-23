#define GMMODULE

#include "include.h"
#include "init.h"
#include "hook.h"

#include "GarrysMod/Lua/Interface.h"

using namespace GarrysMod::Lua;

GMOD_MODULE_OPEN()
{
	Interface::LuaState = LUA;
	Interface::Link();

	Hook::Link();
	Hook::Init();
	Hook::MakeHook();

	return 0;
}

GMOD_MODULE_CLOSE()
{
	Hook::CleanUp();

	return 0;
}