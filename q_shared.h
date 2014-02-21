// Copyright (C) 2001-2002 Raven Software.
//
#ifndef __Q_SHARED_H
#define __Q_SHARED_H

#include "version.h"

// q_shared.h -- included first by ALL program modules.
// A user mod should never modify this file
#if (defined _MSC_VER)
#pragma warning(disable : 4244)     // double to float loss of data
#pragma warning(disable : 4224)		// string name parameter defined as type
#pragma warning(disable : 4800)		// forcing value to bool
#pragma warning(disable : 4996)		// use safe functions insteaad
#define Q_EXPORT __declspec(dllexport)
#elif (defined __SUNPRO_C)
#define Q_EXPORT __global
#elif ((__GNUC__ >= 3) && (!__EMX__) && (!sun))
#define Q_EXPORT __attribute__((visibility("default")))
#else
#define Q_EXPORT
#endif

#define SOF2_VERSION_ID	"1.00"
#define	GAME_VERSION		"sof2mp-1.00"

#define OUTFITTING_GROUP_PRIMARY	0
#define OUTFITTING_GROUP_SECONDARY	1
#define OUTFITTING_GROUP_PISTOL		2
#define OUTFITTING_GROUP_GRENADE	3
#define OUTFITTING_GROUP_ACCESSORY	4
#define OUTFITTING_GROUP_MAX		5
#define OUTFITTING_GROUP_KNIFE		6

#define _USE_MATH_DEFINES // sets M_PI

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>
#ifdef __cplusplus
#include <algorithm>
using namespace std;
#else
#include <stdbool.h>
#endif

//======================= WIN32 DEFINES =================================

#ifdef WIN32

// buildstring will be incorporated into the version string
#ifndef _DEBUG
#define	CPUSTRING	"win-x86"
#else
#define	CPUSTRING	"win-x86-debug"
#endif

#define	PATH_SEP '\\'

#endif

//======================= LINUX DEFINES =================================
#ifdef __linux__

// bk001205 - from Makefile
#define stricmp strcasecmp

#define	MAC_STATIC // bk: FIXME
#define __inline inline 

#ifdef __i386__
#define	CPUSTRING	"linux-i386"
#elif defined __axp__
#define	CPUSTRING	"linux-alpha"
#else
#define	CPUSTRING	"linux-other"
#endif

#define	PATH_SEP '/'

// bk001205 - try
#ifdef Q3_STATIC
#define	GAME_HARD_LINKED
#define	CGAME_HARD_LINKED
#define	UI_HARD_LINKED
#define	BOTLIB_HARD_LINKED
#endif

#endif

//=============================================================

typedef unsigned char 		byte;

typedef enum {qfalse, qtrue}	qboolean;

typedef int		qhandle_t;
typedef int		fileHandle_t;

// angle indexes
#define	PITCH				0		// up / down
#define	YAW					1		// left / right
#define	ROLL				2		// fall over

// the game guarantees that no string from the network will ever
// exceed MAX_STRING_CHARS
#define	MAX_STRING_CHARS	1024	// max length of a string passed to Cmd_TokenizeString
#define	MAX_STRING_TOKENS	1024	// max tokens resulting from Cmd_TokenizeString
#define	MAX_TOKEN_CHARS		1024	// max length of an individual token

#define	MAX_INFO_STRING		1024
#define	MAX_INFO_KEY		1024
#define	MAX_INFO_VALUE		1024

#define	BIG_INFO_STRING		8192  // used for system info key only
#define	BIG_INFO_KEY		  8192
#define	BIG_INFO_VALUE		8192


#define	MAX_QPATH			64		// max length of a quake game pathname
#ifdef PATH_MAX
#define MAX_OSPATH			PATH_MAX
#else
#define	MAX_OSPATH			256		// max length of a filesystem pathname
#endif

#define	MAX_SAY_TEXT	150

// paramters for command buffer stuffing
typedef enum {
	EXEC_NOW,			// don't return until completed, a VM should NEVER use this,
						// because some commands might cause the VM to be unloaded...
	EXEC_INSERT,		// insert at current position, but don't run yet
	EXEC_APPEND			// add to end of the command buffer (normal case)
} cbufExec_t;


