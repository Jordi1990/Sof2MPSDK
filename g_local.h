// Copyright (C) 2001-2002 Raven Software.
//
// g_local.h -- local definitions for game module
#ifndef G_LOCAL_H
#define G_LOCAL_H
#include "q_shared.h"
#include "bg_public.h"
#include "g_public.h"
#include "g2.h"

// Classes
#include "userinfo.h"
//==================================================================

// the "gameversion" client command will print this plus compile date
#define	GAMEVERSION	"sof2mp"

#define BODY_QUEUE_SIZE_MAX			MAX_CLIENTS
#define BODY_QUEUE_SIZE				8

#define	FRAMETIME					100					// msec

#define	INTERMISSION_DELAY_TIME		3000

// gentity->flags
#define	FL_GODMODE					0x00000010
#define	FL_NOTARGET					0x00000020
#define	FL_TEAMSLAVE				0x00000400	// not the first on the team
#define FL_NO_KNOCKBACK				0x00000800
#define FL_DROPPED_ITEM				0x00001000
#define FL_NO_BOTS					0x00002000	// spawn point not for bot use
#define FL_NO_HUMANS				0x00004000	// spawn point just for bots

#define	MAX_SPAWNS					128

// movers are things like doors, plats, buttons, etc
typedef enum 
{
	MOVER_POS1,
	MOVER_POS2,
	MOVER_1TO2,
	MOVER_2TO1

} moverState_t;

typedef struct gentity_s gentity_t;
typedef struct gclient_s gclient_t;

struct gentity_s 
{
	entityState_t	s;				// communicated by server to clients
	entityShared_t	r;				// shared by both the server system and game

	// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
	// EXPECTS THE FIELDS IN THAT ORDER!
	//================================

	struct gclient_s	*client;			// NULL if not a client

	bool	inuse;

	char		*classname;			// set in QuakeEd
	int			spawnflags;			// set in QuakeEd

	bool	neverFree;			// if true, FreeEntity will only unlink
									// bodyque uses this

	int			flags;				// FL_* variables

	char		*model;
	char		*model2;
	int			freetime;			// level.time when the object was freed
	
	int			eventTime;			// events will be cleared EVENT_VALID_MSEC after set
	bool	freeAfterEvent;
	bool	unlinkAfterEvent;

	bool	physicsObject;		// if true, it can be pushed by movers and fall off edges
									// all game items are physicsObjects, 
	float		physicsBounce;		// 1.0 = continuous bounce, 0.0 = no bounce
	int			clipmask;			// brushes with this content value will be collided against
									// when moving.  items and corpses do not collide against
									// players, for instance

	// movers
	moverState_t moverState;
	int			soundPos1;
	int			sound1to2;
	int			sound2to1;
	int			soundPos2;
	int			soundLoop;
	gentity_t	*parent;
	gentity_t	*nextTrain;
	vec3_t		pos1, pos2;

	char		*message;

	int			timestamp;		// body queue sinking, etc

	float		angle;			// set in editor, -1 = up, -2 = down
	char		*target;
	char		*targetname;
	char		*team;
	char		*targetShaderName;
	char		*targetShaderNewName;
	gentity_t	*target_ent;

	float		speed;
	vec3_t		movedir;
	float		mass;
	int			setTime;

//Think Functions
	int			nextthink;
	void		(*think)(gentity_t *self);
	void		(*reached)(gentity_t *self);	// movers call this when hitting endpoint
	void		(*blocked)(gentity_t *self, gentity_t *other);
	void		(*touch)(gentity_t *self, gentity_t *other, trace_t *trace);
	void		(*use)(gentity_t *self, gentity_t *other, gentity_t *activator);
	void		(*pain)(gentity_t *self, gentity_t *attacker, int damage);
	void		(*die)(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod, int hitLocation, vec3_t hitDir );

	int			pain_debounce_time;
	int			fly_sound_debounce_time;	// wind tunnel
	int			last_move_time;

//Health and damage fields
	int			health;
	bool	takedamage;

	int			damage;
	int			dflags;
	int			splashDamage;	// quad will increase this without increasing radius
	int			splashRadius;
	int			methodOfDeath;
	int			splashMethodOfDeath;

	int			count;
	float		bounceScale;

	gentity_t	*chain;
	gentity_t	*enemy;
	gentity_t	*activator;
	gentity_t	*teamchain;		// next entity in team
	gentity_t	*teammaster;	// master of the team

	int			watertype;
	int			waterlevel;

	int			noise_index;

