#ifndef H_FEATURES_LUA_IMP
#define H_FEATURES_LUA_IMP

#include "../init.h"
#include "voicecodec.h"

using namespace GarrysMod::Lua;

LUA_FUNCTION(ChangeName)
{
	const char* szName = LUA->GetString(1);
	char pckBuf[64];

	bf_write pck(pckBuf, 64);
	pck.WriteUBitLong(5, NET_MESSAGE_BITS);
	pck.WriteByte(1);
	pck.WriteString("name");
	pck.WriteString(szName);

	Interface::NetChannel->SendData(pck, true);
	return 1;
}

static bool finished = false;
static int16 curFrame[FRAME_SIZE];
static FILE* curFile = nullptr;

enum PayLoadType : uint8
{
	PLT_OPUS_PLC = 6,
	PLT_SamplingRate = 11
};

void CopySampleData()
{
	if (finished)
	{
		return;
	}

	if (!curFile)
	{
		curFile = fopen("music.pcm", "rb");
	}

	fread(curFrame, sizeof(int16), FRAME_SIZE / sizeof(int16), curFile);

	if (feof(curFile))
	{
		finished = true;
	}
}

uint16 EncodeVoicePayload(const char* pUncompressedBytes, uint32 nSamples, char* pCompressed, uint32 maxCompressedBytes, bool bFinal)
{
	char* cursor = (char*)pCompressed;
	
	*(uint32 *)cursor = 0x00000011; // steamid (low part)
	cursor += 4;

	*(uint32 *)cursor = 0x01100001; // steamid (high part)
	cursor += 4;

	{// SampleRate opcode
		*cursor = PLT_SamplingRate;
		++cursor;

		*(uint16*)cursor = opuscodec->m_samplerate;
		cursor += sizeof(uint16);
	}

	{// Opus voice opcode & payload
		*cursor = PLT_OPUS_PLC;
		++cursor;

		uint16 bytescompressed = opuscodec->Compress(pUncompressedBytes, nSamples, cursor + 2, maxCompressedBytes - (1 + 2 + 1 + 2), bFinal);

		*(uint16*)cursor = bytescompressed;
		cursor += sizeof(uint16);

		cursor += bytescompressed;
	}

	{// crc32 checksum at the end
		uint32 checksum = CRC32_ProcessSingleBuffer(pCompressed, cursor - pCompressed);

		*(uint32*)cursor = checksum;
		cursor += sizeof(uint32);
	}

	return cursor - pCompressed;
}

void NormalizeSamples(char* data, uint32 samples)
{
	int16* pcm = (int16*)data;

	for (uint32 i = 0; i < samples; i++)
	{
		int16 sample = pcm[i];
		pcm[i] = sample;
	}
}

static bool load = false;

class StrojarAudioSource
{
public:
	char* m_data = 0;
	double m_starttime = 0;
	uint32 m_curbyte = 0;
	uint32 m_size = 0;
public:
	void LoadFromFile(const char* filename)
	{
		FILE* fin = fopen(filename, "rb");
		fseek(fin, 0, SEEK_END);
		m_size = ftell(fin);
		fseek(fin, 0, SEEK_SET);

		m_data = new char[m_size];
		fread(m_data, sizeof(char), m_size, fin);

		m_starttime = Plat_FloatTime();
		m_curbyte = 0;
	}

	uint32 SamplePlay(char* dest, uint32 destSize)
	{
		double curtime = Plat_FloatTime();
		uint32 nShouldGet = uint32((curtime - m_starttime) * opuscodec->m_samplerate);

		if (nShouldGet > 0)
		{
			uint32 gotten = min(destSize / BYTES_PER_SAMPLE, min(nShouldGet, (m_size - m_curbyte) / BYTES_PER_SAMPLE));

			if (gotten > 0)
			{
				memcpy(dest, &m_data[m_curbyte], gotten * BYTES_PER_SAMPLE);
				m_curbyte += gotten * BYTES_PER_SAMPLE;
				m_starttime = curtime;

				return gotten;
			}

			return 0;
		}

		return 0;
	}
};

static StrojarAudioSource* music = 0;

LUA_FUNCTION(VoiceMusic)
{
	/*
	if (!load)
	{
		music = new StrojarAudioSource;
		music->LoadFromFile("music.pcm");
		
		load = true;
	}

	char tempData[8192];
	uint32 nSamples = music->SamplePlay(tempData, sizeof(tempData));

	if (nSamples != 0)
	{
		NormalizeSamples(tempData, nSamples);

		char compressed[8192];
		uint16 nCompressed = EncodeVoicePayload(tempData, nSamples, compressed, sizeof(compressed), false);

		char msg[16384];

		bf_write pck(msg, sizeof(msg));
		pck.WriteUBitLong(clc_VoiceData, NET_MESSAGE_BITS);
		pck.WriteWord(nCompressed * 8);
		pck.WriteBits(compressed, nCompressed * 8);

		Interface::NetChannel->SendData(pck, false);
	}
	*/

	char specialString[32];
	specialString[0] = 0x07;
	specialString[1] = 0xFF;
	specialString[2] = 0x0;

	char pckBuf[64];
	bf_write pck(pckBuf, 64);
	pck.WriteUBitLong(net_File, NET_MESSAGE_BITS);
	pck.WriteUBitLong(0, 32);
	pck.WriteString(specialString);
	pck.WriteOneBit(1);

	Interface::NetChannel->SendData(pck, false);
	return 1;
}

LUA_FUNCTION(NeverKick)
{
	Interface::NetChannel->SetTimeout(3600.0f);
	return 1;
}

#endif