#define LS_STYLES_START			0
#define LS_NUM_STYLES			32
#define	LS_SWITCH_START			(LS_STYLES_START+LS_NUM_STYLES)
#define LS_NUM_SWITCH			32
#define MAX_LIGHT_STYLES		64

// parameters to the main Error routine
typedef enum {
	ERR_FATAL,					// exit the entire game with a popup window
	ERR_DROP,					// print to console and disconnect from game
	ERR_SERVERDISCONNECT,		// don't kill server
	ERR_DISCONNECT,				// client disconnected from the server
	ERR_NEED_CD					// pop up the need-cd dialog
} errorParm_t;

typedef float vec_t;
typedef vec_t vec3_t[3];

#define NUMVERTEXNORMALS	162
extern	vec3_t	bytedirs[NUMVERTEXNORMALS];

#define Q_COLOR_ESCAPE	'^'
#define Q_IsColorString(p)	( p && *(p) == Q_COLOR_ESCAPE && *((p)+1) && *((p)+1) != Q_COLOR_ESCAPE )

#define COLOR_BLACK		'0'
#define COLOR_RED		'1'
#define COLOR_GREEN		'2'
#define COLOR_YELLOW	'3'
#define COLOR_BLUE		'4'
#define COLOR_CYAN		'5'
#define COLOR_MAGENTA	'6'
#define COLOR_WHITE		'7'
#define ColorIndex(c)	( ( (c) - '0' ) & 7 )

#define S_COLOR_BLACK	"^0"
#define S_COLOR_RED		"^1"
#define S_COLOR_GREEN	"^2"
#define S_COLOR_YELLOW	"^3"
#define S_COLOR_BLUE	"^4"
#define S_COLOR_CYAN	"^5"
#define S_COLOR_MAGENTA	"^6"
#define S_COLOR_WHITE	"^7"

#define DEG2RAD( a ) ( ( (a) * M_PI ) / 180.0F )

extern	vec3_t	vec3_origin;
extern	vec3_t	vec3_identity;

// this isn't a real cheap function to call!
int DirToByte( vec3_t dir );

#define DotProduct(x,y)					((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define VectorSubtract(a,b,c)			((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd(a,b,c)				((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define VectorCopy(a,b)					((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
#define	VectorScale(v, s, o)			((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define	VectorMA(v, s, b, o)			((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))

#define VectorClear(a)			((a)[0]=(a)[1]=(a)[2]=0)
#define VectorSet(v, x, y, z)	((v)[0]=(x), (v)[1]=(y), (v)[2]=(z))

#define	SnapVector(v) {v[0]=((int)(v[0]));v[1]=((int)(v[1]));v[2]=((int)(v[2]));}

float RadiusFromBounds( const vec3_t mins, const vec3_t maxs );
void AddPointToBounds( const vec3_t v, vec3_t mins, vec3_t maxs );

static __inline int VectorCompare( const vec3_t v1, const vec3_t v2 ) {
	if (v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2]) {
		return 0;
	}			
	return 1;
}

