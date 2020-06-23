#ifndef H_SDK_NETCHAN
#define H_SDK_NETCHAN

#include "tier0/platform.h"
#include "tier1/bitbuf.h"
#include "tier1/netadr.h"

#include "tier1/utlvector.h"
#include "tier1/utlbuffer.h"

#define FLOW_OUTGOING	0		
#define FLOW_INCOMING	1
#define MAX_FLOWS		2

#define NET_MAX_DATAGRAM_PAYLOAD 1400

#define MAX_STREAMS 2
#define MAX_RATE 20000
#define MIN_RATE 1000
#define DEFAULT_RATE 10000
#define NET_FRAMES_BACKUP 64

#define MAX_FRAGMENT_SIZE 256

#define MAX_SUBCHANNELS 8

#define FRAG_NORMAL_STREAM 0
#define FRAG_FILE_STREAM 1

#define MAX_OSPATH 260

#define MAX_FILE_SIZE_BITS	26
#define MAX_FILE_SIZE		((1<<MAX_FILE_SIZE_BITS)-1)

#define NET_MAX_PAYLOAD 4000
#define NET_MAX_MESSAGE 4096

#define PACKET_FLAG_RELIABLE (1<<0)	// packet contains subchannel stream data
#define PACKET_FLAG_COMPRESSED (1<<1)	// packet is compressed
#define PACKET_FLAG_ENCRYPTED (1<<2)  // packet is encrypted
#define PACKET_FLAG_SPLIT (1<<3)  // packet is split
#define PACKET_FLAG_CHOKED (1<<4)  // packet was choked by sender
#define PACKET_FLAG_CHALLENGE (1<<5) // packet is a challenge
#define PACKET_FLAG_TABLES (1<<10) //custom flag, request string tables

#define MIN_ROUTABLE_PAYLOAD 16

class IDemoRecorder;
class INetChannel;
class RequestFile_t;

typedef struct subchannel_s
{
	int32_t frag_ofs[MAX_STREAMS];	// offset of fragments to send (0x00, 0x04)
	int32_t frag_num[MAX_STREAMS];	// amount of fragments to send (0x08, 0x0C)
	int32_t sequence;				// outgoing seq. at time of sending (0x10)
	int32_t state;					// 0 = idle, 1 = needs sending, 2 = ready?, 3 = ??? (0x14)
	int32_t index;					// channel index (0-7) (0x18)
} subchannel_t;

typedef struct netpacket_s
{
	netadr_t from;				// sender IP (0x00)
	int32_t source;				// received source (0x0C)
	double received;			// received time (0x10)
	uint8_t *data;				// pointer to raw packet data (0x18)
	bf_read message;			// easy bitbuf data access (0x1C/28)
	int32_t size;				// size in bytes (0x40/64)
	int32_t wiresize;			// size in bytes before decompression (0x44/68)
	bool stream;				// was send as stream (0x48/72)
	struct netpacket_s *pNext;	// for internal use, should be NULL in public (0x4C/76)
} netpacket_t;

enum ConnectionStatus_t
{
	CONNECTION_STATE_DISCONNECTED = 0,
	CONNECTION_STATE_CONNECTING,
	CONNECTION_STATE_CONNECTION_FAILED,
	CONNECTION_STATE_CONNECTED,
};

typedef enum
{
	NS_CLIENT,
	NS_SERVER
} netsrc_t;

//-----------------------------------------------------------------------------
// INetworkMessageHandler
//-----------------------------------------------------------------------------
abstract_class INetworkMessageHandler
{
public:
	virtual void	OnConnectionClosing(INetChannel *channel, char const *reason) = 0;
	virtual void	OnConnectionStarted(INetChannel *channel) = 0;

	virtual void	OnPacketStarted(int inseq, int outseq) = 0;
	virtual void	OnPacketFinished() = 0;

protected:
	virtual ~INetworkMessageHandler() {}
};

//-----------------------------------------------------------------------------
// CUDPSocket
//-----------------------------------------------------------------------------
class CUDPSocket
{
public:
	class CImpl;
	CImpl				*m_pImpl;

	netadr_t			m_socketIP;
	unsigned int		m_Socket;
	unsigned short		m_Port;
};

//-----------------------------------------------------------------------------
// INetChannelHandler
//-----------------------------------------------------------------------------
class INetChannelHandler
{
public:
	virtual	~INetChannelHandler(void) {};

