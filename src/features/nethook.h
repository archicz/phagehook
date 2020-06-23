#ifndef H_FEATURES_NETHOOK
#define H_FEATURES_NETHOOK

#include "../init.h"
#include "tier1/checksum_crc.h"
#include "voicecodec.h"

typedef bool(__thiscall* FnProcessMessages)(void* thisptr, bf_read& buf);
FnProcessMessages OrigProcessMessages;

static INetMessage* FindMessage(void* thisptr, int cmd)
{
	int numtypes = *((int*)thisptr + 2312);

	for (int i = 0; i <= numtypes; i++)
	{
		INetMessage* msg = *(INetMessage **)(*((DWORD *)thisptr + 2309) + 4 * i);
		if (msg->GetType() == cmd)
		{
			return msg;
		}
	}

	return NULL;
}

static bool ProcessControlMessage(int cmd, bf_read& buf)
{
	char string[1024];

	if (cmd == net_NOP)
	{
		return true;
	}

	if (cmd == net_Disconnect)
	{
		buf.ReadString(string, sizeof(string));
		return true;
	}

	if (cmd == net_File)
	{
		unsigned int transferID = buf.ReadUBitLong(32);

		buf.ReadString(string, sizeof(string));
		int onebit = buf.ReadOneBit();

		return true;
	}

	return false;
}

bool __fastcall HookProcessMessages(void* thisptr, int edx, bf_read& buf)
{
	bf_read bufcpy(buf);

	while (true)
	{
		if (buf.GetNumBitsLeft() < NET_MESSAGE_BITS)
		{
			break;
		}

		unsigned char cmd = buf.ReadUBitLong(NET_MESSAGE_BITS);

		if (cmd <= net_File)
		{
			if (!ProcessControlMessage(cmd, buf))
			{
				Msg("Unknown control message.\n");
			}

			continue;
		}

		INetMessage* netmsg = FindMessage(thisptr, cmd);

		if (netmsg)
		{
			const char *msgname = netmsg->GetName();

			if (!netmsg->ReadFromBuffer(buf))
			{
				Msg("Failed to read message %s\n", msgname);
			}
		}
	}

	return OrigProcessMessages(thisptr, bufcpy);
}

#endif