static __inline vec_t VectorLength( const vec3_t v ) {
	return (vec_t)sqrt (v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

static __inline vec_t VectorLengthSquared( const vec3_t v ) {
	return (v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

static __inline vec_t Distance( const vec3_t p1, const vec3_t p2 ) {
	vec3_t	v;

	VectorSubtract (p2, p1, v);
	return VectorLength( v );
}

static __inline vec_t DistanceSquared( const vec3_t p1, const vec3_t p2 ) {
	vec3_t	v;

	VectorSubtract (p2, p1, v);
	return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
}

static __inline void VectorInverse( vec3_t v ){
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}

static __inline void CrossProduct( const vec3_t v1, const vec3_t v2, vec3_t cross ) {
	cross[0] = v1[1]*v2[2] - v1[2]*v2[1];
	cross[1] = v1[2]*v2[0] - v1[0]*v2[2];
	cross[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

vec_t VectorNormalize (vec3_t v);		// returns vector length
vec_t VectorNormalize2( const vec3_t v, vec3_t out );

int		Q_rand( int *seed );

#define random()	((rand () & 0x7fff) / ((float)0x7fff))
#define crandom()	(2.0 * (random() - 0.5))

void vectoangles( const vec3_t value1, vec3_t angles);
void AnglesToAxis( const vec3_t angles, vec3_t axis[3] );

float	AngleMod(float a);
void	LerpVector ( vec3_t from, vec3_t to, float lerp, vec3_t out );
float	AngleSubtract( float a1, float a2 );
void	AnglesSubtract( vec3_t v1, vec3_t v2, vec3_t v3 );

float AngleNormalize360 ( float angle );
float AngleNormalize180 ( float angle );
float AngleDelta ( float angle1, float angle2 );

void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);


//=============================================

float Com_Clampf( float min, float max, float value );
int Com_Clamp( int min, int max, int value );

char	*COM_SkipPath( char *pathname );
void	COM_StripExtension( const char *in, char *out );

const char	*SkipWhitespace(const char *data, bool *hasNewLines);
char	*COM_Parse( const char **data_p );
char	*COM_ParseExt( const char **data_p, bool allowLineBreak );

// mode parm for FS_FOpenFile
typedef enum {
	FS_READ,
	FS_WRITE,
	FS_APPEND,
	FS_APPEND_SYNC,
	FS_READ_TEXT,
	FS_WRITE_TEXT,
	FS_APPEND_TEXT,
	FS_APPEND_SYNC_TEXT
} fsMode_t;

char *va(char *format, ...);

char *Info_ValueForKey( const char *s, const char *key );
void Info_RemoveKey( char *s, const char *key );
void Info_SetValueForKey( char *s, const char *key, const char *value );

void	 Com_Error( int level, const char *error, ... );
void	 Com_Printf( const char *msg, ... );


/*
==========================================================

CVARS (console variables)

Many variables can be used for cheating purposes, so when
cheats is zero, force all unspecified variables to their
default values.
==========================================================
*/

#define	CVAR_ARCHIVE		0x00000001		// set to cause it to be saved to vars.rc
											// used for system variables, not for player
											// specific configurations
#define	CVAR_USERINFO		0x00000002		// sent to server on connect or change
#define	CVAR_SERVERINFO		0x00000004		// sent in response to front end requests
#define	CVAR_SYSTEMINFO		0x00000008		// these cvars will be duplicated on all clients
#define	CVAR_INIT			0x00000010		// don't allow change from console at all,
											// but can be set from the command line
#define	CVAR_LATCH			0x00000020		// will only change when C code next does
											// a Cvar_Get(), so it can't be changed
											// without proper initialization.  modified
											// will be set, even though the value hasn't
											// changed yet
#define	CVAR_ROM			0x00000040		// display only, cannot be set by user at all (can be set by code)
#define	CVAR_USER_CREATED	0x00000080		// created by a set command
#define	CVAR_TEMP			0x00000100		// can be set even when cheats are disabled, but is not archived
#define CVAR_CHEAT			0x00000200		// can not be changed if cheats are disabled
#define CVAR_NORESTART		0x00000400		// do not clear when a cvar_restart is issued
#define CVAR_INTERNAL		0x00000800		// cvar won't be displayed, ever (for passwords and such)
#define	CVAR_PARENTAL		0x00001000		// lets cvar system know that parental stuff needs to be updated
#define CVAR_LOCK_RANGE		0x00002000		// enforces the mins / maxs

#define	MAX_CVAR_VALUE_STRING	256

typedef int	cvarHandle_t;

// the modules that run in the virtual machine can't access the cvar_t directly,
// so they must ask for structured updates
typedef struct {
	cvarHandle_t	handle;
	int			modificationCount;
	float		value;
	int			integer;
	char		string[MAX_CVAR_VALUE_STRING];
} vmCvar_t;

#include "surfaceflags.h"			// shared with the q3map utility

// plane_t structure
// !!! if this is changed, it must be changed in asm code too !!!
typedef struct cplane_s {
	vec3_t	normal;
	float	dist;
	byte	type;			// for fast side tests: 0,1,2 = axial, 3 = nonaxial
	byte	signbits;		// signx + (signy<<1) + (signz<<2), used as lookup during collision
	byte	pad[2];
} cplane_t;
/*
Ghoul2 Insert Start
*/
typedef struct
{
	float		mDistance;
	int			mEntityNum;
	int			mModelIndex;
	int			mPolyIndex;
	int			mSurfaceIndex;
	vec3_t		mCollisionPosition;
	vec3_t		mCollisionNormal;
	int			mFlags;
	int			mMaterial;
	int			mLocation;
	float		mBarycentricI; // two barycentic coodinates for the hit point
	float		mBarycentricJ; // K = 1-I-J
}CollisionRecord_t;

#define MAX_G2_COLLISIONS 16

typedef CollisionRecord_t G2Trace_t[MAX_G2_COLLISIONS];	// map that describes all of the parts of ghoul2 models that got hit

/*
Ghoul2 Insert End
*/
// a trace is returned when a box is swept through the world
typedef struct {
	qboolean	allsolid;	// if true, plane is not valid
	qboolean	startsolid;	// if true, the initial point was in a solid area
	float		fraction;	// time completed, 1.0 = didn't hit anything
	vec3_t		endpos;		// final position
	cplane_t	plane;		// surface normal at impact, transformed to world space
	int			surfaceFlags;	// surface hit
	int			contents;	// contents on other side of surface hit
	int			entityNum;	// entity the contacted sirface is a part of
} trace_t;

typedef struct 
{
	float		yawAngle;
	bool	yawing;

	float		pitchAngle;
	bool	pitching;

	int			anim;
	int			animTime;

} animInfo_t;

/*
========================================================================

  ELEMENTS COMMUNICATED ACROSS THE NET

========================================================================
*/

#define	ANGLE2SHORT(x)	((int)((x)*65536/360) & 65535)
#define	SHORT2ANGLE(x)	((x)*(360.0/65536))

//
// per-level limits
//
#define	MAX_CLIENTS			64		// absolute limit
#define MAX_LOCATIONS		64
#define MAX_TERRAINS		32
#define MAX_LADDERS			64

#define MAX_INSTANCE_TYPES		16

#define	GENTITYNUM_BITS			10		// don't need to send any more
#define	MAX_GENTITIES			(1<<GENTITYNUM_BITS)

// entitynums are communicated with GENTITY_BITS, so any reserved
// values thatare going to be communcated over the net need to
// also be in this range
#define	ENTITYNUM_NONE			(MAX_GENTITIES-1)
#define	ENTITYNUM_WORLD			(MAX_GENTITIES-2)
#define	ENTITYNUM_MAX_NORMAL	(MAX_GENTITIES-2)


// these are also in be_aas_def.h - argh (rjr)
#define	MAX_MODELS				256		// these are sent over the net as 8 bits
#define	MAX_SOUNDS				256		// so they cannot be blindly increased
#define MAX_AMBIENT_SOUNDSETS	64
#define MAX_FX					64		// max effects strings, I'm hoping that 64 will be plenty
#define MAX_SUB_BSP				32
#define MAX_ICONS				32
#define	MAX_CHARSKINS			64		// character skins

#define	MAX_CONFIGSTRINGS	1400

// these are the only configstrings that the system reserves, all the
// other ones are strictly for servergame to clientgame communication
#define	CS_SERVERINFO		0		// an info string with all the serverinfo cvars
#define	CS_SYSTEMINFO		1		// an info string for server system to client system configuration (timescale, etc)
#define CS_PLAYERS			2		// info string for player user info
#define CS_CUSTOM			(CS_PLAYERS + MAX_CLIENTS )

#define	RESERVED_CONFIGSTRINGS	2	// game can't modify below this, only the system can

//=========================================================

// bit field limits
#define	MAX_STATS				16
#define	MAX_PERSISTANT			16
#define	MAX_AMMO				16
#define	MAX_WEAPONS				32
#define MAX_GAMETYPE_ITEMS		5

#define	MAX_PS_EVENTS			4

#define PS_PMOVEFRAMECOUNTBITS	6

typedef enum
{
	ATTACK_NORMAL,
	ATTACK_ALTERNATE,
	ATTACK_MAX

} attackType_t;

// playerState_t is the information needed by both the client and server
// to predict player motion and actions
// nothing outside of pmove should modify these, or some degree of prediction error
// will occur

// you can't add anything to this without modifying the code in msg.c

// playerState_t is a full superset of entityState_t as it is used by players,
// so if a playerState_t is transmitted, the entityState_t can be fully derived
// from it.

typedef struct playerState_s 
{
	int			commandTime;	// cmd->serverTime of last executed command
	int			pm_type;
	int			bobCycle;		// for view bobbing and footstep generation
	int			pm_flags;		// ducked, etc
	int			pm_debounce;	// debounce buttons
	int			pm_time;

	vec3_t		origin;
	vec3_t		velocity;

	int			weaponTime;
	int			weaponFireBurstCount;
	int			weaponAnimId;
	int			weaponAnimIdChoice;
	int			weaponAnimTime;
	int			weaponCallbackTime;
	int			weaponCallbackStep;

	int			gravity;
	int			speed;
	int			delta_angles[3];				// add to command angles to get view direction
												// changed by spawns, rotating objects, and teleporters
	int			groundEntityNum;				// ENTITYNUM_NONE = in air
												
	int			legsAnim;						// mask off ANIM_TOGGLEBIT
												
	int			torsoTimer;						// don't change low priority animations until this runs out
	int			torsoAnim;						// mask off ANIM_TOGGLEBIT
												
	int			movementDir;					// a number 0 to 7 that represents the reletive angle
												// of movement to the view angle (axial and diagonals)
												// when at rest, the value will remain unchanged
												// used to twist the legs during strafing
												
	int			eFlags;							// copied to entityState_t->eFlags
												
	int			eventSequence;					// pmove generated events
	int			events[MAX_PS_EVENTS];			
	int			eventParms[MAX_PS_EVENTS];		
												
	int			externalEvent;					// events set on player from another source
	int			externalEventParm;				
	int			externalEventTime;				
												
	int			clientNum;						// ranges from 0 to MAX_CLIENTS-1
	int			weapon;							// copied to entityState_t->weapon
	int			weaponstate;					
												
	vec3_t		viewangles;						// for fixed views
	int			viewheight;						
												
	// damage feedback							
	int			damageEvent;					// when it changes, latch the other parms
	int			damageYaw;						
	int			damagePitch;					
	int			damageCount;					
												
	int			painTime;						// used for both game and client side to process the pain twitch - NOT sent across the network
	int			painDirection;					// NOT sent across the network
										
	int			stats[MAX_STATS];				
	int			persistant[MAX_PERSISTANT];		// stats that aren't cleared on death
	int			ammo[MAX_AMMO];
	int			clip[ATTACK_MAX][MAX_WEAPONS];
	int			firemode[MAX_WEAPONS];

	int			generic1;
	int			loopSound;

	// Incaccuracy values for firing
	int			inaccuracy;
	int			inaccuracyTime;
	int			kickPitch;

	// not communicated over the net at all
	int			ping;							// server to game info for scoreboard
	int			pmove_framecount;				// FIXME: don't transmit over the network
	int			jumppad_frame;
	int			entityEventSequence;
	vec3_t		pvsOrigin;						// view origin used to calculate PVS (also the lean origin)
												// THIS VARIABLE MUST AT LEAST BE THE PLAYERS ORIGIN ALL OF THE 
												// TIME OR THE PVS CALCULATIONS WILL NOT WORK.

	// Zooming
	int			zoomTime;
	int			zoomFov;

	// LAdders
	int			ladder;
	int			leanTime;

	// Timers 
	int			grenadeTimer;
	int			respawnTimer;

} playerState_t;


typedef enum 
{
	TEAM_FREE,
	TEAM_RED,
	TEAM_BLUE,
	TEAM_SPECTATOR,

	TEAM_NUM_TEAMS

} team_t;

//====================================================================


//
// usercmd_t->button bits, many of which are generated by the client system,
// so they aren't game/cgame only definitions
//
#define	BUTTON_ATTACK		(1<<0)
#define	BUTTON_TALK			(1<<1)			// displays talk balloon and disables actions	
#define BUTTON_GOGGLES		(1<<2)			// turns nv or therm goggles on/off
#define BUTTON_LEAN			(1<<3)			// lean modifier, when held strafe left and right will lean
#define	BUTTON_WALKING		(1<<4)			// walking can't just be infered from MOVE_RUN
											// because a key pressed late in the frame will
											// only generate a small move value for that frame
											// walking will use different animations and
											// won't generate footsteps
#define	BUTTON_USE			(1<<5)			// the ol' use key returns!
#define	BUTTON_RELOAD		(1<<6)			// reloads current weapon
#define BUTTON_ALT_ATTACK	(1<<7)
#define	BUTTON_ANY			(1<<8)			// any key whatsoever
#define BUTTON_ZOOMIN		(1<<9)
#define BUTTON_ZOOMOUT		(1<<10)
#define BUTTON_FIREMODE		(1<<11)

#define BUTTON_LEAN_RIGHT	(1<<12)
#define BUTTON_LEAN_LEFT	(1<<13)


// usercmd_t is sent to the server each client frame
typedef struct usercmd_s 
{
	int				serverTime;
	int				angles[3];
	int 			buttons;
	byte			weapon;
	signed char		forwardmove;
	signed char		rightmove;
	signed char		upmove;

} usercmd_t;

typedef enum {
	TR_STATIONARY,
	TR_INTERPOLATE,				// non-parametric, but interpolate between snapshots
	TR_LINEAR,
	TR_LINEAR_STOP,
	TR_SINE,					// value = base + sin( time / duration ) * delta
	TR_GRAVITY,
	TR_HEAVYGRAVITY,
	TR_LIGHTGRAVITY
} trType_t;

typedef struct {
	trType_t	trType;
	int		trTime;
	int		trDuration;			// if non 0, trTime + trDuration = stop time
	vec3_t	trBase;
	vec3_t	trDelta;			// velocity, etc
} trajectory_t;

// entityState_t is the information conveyed from the server
// in an update message about entities that the client will
// need to render in some way
// Different eTypes may use the information in different ways
// The messages are delta compressed, so it doesn't really matter if
// the structure size is fairly large

typedef struct entityState_s 
{
	int				number;			// entity index
	int				eType;			// entityType_t
	int				eFlags;

	trajectory_t	pos;			// for calculating position
	trajectory_t	apos;			// for calculating angles

	int				time;
	int				time2;
					
	vec3_t			origin;
	vec3_t			origin2;
					
	vec3_t			angles;
	vec3_t			angles2;
					
	int				otherEntityNum;	// shotgun sources, etc
	int				otherEntityNum2;
					
	int				groundEntityNum;	// -1 = in air
					
	int				loopSound;		// constantly loop this sound
	int				mSoundSet;
										
	int				modelindex;
	int				modelindex2;
	int				clientNum;		// 0 to (MAX_CLIENTS - 1), for players and corpses
	int				frame;
					
	int				solid;			// for client side prediction, trap_linkentity sets this properly
					
	int				event;			// impulse events -- muzzle flashes, footsteps, etc
	int				eventParm;

	int				generic1;

	// for players
	// these fields are only transmitted for client entities!!!!!
	int				gametypeitems;	// bit flags indicating which items are carried
	int				weapon;			// determines weapon and flash model, etc
	int				legsAnim;		// mask off ANIM_TOGGLEBIT
	int				torsoAnim;		// mask off ANIM_TOGGLEBIT
	int				torsoTimer;		// time the animation will play for
	int				leanOffset;		// Lean direction
} entityState_t;

#define SAY_ALL		0
#define SAY_TEAM	1
#define SAY_TELL	2

float flrand(float min, float max);
int irand(int min, int max);

typedef enum Eorientations
{
	ORIGIN = 0,
	POSITIVE_X,
	POSITIVE_Z,
	POSITIVE_Y,
	NEGATIVE_X,
	NEGATIVE_Z,
	NEGATIVE_Y
};

/*
========================================================================

String ID Tables

========================================================================
*/
#define ENUM2STRING(arg)   #arg,arg
typedef struct stringID_table_s
{
	char	*name;
	int		id;
} stringID_table_t;

int GetIDForString ( stringID_table_t *table, const char *string );

#endif	// __Q_SHARED_H