	virtual void ConnectionStart(INetChannel *chan) = 0;	// called first time network channel is established

	virtual void ConnectionClosing(const char *reason) = 0; // network channel is being closed by remote site

	virtual void ConnectionCrashed(const char *reason) = 0; // network error occured

	virtual void PacketStart(int incoming_sequence, int outgoing_acknowledged) = 0;	// called each time a new packet arrived

	virtual void PacketEnd(void) = 0; // all messages has been parsed

	virtual void FileRequested(const char *fileName, unsigned int transferID) = 0; // other side request a file for download

	virtual void FileReceived(const char *fileName, unsigned int transferID) = 0; // we received a file

	virtual void FileDenied(const char *fileName, unsigned int transferID) = 0;	// a file request was denied by other side

	virtual void FileSent(const char *fileName, unsigned int transferID) = 0;	// we sent a file
};

class INetChannel;

//-----------------------------------------------------------------------------
// INetMessage
//-----------------------------------------------------------------------------
class INetMessage
{
public:
	virtual	~INetMessage() {};

	// Use these to setup who can hear whose voice.
	// Pass in client indices (which are their ent indices - 1).

	virtual void	SetNetChannel(INetChannel * netchan) = 0; // netchannel this message is from/for
	virtual void	SetReliable(bool state) = 0;	// set to true if it's a reliable message

	virtual bool	Process(void) = 0; // calles the recently set handler to process this message

	virtual	bool	ReadFromBuffer(bf_read &buffer) = 0; // returns true if parsing was OK
	virtual	bool	WriteToBuffer(bf_write &buffer) = 0;	// returns true if writing was OK

	virtual bool	IsReliable(void) const = 0;  // true, if message needs reliable handling

	virtual int				GetType(void) const = 0; // returns module specific header tag eg svc_serverinfo
	virtual int				GetGroup(void) const = 0;	// returns net message group of this message
	virtual const char		*GetName(void) const = 0;	// returns network message name, eg "svc_serverinfo"
	virtual INetChannel		*GetNetChannel(void) const = 0;
	virtual const char		*ToString(void) const = 0; // returns a human readable string about message content
};

//-----------------------------------------------------------------------------
// INetChannelInfo
//-----------------------------------------------------------------------------
class INetChannelInfo
{
public:

	enum {
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
	};

	virtual const char  *GetName(void) const = 0;	// get channel name
	virtual const char  *GetAddress(void) const = 0; // get channel IP address as string
	virtual float		GetTime(void) const = 0;	// current net time
	virtual float		GetTimeConnected(void) const = 0;	// get connection time in seconds
	virtual int			GetBufferSize(void) const = 0;	// netchannel packet history size
	virtual int			GetDataRate(void) const = 0; // send data rate in byte/sec

	virtual bool		IsLoopback(void) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut(void) const = 0;	// true if timing out
	virtual bool		IsPlayback(void) const = 0;	// true if demo playback

	virtual float		GetLatency(int flow) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency(int flow) const = 0; // average packet latency in seconds
	virtual float		GetAvgLoss(int flow) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAvgChoke(int flow) const = 0;	 // avg packet choke[0..1]
	virtual float		GetAvgData(int flow) const = 0;	 // data flow in bytes/sec
	virtual float		GetAvgPackets(int flow) const = 0; // avg packets/sec
	virtual int			GetTotalData(int flow) const = 0;	 // total flow in/out in bytes
	virtual int			GetSequenceNr(int flow) const = 0;	// last send seq number
	virtual bool		IsValidPacket(int flow, int frame_number) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime(int flow, int frame_number) const = 0; // time when packet was send
	virtual int			GetPacketBytes(int flow, int frame_number, int group) const = 0; // group size of this packet
	virtual bool		GetStreamProgress(int flow, int *received, int *total) const = 0;  // TCP progress if transmitting
	virtual float		GetTimeSinceLastReceived(void) const = 0;	// get time since last recieved packet in seconds
	virtual	float		GetCommandInterpolationAmount(int flow, int frame_number) const = 0;
	virtual void		GetPacketResponseLatency(int flow, int frame_number, int *pnLatencyMsecs, int *pnChoke) const = 0;
	virtual void		GetRemoteFramerate(float *pflFrameTime, float *pflFrameTimeStdDeviation) const = 0;

