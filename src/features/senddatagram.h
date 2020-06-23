#ifndef H_FEATURES_SENDDATAGRAM
#define H_FEATURES_SENDDATAGRAM

#include "../init.h"

typedef int(__thiscall* SendDatagramFn)(INetChannel* thisptr, bf_write *data);
SendDatagramFn OriginalSendDatagram;

int __fastcall HookSendDatagram(INetChannel* thisptr, int edx, bf_write *data)
{
	return OriginalSendDatagram(thisptr, data);
}

#endif