	// timing variables
	float		wait;
	float		random;
	int			delay;

	gitem_t		*item;			// for bonus items
};

typedef struct gspawn_s
{
	team_t		team;
	vec3_t		origin;
	vec3_t		angles;

} gspawn_t;

typedef enum 
{
	CON_DISCONNECTED,
	CON_CONNECTING,
	CON_CONNECTED

} clientConnected_t;

typedef enum 
{
	SPECTATOR_NOT,
	SPECTATOR_FREE,
	SPECTATOR_FOLLOW,
	SPECTATOR_SCOREBOARD

} spectatorState_t;

typedef enum 
{
	TEAM_BEGIN,		// Beginning a team game, spawn at base
	TEAM_ACTIVE		// Now actively playing

} playerTeamStateState_t;

typedef struct 
{
	playerTeamStateState_t	state;

	int			location;

	int			captures;
	int			basedefense;
	int			carrierdefense;
	int			flagrecovery;
	int			fragcarrier;
	int			assists;

	float		lasthurtcarrier;
	float		lastreturnedflag;
	float		flagsince;
	float		lastfraggedcarrier;

} playerTeamState_t;

// the auto following clients don't follow a specific client
// number, but instead follow the first two active players
#define	FOLLOW_ACTIVE1	-1
#define	FOLLOW_ACTIVE2	-2

// client data that stays across multiple levels or map restarts
// this is achieved by writing all the data to cvar strings at game shutdown
// time and reading them back at connection time.  Anything added here
// MUST be dealt with in G_InitSessionData() / G_ReadSessionData() / G_WriteSessionData()
typedef struct 
{
	team_t				team;					// current team
	int					spectatorTime;			// for determining next-in-line to play
	spectatorState_t	spectatorState;			
	int					spectatorClient;		// for chasecam and follow mode
	int					score;					// total score
	bool			ghost;					// Whether or not the player is a ghost (sticks past team change)
	int					teamkillDamage;			// amount of damage death to teammates
	int					teamkillForgiveTime;	// time when team damage will be forgivin
	bool			noTeamChange;			// cant change teams when this is true (rt_none only)

} clientSession_t;

//
#define MAX_NETNAME			36
#define MAX_IDENTITY		64
#define	MAX_VOTE_COUNT		3

typedef struct statinfo_s
{
	int weapon_shots[2][WP_NUM_WEAPONS];
	int weapon_hits[2][WP_NUM_WEAPONS];
	int weapon_headshots[2][WP_NUM_WEAPONS];
	int			weapon;
	int			attack;
	int         shotcount;
	int		    hitcount;
	float		accuracy;
	float		ratio;
	int         handhits;
	int         foothits;
	int         armhits;
	int         leghits;
	int         headhits;
	int         neckhits;
	int         torsohits;
	int         waisthits;
	int			headShotKills;
	int			kills;
	int			killsinarow;
	int			deaths;
	int			damageDone;
	int			damageTaken;
	int			lastclient_hurt;
	int			lasthurtby;
	int			overallScore;
	int			explosiveKills;
	int			knifeKills;
} statinfo_t;

// client data that stays across multiple respawns, but is cleared
// on each level change or team change at ClientBegin()
typedef struct 
{
	clientConnected_t	connected;	
	usercmd_t			cmd;						// we would lose angles if not persistant
	bool			localClient;				// true if "ip" info key is "localhost"
	bool			initialSpawn;				// the first spawn should be at a cool location
	bool			predictItemPickup;			// based on cg_predictItems userinfo
	bool			pmoveFixed;					//
	bool			antiLag;					// anti-lag on or off
	bool			autoReload;					// auto rellaod turned on or off
	string				netname;
	char				deferredname[MAX_NETNAME];
	int					netnameTime;				// Last time the name was changed
	TIdentity*			identity;					// Current identity
	goutfitting_t		outfitting;					// Current outfitting
	int					enterTime;					// level.time the client entered the game
	playerTeamState_t	teamState;					// status in teamplay games
	int					voteCount;					// to prevent people from constantly calling votes
	int					firemode[MAX_WEAPONS];		// weapon firemodes

	statinfo_t			statinfo;					// This holds the client's stats (stuns, last damage by, etc..).
	float	rpmClient;
	string	cleanName;
	string	ip;
	string country;
	string countryCode;

} clientPersistant_t;

#define MAX_SERVER_FPS		40
#define MAX_ANTILAG			MAX_SERVER_FPS