	virtual float		GetTimeoutSeconds() const = 0;
};

typedef void* FileHandle_t;

typedef struct dataFragments_s
{
	FileHandle_t	file;			// open file handle
	char			filename[MAX_OSPATH]; // filename
	char*			buffer;			// if NULL it's a file
	unsigned int	bytes;			// size in bytes
	unsigned int	bits;			// size in bits
	unsigned int	transferID;		// only for files
	bool			isCompressed;	// true if data is bzip compressed
	unsigned int	nUncompressedSize; // full size in bytes
	bool			asTCP;			// send as TCP stream
	int				numFragments;	// number of total fragments
	int				ackedFragments; // number of fragments send & acknowledged
	int				pendingFragments; // number of fragments send, but not acknowledged yet
} dataFragments_t;

struct lzss_header_t
{
	unsigned int	id;
	unsigned int	actualSize;	// always little endian
};

#define FRAG_NORMAL_STREAM 0

#define FILESYSTEM_INVALID_HANDLE	( FileHandle_t )0

#define FRAGMENT_BITS		8
#define FRAGMENT_SIZE		(1<<FRAGMENT_BITS)

#define BYTES2FRAGMENTS(i) ((i+FRAGMENT_SIZE-1)/FRAGMENT_SIZE)

#define LZSS_ID (('S'<<24)|('S'<<16)|('Z'<<8)|('L'))

//-----------------------------------------------------------------------------
// INetChannel
//-----------------------------------------------------------------------------
abstract_class INetChannel : public INetChannelInfo
{
public:
	virtual	~INetChannel(void) {};

	virtual void	SetDataRate(float rate) = 0;
	virtual bool	RegisterMessage(INetMessage *msg) = 0;
	virtual bool	StartStreaming(unsigned int challengeNr) = 0;
	virtual void	ResetStreaming(void) = 0;
	virtual void	SetTimeout(float seconds) = 0;
	virtual void	SetDemoRecorder(IDemoRecorder *recorder) = 0;
	virtual void	SetChallengeNr(unsigned int chnr) = 0;

	virtual void	Reset(void) = 0;
	virtual void	Clear(void) = 0;
	virtual void	Shutdown(const char *reason) = 0;

	virtual void	ProcessPlayback(void) = 0;
	virtual bool	ProcessStream(void) = 0;
	virtual void	ProcessPacket(struct netpacket_s* packet, bool bHasHeader) = 0;

	virtual bool	SendNetMsg(INetMessage &msg, bool bForceReliable = false, bool bVoice = false) = 0;
#ifdef POSIX
	FORCEINLINE bool SendNetMsg(INetMessage const &msg, bool bForceReliable = false, bool bVoice = false) { return SendNetMsg(*((INetMessage *)&msg), bForceReliable, bVoice); }
#endif
	virtual bool	SendData(bf_write &msg, bool bReliable = true) = 0;
	virtual bool	SendFile(const char *filename, unsigned int transferID) = 0;
	virtual void	DenyFile(const char *filename, unsigned int transferID) = 0;
	virtual void	RequestFile_OLD(const char *filename, unsigned int transferID) = 0;	// get rid of this function when we version the 
	virtual void	SetChoked(void) = 0;
	virtual int		SendDatagram(bf_write *data) = 0;
	virtual bool	Transmit(bool onlyReliable = false) = 0;

	virtual const netadr_t	&GetRemoteAddress(void) const = 0;
	virtual INetChannelHandler *GetMsgHandler(void) const = 0;
	virtual int				GetDropNumber(void) const = 0;
	virtual int				GetSocket(void) const = 0;
	virtual unsigned int	GetChallengeNr(void) const = 0;
	virtual void			GetSequenceData(int &nOutSequenceNr, int &nInSequenceNr, int &nOutSequenceNrAck) = 0;
	virtual void			SetSequenceData(int nOutSequenceNr, int nInSequenceNr, int nOutSequenceNrAck) = 0;

	virtual void	UpdateMessageStats(int msggroup, int bits) = 0;
	virtual bool	CanPacket(void) const = 0;
	virtual bool	IsOverflowed(void) const = 0;
	virtual bool	IsTimedOut(void) const = 0;
	virtual bool	HasPendingReliableData(void) = 0;

	virtual void	SetFileTransmissionMode(bool bBackgroundMode) = 0;
	virtual void	SetCompressionMode(bool bUseCompression) = 0;
	virtual unsigned int RequestFile(const char *filename) = 0;
	virtual float	GetTimeSinceLastReceived(void) const = 0;	// get time since last received packet in seconds

	virtual void	SetMaxBufferSize(bool bReliable, int nBytes, bool bVoice = false) = 0;

	virtual bool	IsNull() const = 0;
	virtual int		GetNumBitsWritten(bool bReliable) = 0;
	virtual void	SetInterpolationAmount(float flInterpolationAmount) = 0;
	virtual void	SetRemoteFramerate(float flFrameTime, float flFrameTimeStdDeviation) = 0;

	// Max # of payload bytes before we must split/fragment the packet
	virtual void	SetMaxRoutablePayloadSize(int nSplitSize) = 0;
	virtual int		GetMaxRoutablePayloadSize() = 0;

	virtual int		GetProtocolVersion() = 0;
public:
	ConnectionStatus_t m_ConnectionState;

	// last send outgoing sequence number
	int			m_nOutSequenceNr;
	// last received incoming sequnec number
	int			m_nInSequenceNr;
	// last received acknowledge outgoing sequnce number
	int			m_nOutSequenceNrAck;

	// state of outgoing reliable data (0/1) flip flop used for loss detection
	int			m_nOutReliableState;
	// state of incoming reliable data
	int			m_nInReliableState;

	int			m_nChokedPackets;	//number of choked packets
	int			m_PacketDrop;

	// Reliable data buffer, send wich each packet (or put in waiting list)
	bf_write	m_StreamReliable;
	byte		m_ReliableDataBuffer[8 * 1024];	// In SP, we don't need much reliable buffer, so save the memory (this is mostly for xbox).
	CUtlVector<byte> m_ReliableDataBufferMP;

	// unreliable message buffer, cleared wich each packet
	bf_write	m_StreamUnreliable;
	byte		m_UnreliableDataBuffer[NET_MAX_DATAGRAM_PAYLOAD];

	// don't use any vars below this (only in net_ws.cpp)

	CUDPSocket	*m_pSocket;   // NS_SERVER or NS_CLIENT index, depending on channel.
	int			m_StreamSocket;	// TCP socket handle

	unsigned int m_MaxReliablePayloadSize;	// max size of reliable payload in a single packet	

											// Address this channel is talking to.
	netadr_t	remote_address;

	// For timeouts.  Time last message was received.
	float		last_received;
	// Time when channel was connected.
	float      connect_time;

	// Bandwidth choke
	// Bytes per second
	int			m_Rate;
	// If realtime > cleartime, free to send next packet
	float		m_fClearTime;

	float		m_Timeout;		// in seconds 

	char			m_Name[32];		// channel name
};

