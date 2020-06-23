#ifndef OPUSVOICECODEC_H
#define OPUSVOICECODEC_H
#pragma once

#include "tier1/utlbuffer.h"
#include "opus.h"

const uint32 BYTES_PER_SAMPLE = 2;
const uint32 MAX_CHANNELS = 1;
const uint32 FRAME_SIZE = 480;
const uint32 MAX_FRAME_SIZE = 3 * FRAME_SIZE;
const uint32 MAX_PACKET_LOSS = 10;

class VoiceEncoder_Opus
{
public:
	OpusEncoder* m_pEncoder;
	OpusDecoder* m_pDecoder;
	CUtlBuffer m_bufOverflowBytes;

	int m_samplerate;
	int m_bitrate;

	unsigned short m_nCurFrame;
	unsigned short m_nLastFrame;

	bool m_PacketLossConcealment;

public:
	VoiceEncoder_Opus();

	virtual ~VoiceEncoder_Opus();

	bool Init(int quality);
	void Release();
	bool ResetState();
	int Compress(const char *pUncompressedBytes, int nSamples, char *pCompressed, int maxCompressedBytes, bool bFinal);
	int Decompress(const char *pCompressed, int compressedBytes, char *pUncompressed, int maxUncompressedBytes);

	int GetNumQueuedEncodingSamples() const { return m_bufOverflowBytes.TellPut() / BYTES_PER_SAMPLE; }
};

extern VoiceEncoder_Opus* opuscodec;

#endif