// Antilag information
typedef struct gantilag_s
{
	vec3_t	rOrigin;				// entity.r.currentOrigin
	vec3_t	rAngles;				// entity.r.currentAngles
	vec3_t	mins;					// entity.r.mins
	vec3_t	maxs;					// entity.r.maxs

	vec3_t	legsAngles;				// entity.client.ghoulLegsAngles
	vec3_t	lowerTorsoAngles;		// entity.client.ghoulLowerTorsoAngles
	vec3_t	upperTorsoAngles;		// entity.client.ghoulUpperTorsoAngles
	vec3_t	headAngles;				// entity.client.ghoulHeadAngles

	int		time;					// time history item was saved
	int		leveltime;

	int		torsoAnim;				// entity.client.ps.torsoAnim
	int		legsAnim;				// entity.client.ps.legsAnim

	float	leanTime;				// entity.client.ps.leanOffset
	int		pm_flags;				// entity.client.ps.pm_flags

} gantilag_t;

// this structure is cleared on each ClientSpawn(),
// except for 'client->pers' and 'client->sess'
struct gclient_s 
{
	// ps MUST be the first element, because the server expects it
	playerState_t	ps;				// communicated by server to clients

	// the rest of the structure is private to game
	clientPersistant_t	pers;
	clientSession_t		sess;

	bool	readyToExit;		// wishes to leave the intermission

	bool	noclip;

	int			lastCmdTime;		// level.time of last usercmd_t, for EF_CONNECTION
									// we can't just use pers.lastCommand.time, because
									// of the g_sycronousclients case
	int			buttons;
	int			oldbuttons;
	int			latched_buttons;

	vec3_t		oldOrigin;

	// sum up damage over an entire frame, so
	// shotgun blasts give a single big kick
	int			damage_armor;		// damage absorbed by armor
	int			damage_blood;		// damage taken out of health
	int			damage_knockback;	// impact damage
	vec3_t		damage_from;		// origin for vector calculation
	bool	damage_fromWorld;	// if true, don't use the damage_from vector
	
	int			accurateCount;		// for "impressive" reward sound

	//
	int			lastkilled_client;	// last client that this client killed
	int			lasthurt_client;	// last client that damaged this client
	int			lasthurt_time;		// time this client was last hurt
	int			lasthurt_mod;		// type of damage the client did

	// timers
	int			respawnTime;		// can respawn when time > this, force after g_forcerespwan
	int			invulnerableTime;	// invulnerable time
	int			inactivityTime;		// kick players when time > this
	bool	inactivityWarning;	// qtrue if the five seoond warning has been given
	int			rewardTime;			// clear the EF_AWARD_IMPRESSIVE, etc when time > this

	int			airOutTime;

	int			lastKillTime;		// for multiple kill rewards

	bool	fireHeld;			// used for hook
	gentity_t	*hook;				// grapple hook if out

	int			switchTeamTime;		// time the player switched teams
	int			voteDelayTime;		// time when this client can vote again

	char		*areabits;

	int				voiceFloodTimer;		// Timer used to forgive voice chat flooding
	int				voiceFloodCount;		// Amount of voice chats that need to be forgivin
	int				voiceFloodPenalty;		// Time when a client can voice chat again

	// Anti-lag information
	gantilag_t		antilag[MAX_ANTILAG];
	gantilag_t		antilagUndo;
	int				antilagHead;

	bool		noOutfittingChange;

	animInfo_t		torso;
	animInfo_t		legs;
	vec3_t			ghoulLegsAngles;
	vec3_t			ghoulLowerTorsoAngles;
	vec3_t			ghoulUpperTorsoAngles;
	vec3_t			ghoulHeadAngles;

	gentity_t		*siameseTwin;
};


//
// this structure is cleared as each map is entered
//
#define	MAX_SPAWN_VARS			64
#define	MAX_SPAWN_VARS_CHARS	4096

