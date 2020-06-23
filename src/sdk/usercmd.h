//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#ifndef H_SDK_CUSERCMD
#define H_SDK_CUSERCMD

#include "mathlib/vector.h"
#include "imovehelper.h"
#include "tier1/checksum_crc.h"
#include "tier1/utlflags.h"

class CUserCmd
{
public:
	int		command_number;

	// the tick the client created this command
	int		tick_count;

	// Player instantaneous view angles.
	QAngle	viewangles;
	// Intended velocities
	//	forward velocity.
	float	forwardmove;
	//  sideways velocity.
	float	sidemove;
	//  upward velocity.
	float	upmove;
	// Attack button states
	CUtlFlags <int>		buttons;
	// Impulse command issued.
	byte    impulse;
	// Current weapon id
	int		weaponselect;
	int		weaponsubtype;

	int		random_seed;	// For shared random functions

	short	mousedx;		// mouse accum in x from create move
	short	mousedy;		// mouse accum in y from create move

							// Client only, tracks whether we've predicted this command at least once
	bool	hasbeenpredicted;
};

#endif