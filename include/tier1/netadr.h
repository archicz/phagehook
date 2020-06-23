//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
// netadr.h
#ifndef NETADR_H
#define NETADR_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#undef SetPort

typedef enum
{
	NA_NULL = 0,
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
} netadrtype_t;

typedef struct netadr_s
{
	netadrtype_t	type;
	unsigned char	ip[4];
	unsigned short	port;
} netadr_t;

#endif // NETADR_H