typedef struct 
{
	struct gclient_s	*clients;		// [maxclients]

	struct gentity_s	*gentities;

	int				gentitySize;
	int				num_entities;		// current number, <= MAX_GENTITIES

	int				warmupTime;			// restart match at this time

	int				gametype;			// Current gametype index (gametypeData[gametype])
	gametypeData_t*	gametypeData;		// Current gametype data for easier access

	vec3_t			worldMins;			// Absolute min coords of the world
	vec3_t			worldMaxs;			// Absolute max coords of the world

	fileHandle_t	logFile;

	// store latched cvars here that we want to get at often
	int			maxclients;

	int			framenum;
	int			time;					// in msec
	int			previousTime;			// so movers can back up when blocked
	int			frameStartTime;			

	int			startTime;				// level.time the map was started
	int			globalVoiceTime;		// last global voice

	int			teamScores[TEAM_NUM_TEAMS];
	int			lastTeamLocationTime;		// last time of client team location update

	bool	newSession;				// don't use any old session data, because
										// we changed gametype

	bool	restarted;				// waiting for a map_restart to fire

	int			numConnectedClients;
	int			numNonSpectatorClients;	// includes connecting clients
	int			numPlayingClients;		// connected, non-spectators
	int			sortedClients[MAX_CLIENTS];		// sorted by score
	int			follow1, follow2;		// clientNums for auto-follow spectators

	int			warmupModificationCount;	// for detecting if g_warmup is changed

	// voting state
	char		voteString[MAX_STRING_CHARS];
	char		voteDisplayString[MAX_STRING_CHARS];
	int			voteTime;				// level.time vote was called
	int			voteExecuteTime;		// time the vote is executed
	int			voteYes;
	int			voteNo;
	int			voteClient;				// client who initiated vote
	int			numVotingClients;		// set by CalculateRanks

	// spawn variables
	bool	spawning;				// the G_Spawn*() functions are valid
	int			numSpawnVars;
	char		*spawnVars[MAX_SPAWN_VARS][2];	// key / value pairs
	int			numSpawnVarChars;
	char		spawnVarChars[MAX_SPAWN_VARS_CHARS];

	// intermission state
	int			intermissionQueued;		// intermission was qualified, but
										// wait INTERMISSION_DELAY_TIME before
										// actually going there so the last
										// frag can be watched.  Disable future
										// kills during this delay
	int			intermissiontime;		// time the intermission was started
	char		*changemap;
	bool	readyToExit;			// at least one client wants to exit
	int			exitTime;
	vec3_t		intermission_origin;	// also used for spectator spawns
	vec3_t		intermission_angle;

	bool	locationLinked;			// target_locations get linked
	gentity_t	*locationHead;			// head of the location list

	// body queue
	int			bodyQueIndex;			// dead bodies
	int			bodyQueSize;			// how many dead bodies can there be
	int			bodySinkTime;
	gentity_t	*bodyQue[BODY_QUEUE_SIZE_MAX];

	int			portalSequence;

	int			mNumBSPInstances;
	int			mBSPInstanceDepth;
	vec3_t		mOriginAdjust;
	float		mRotationAdjust;
	char		*mTargetAdjust;
	bool	hasBspInstances;
	char		mFilter[MAX_QPATH];
	char		mTeamFilter[MAX_QPATH];

	struct gentity_s	*landScapes[MAX_TERRAINS];

	int				gametypeStartTime;
	int				gametypeJoinTime;
	int				gametypeResetTime;
	int				gametypeRoundTime;
	int				gametypeRespawnTime[TEAM_NUM_TEAMS];
	int				gametypeDelayTime;
	const char*		gametypeTeam[TEAM_NUM_TEAMS];
	
	void*			serverGhoul2;
	animation_t		ghoulAnimations[MAX_ANIMATIONS];

	int				ladderCount;

	gspawn_t		spawns[MAX_SPAWNS];
	int				spawnCount;

	bool		pickupsDisabled;

	int				timeExtension;

	int lastTMIUpdate;

} level_locals_t;

//
// g_spawn.c
//
bool	G_SpawnString( const char *key, const char *defaultString, char **out );
// spawn string returns a temporary reference, you must CopyString() if you want to keep it
bool	G_SpawnFloat( const char *key, const char *defaultString, float *out );
bool	G_SpawnInt( const char *key, const char *defaultString, int *out );
bool	G_ParseSpawnVars( bool inSubBSP );
void		G_SpawnGEntityFromSpawnVars( bool inSubBSP );
void		G_SpawnEntitiesFromString( bool inSubBSP );
char		*G_NewString( const char *string );
void		AddSpawnField(char *field, char *value);

//
// g_cmds.c
//
void		Cmd_Score_f			( gentity_t *ent );
void		G_StopFollowing		( gentity_t *ent );
void		G_StopGhosting		( gentity_t* ent );
void		G_StartGhosting		( gentity_t* ent );