struct subChannel_s
{
	int				startFraggment[MAX_STREAMS];
	int				numFragments[MAX_STREAMS];
	int				sendSeqNr;
	int				state; // 0 = free, 1 = scheduled to send, 2 = send & waiting, 3 = dirty
	int				index; // index in m_SubChannels[]
};

typedef struct netframe_s
{
	// Data received from server
	float			time;			// net_time received/send
	int				size;			// total size in bytes
	float			latency;		// raw ping for this packet, not cleaned. set when acknowledged otherwise -1.
	float			avg_latency;	// averaged ping for this packet
	bool			valid;			// false if dropped, lost, flushed
	int				choked;			// number of previously chocked packets
	int				dropped;
	float			m_flInterpolationAmount;
	unsigned short	msggroups[INetChannelInfo::TOTAL];	// received bytes for each message group
} netframe_t;

typedef struct
{
	float		nextcompute;	// Time when we should recompute k/sec data
	float		avgbytespersec;	// average bytes/sec
	float		avgpacketspersec;// average packets/sec
	float		avgloss;		// average packet loss [0..1]
	float		avgchoke;		// average packet choke [0..1]
	float		avglatency;		// average ping, not cleaned
	float		latency;		// current ping, more accurate also more jittering
	int			totalpackets;	// total processed packets
	int			totalbytes;		// total processed bytes
	int			currentindex;		// current frame index
	netframe_t	frames[NET_FRAMES_BACKUP]; // frame history
	netframe_t	*currentframe;	// current frame
} netflow_t;

class CNetChan : public INetChannel
{
};

#endif