void		BroadcastTeamChange( gclient_t *client, int oldTeam );
void		SetTeam( gentity_t *ent, char *s, const char* identity );
void		Cmd_FollowCycle_f( gentity_t *ent, int dir );
bool	CheatsOk				( gentity_t *ent );
void		G_SpawnDebugCylinder	( vec3_t origin, float radius, gentity_t* clientent, float viewRadius, int colorIndex );
void		G_Voice					( gentity_t *ent, gentity_t *target, int mode, const char *id, bool voiceonly );
void		G_VoiceGlobal			( gentity_t* ent, const char* id, bool force );
bool	G_CanVoiceGlobal		( void );

//
// g_items.c
//

void G_RunItem( gentity_t *ent );
void RespawnItem( gentity_t *ent );

gentity_t*	G_DropItem			( gentity_t *ent, gitem_t *item, float angle );
gentity_t*	G_DropWeapon		( gentity_t* ent, weapon_t weapon, int pickupDelay );

void G_SpawnItem (gentity_t *ent, gitem_t *item);
void FinishSpawningItem( gentity_t *ent );

void	Add_Ammo (gentity_t *ent, int weapon, int count);
void Touch_Item (gentity_t *ent, gentity_t *other, trace_t *trace);

void ClearRegisteredItems( void );
void RegisterItem( gitem_t *item );
void SaveRegisteredItems( void );

//
// g_utils.c
//
int		G_ModelIndex		( char *name );
int		G_SoundIndex		( char *name );
int		G_AmbientSoundSetIndex( char *name ); 
int		G_BSPIndex			( char *name );
int		G_EffectIndex		( char *name );

void	G_TeamCommand( team_t team, char *cmd );
void	G_KillBox (gentity_t *ent);
gentity_t *G_Find (gentity_t *from, int fieldofs, const char *match);
gentity_t *G_PickTarget (char *targetname);
void	G_UseTargets (gentity_t *ent, gentity_t *activator);
void	G_UseTargetsByName( const char* name, gentity_t *exclude, gentity_t *activator );
void	G_SetMovedir ( vec3_t angles, vec3_t movedir);

void	G_InitGentity( gentity_t *e );
gentity_t	*G_Spawn (void);
gentity_t *G_TempEntity( vec3_t origin, int event );
void	G_PlayEffect(int fxID, vec3_t org, vec3_t ang);
void	G_Sound( gentity_t *ent, int soundIndex );
void	G_FreeEntity( gentity_t *e );

void	G_TouchTriggers (gentity_t *ent);

char	*vtos( const vec3_t v );

float vectoyaw( const vec3_t vec );

void G_AddPredictableEvent( gentity_t *ent, int event, int eventParm );
void G_AddEvent( gentity_t *ent, int event, int eventParm );
void G_SetOrigin( gentity_t *ent, vec3_t origin );
void AddRemap(const char *oldShader, const char *newShader, float timeOffset);
const char *BuildShaderStateConfig(void);

// CG specific API access

void		trap_G2API_CleanGhoul2Models	( void **ghoul2Ptr );
void		trap_G2API_CollisionDetect		( CollisionRecord_t *collRecMap, void* ghoul2, const vec3_t angles, const vec3_t position,int frameNumber, int entNum, vec3_t rayStart, vec3_t rayEnd, vec3_t scale, int traceFlags, int useLod );
qhandle_t	trap_G2API_RegisterSkin			( const char *skinName, int numPairs, const char *skinPairs);
bool	trap_G2API_SetSkin				( void* ghoul2, int modelIndex, qhandle_t customSkin);
bool	trap_G2API_GetAnimFileNameIndex ( void* ghoul2, qhandle_t modelIndex, const char* name );

int			trap_G2API_InitGhoul2Model(void **ghoul2Ptr, const char *fileName, int modelIndex, qhandle_t customSkin,
						  qhandle_t customShader, int modelFlags, int lodBias);

bool	trap_G2API_SetBoneAngles(void *ghoul2, int modelIndex, const char *boneName, const vec3_t angles, const int flags,
								const int up, const int right, const int forward, qhandle_t *modelList,
								int blendTime , int currentTime );
bool	trap_G2API_SetBoneAnim(void *ghoul2, const int modelIndex, const char *boneName, const int startFrame, const int endFrame,
							  const int flags, const float animSpeed, const int currentTime, const float setFrame , const int blendTime );


//
// g_combat.c
//
bool	CanDamage			( gentity_t *targ, vec3_t origin);
int			G_GetHitLocation	( gentity_t *target, vec3_t ppoint, vec3_t dir );
int			G_Damage			( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker, vec3_t dir, vec3_t point, int damage, int dflags, int mod, int location );
bool	G_RadiusDamage		( vec3_t origin, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int power, int mod);
void		body_die			( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath, int hitLocation, vec3_t hitDir );
void		TossClientItems		( gentity_t *self );

// damage flags
#define DAMAGE_RADIUS				0x00000001	// damage was indirect
#define DAMAGE_NO_ARMOR				0x00000002	// armour does not protect from this damage
#define DAMAGE_NO_KNOCKBACK			0x00000004	// do not affect velocity, just view angles
#define DAMAGE_NO_PROTECTION		0x00000008  // armor, shields, invulnerability, and godmode have no effect
#define DAMAGE_NO_TEAM_PROTECTION	0x00000010  // armor, shields, invulnerability, and godmode have no effect
#define DAMAGE_DEATH_KNOCKBACK		0x00000080	// only does knockback on death of target
#define DAMAGE_AREA_DAMAGE			0x00000400	// spawn area damage
#define DAMAGE_NO_GORE				0x00000800	// dont spawn gore pieces with this damage
#define	DAMAGE_FORCE_GORE			0x00001000	// force something to pop off

//
// g_missile.c
//
void		G_RunMissile		( gentity_t *ent );
gentity_t*	G_CreateMissile		( vec3_t org, vec3_t dir, float vel, int life, gentity_t *owner, attackType_t attack );
gentity_t*	G_CreateDamageArea	( vec3_t origin, gentity_t* attacker, float damage, float radius, int duration, int mod );
void		G_ExplodeMissile	( gentity_t *ent );
void		G_GrenadeThink		( gentity_t* ent );

//
// g_mover.c
//
void		G_RunMover			( gentity_t *ent );
void		Touch_DoorTrigger	( gentity_t *ent, gentity_t *other, trace_t *trace );
void		G_ResetGlass		( void );


//
// g_trigger.c
//
void		trigger_teleporter_touch		( gentity_t *self, gentity_t *other, trace_t *trace );
void		InitTrigger						( gentity_t *self );

//
// g_misc.c
//
void		TeleportPlayer					( gentity_t *player, vec3_t origin, vec3_t angles );


//
// g_weapon.c
//
void		SnapVectorTowards				( vec3_t v, vec3_t to );
void		G_InitHitModels					( void );

//
// g_client.c
//
bool	G_IsClientSpectating			( gclient_t* client );
bool	G_IsClientDead					( const gclient_t* client );
int			TeamCount						( int ignoreClientNum, team_t team, int* alive );
team_t		PickTeam						( int ignoreClientNum );
void		SetClientViewAngle				( gentity_t *ent, vec3_t angle );
void		CopyToBodyQue					( gentity_t *ent, int hitLocation, vec3_t hitDirection );
void		respawn							( gentity_t *ent);
void		BeginIntermission				( void);
void		ClientSpawn						( gentity_t *ent );
void		player_die						( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod, int hitLocation, vec3_t hitDir );
void		G_InitBodyQueue					( void);
void		G_AddScore						( gentity_t *ent, int score );
void		CalculateRanks					( void );
bool	G_SpotWouldTelefrag				( gspawn_t* spawn );
void		G_UpdateClientAnimations		( gentity_t* ent );
void		G_SetRespawnTimer				( gentity_t* ent );
gentity_t*	G_FindNearbyClient				( vec3_t origin, team_t team, float radius, gentity_t* ignore );
void		G_AddClientSpawn				( gentity_t* ent, team_t team );

//
// g_svcmds.c
//
bool	ConsoleCommand					( void );

//
// g_weapon.c
//
gentity_t*	G_FireWeapon					( gentity_t *ent, attackType_t attack );
gentity_t*	G_FireProjectile				( gentity_t *ent, weapon_t weapon, attackType_t attack, int projectileLifetime, int flags );

//
// p_hud.c
//
void		MoveClientToIntermission		( gentity_t *client );
void		DeathmatchScoreboardMessage		( gentity_t *client );

//
// g_cmds.c
//

//
// g_main.c
//
void		FindIntermissionPoint				( void );
void		G_RunThink							( gentity_t *ent );
void 	G_LogPrintf							( const char *fmt, ... );
void		SendScoreboardMessageToAllClients	( void );

//
// g_client.c
//
char*		ClientConnect						( int clientNum, bool firstTime, bool isBot );
void		ClientUserinfoChanged				( int clientNum, userinfo *userInfo );
void		ClientDisconnect					( int clientNum );
void		ClientBegin							( int clientNum );
void		ClientCommand						( int clientNum );
gspawn_t*	G_SelectRandomSpawnPoint			( team_t team );
int			G_GametypeCommand					( int cmd, int arg0, int arg1, int arg2, int arg3, int arg4 );

//
// g_active.c
//
void G_CheckClientTimeouts	( gentity_t *ent );
void G_CheckClientTeamkill	( gentity_t *ent );
void ClientThink			( int clientNum );
void ClientEndFrame			( gentity_t *ent );
void G_RunClient			( gentity_t *ent );

//
// g_team.c
//
bool OnSameTeam( gentity_t *ent1, gentity_t *ent2 );

//
// g_session.c
//
void G_ReadSessionData( gclient_t *client );
void G_InitSessionData( gclient_t *client, char *userinfo );

void G_InitWorldSession( void );
void G_WriteSessionData( void );

//
// g_bot.c
//
bool G_DoesMapSupportGametype ( const char* gametype );
void G_LoadArenas ( void );

//
// g_gametype.c
//
gentity_t*	G_SpawnGametypeItem					( const char* pickup_name, bool dropped );
bool	G_ParseGametypeFile					( void );
void		G_ResetGametype						( void );
void		G_ResetGametypeItem					( gitem_t* item );
void		G_DropGametypeItems(gentity_t* self, int delayPickup);

#include "g_team.h" // teamplay specific stuff


extern	level_locals_t	level;
extern	gentity_t		g_entities[MAX_GENTITIES];

#define	FOFS(x) ((int)&(((gentity_t *)0)->x))

extern	vmCvar_t	g_gametype;
extern	vmCvar_t	g_dedicated;
extern	vmCvar_t	g_cheats;
extern	vmCvar_t	g_maxclients;			// allow this many total, including spectators
extern	vmCvar_t	g_maxGameClients;		// allow this many active
extern	vmCvar_t	g_restarted;

extern	vmCvar_t	g_logHits;

extern	vmCvar_t	g_dmflags;
extern	vmCvar_t	g_scorelimit;
extern	vmCvar_t	g_timelimit;
extern	vmCvar_t	g_friendlyFire;
extern	vmCvar_t	g_password;
extern	vmCvar_t	g_needpass;
extern	vmCvar_t	g_gravity;
extern	vmCvar_t	g_speed;
extern	vmCvar_t	g_knockback;
extern	vmCvar_t	g_forcerespawn;
extern	vmCvar_t	g_inactivity;
extern	vmCvar_t	g_debugMove;
extern	vmCvar_t	g_debugAlloc;
extern	vmCvar_t	g_debugDamage;
extern	vmCvar_t	g_weaponRespawn;
extern	vmCvar_t	g_backpackRespawn;
extern	vmCvar_t	g_synchronousClients;
extern	vmCvar_t	g_motd;
extern	vmCvar_t	g_warmup;
extern	vmCvar_t	g_doWarmup;
extern	vmCvar_t	g_allowVote;
extern	vmCvar_t	g_voteDuration;
extern	vmCvar_t	g_failedVoteDelay;
extern	vmCvar_t	g_teamAutoJoin;
extern	vmCvar_t	g_teamForceBalance;
extern	vmCvar_t	g_banIPs;
extern	vmCvar_t	g_filterBan;
extern	vmCvar_t	g_smoothClients;
extern	vmCvar_t	pmove_fixed;
extern	vmCvar_t	pmove_msec;
extern	vmCvar_t	g_rankings;
extern	vmCvar_t	g_respawnInterval;
extern	vmCvar_t	g_respawnInvulnerability;
extern	vmCvar_t	RMG;
extern  vmCvar_t	g_debugRMG;
extern	vmCvar_t	g_timeouttospec;
extern  vmCvar_t	g_roundtimelimit;
extern  vmCvar_t	g_timeextension;
extern  vmCvar_t	g_roundstartdelay;
extern  vmCvar_t	g_availableWeapons;
extern	vmCvar_t	g_forceFollow;
extern	vmCvar_t	g_followEnemy;
extern  vmCvar_t	g_mapcycle;
extern	vmCvar_t	g_pickupsDisabled;
extern	vmCvar_t	g_suicidePenalty;
extern	vmCvar_t	g_teamkillPenalty;
extern	vmCvar_t	g_teamkillDamageMax;
extern	vmCvar_t	g_teamkillDamageForgive;
extern	vmCvar_t	g_voiceFloodCount;
extern	vmCvar_t	g_voiceFloodPenalty;

// new cvars
extern	vmCvar_t	g_clientTMIUpdate;

void	trap_Print( const char *text );
void	trap_Error( const char *text );
int		trap_Milliseconds( void );
int		trap_Argc( void );
void	trap_Argv( int n, char *buffer, int bufferLength );
void	trap_Args( char *buffer, int bufferLength );
int		trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
void	trap_FS_Read( void *buffer, int len, fileHandle_t f );
void	trap_FS_Write( const void *buffer, int len, fileHandle_t f );
void	trap_FS_FCloseFile( fileHandle_t f );
int		trap_FS_GetFileList	( const char *path, const char *extension, char *listbuf, int bufsize );
void	trap_SendConsoleCommand( int exec_when, const char *text );
void	trap_Cvar_Register( vmCvar_t *cvar, const char *var_name, const char *value, int flags, float MinValue, float MaxValue );
void	trap_Cvar_Update( vmCvar_t *cvar );
void	trap_Cvar_Set( const char *var_name, const char *value );
int		trap_Cvar_VariableIntegerValue( const char *var_name );
float	trap_Cvar_VariableValue( const char *var_name );
void	trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void	trap_LocateGameData( gentity_t *gEnts, int numGEntities, int sizeofGEntity_t, playerState_t *gameClients, int sizeofGameClient );
void	trap_RMG_Init(int terrainID);
void	trap_DropClient( int clientNum, const char *reason );
void	trap_SendServerCommand( int clientNum, const char *text );
void	trap_SetConfigstring( int num, const char *string );
void	trap_GetConfigstring( int num, char *buffer, int bufferSize );
void	trap_GetUserinfo( int num, char *buffer, int bufferSize );
void	trap_SetUserinfo( int num, const char *buffer );
void	trap_GetServerinfo( char *buffer, int bufferSize );
void	trap_SetBrushModel( gentity_t *ent, const char *name );
void	trap_SetActiveSubBSP(int index);
void	trap_Trace( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
int		trap_PointContents( const vec3_t point, int passEntityNum );
bool trap_InPVS( const vec3_t p1, const vec3_t p2 );
bool trap_InPVSIgnorePortals( const vec3_t p1, const vec3_t p2 );
void	trap_AdjustAreaPortalState( gentity_t *ent, bool open );
bool trap_AreasConnected( int area1, int area2 );
void	trap_LinkEntity( gentity_t *ent );
void	trap_UnlinkEntity( gentity_t *ent );
int		trap_EntitiesInBox( const vec3_t mins, const vec3_t maxs, int *entityList, int maxcount );
bool trap_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );
int		trap_BotAllocateClient( void );
void	trap_BotFreeClient( int clientNum );
void	trap_GetUsercmd( int clientNum, usercmd_t *cmd );
bool	trap_GetEntityToken( char *buffer, int bufferSize );
void		trap_GetWorldBounds ( vec3_t mins, vec3_t maxs );

void	*trap_BotGetMemoryGame(int size);
void	trap_BotFreeMemoryGame(void *ptr);

void	trap_SnapVector( float *v );

int			trap_CM_RegisterTerrain(const char *config);

void		*trap_VM_LocalAlloc ( int size );
void		*trap_VM_LocalAllocUnaligned ( int size );			// WARNING!!!! USE WITH CAUTION!!! BEWARE OF DOG!!!
void		*trap_VM_LocalTempAlloc( int size );
void		trap_VM_LocalTempFree( int size );					// free must be in opposite order of allocation!
const char	*trap_VM_LocalStringAlloc ( const char *source );

void G_UpdateClientAntiLag	( gentity_t* ent );
void G_UndoAntiLag			( void );
void G_ApplyAntiLag			( gentity_t* ref, bool enlargeHitBox );
void G_RespawnClients(bool force, team_t team);
void gametype_trigger_touch(gentity_t *self, gentity_t *other, trace_t *trace);

void infoMsgToClients(int clientNum, const char *msg);

#include "gametype.h"
#include "gametype_inf.h"

extern Gametype *gtCore;

// rpm.cpp
void RPM_UpdateTMI();
void clientRefresh(gentity_t *ent);
void RPM_Obituary(gentity_t *target, gentity_t *attacker, int mod, attackType_t attack, int hitLocation);

// g_sounds.cpp
void ParseChatSounds();
string parseChatTokens(gentity_t *ent, const char *message, int *outSound);
void globalSound(int soundIndex);
bool isVoiceFlooded(gentity_t *ent);
#endif
