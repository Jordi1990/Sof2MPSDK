// Copyright (C) 2001-2002 Raven Software
//
#include "g_local.h"
#include <boost/algorithm/string/replace.hpp>
// g_client.c -- client functions that don't happen every frame

static vec3_t	playerMins = {-15, -15, -46};
static vec3_t	playerMaxs = {15, 15, 48};


/*
================
G_AddClientSpawn

adds a spawnpoint to the spawnpoint array using the given entity for
origin and angles as well as the team for filtering teams.
================
*/
void G_AddClientSpawn ( gentity_t* ent, team_t team )
{
	static vec3_t	mins = {-15,-15,-45};
	static vec3_t	maxs = {15,15,46};
	vec3_t			end;
	trace_t			tr;

	// Drop it to the ground, and if it starts solid just throw it out
	VectorCopy ( ent->s.origin, end );
	end[2] -= 1024;

	tr.fraction = 0.0f;
	trap_Trace ( &tr, ent->s.origin, mins, maxs, end, ent->s.number, MASK_SOLID );

	// We are only looking for terrain collisions at this point
	if ( tr.contents & CONTENTS_TERRAIN )
	{
		// If its in the ground then throw it awway
		if ( tr.startsolid )
		{
			return;
		}
		// Drop it down to the ground now
		else if ( tr.fraction < 1.0f && tr.fraction > 0.0f )
		{
			VectorCopy ( tr.endpos, ent->s.origin );
			ent->s.origin[2] += 1.0f;
			tr.startsolid = qfalse;
		}
	}

	if ( tr.startsolid )
	{
		Com_Printf ( S_COLOR_YELLOW "WARNING: gametype_player starting in solid at %.2f,%.2f,%.2f\n", ent->s.origin[0], ent->s.origin[1], ent->s.origin[2] );
	}

	level.spawns[level.spawnCount].team = team;

	// Release the entity and store the spawn in its own array
	VectorCopy ( ent->s.origin, level.spawns[level.spawnCount].origin );
	VectorCopy ( ent->s.angles, level.spawns[level.spawnCount].angles );

	// Increase the spawn count
	level.spawnCount++;
}

/*QUAKED info_player_deathmatch (1 0 1) (-16 -16 -46) (16 16 48) initial
potential spawning position for deathmatch games.
The first time a player enters the game, they will be at an 'initial' spot.
Targets will be fired when someone spawns in on them.
*/
void SP_info_player_deathmatch( gentity_t *ent ) 
{
	// Cant take any more spawns!!
	if ( level.spawnCount >= MAX_SPAWNS )
	{
		G_FreeEntity ( ent );
		return;
	}

	G_AddClientSpawn ( ent, TEAM_FREE );

	G_FreeEntity ( ent );
}

/*
================
G_SpotWouldTelefrag
================
*/
bool G_SpotWouldTelefrag( gspawn_t* spawn ) 
{
	int			num;
	int			touch[MAX_GENTITIES];
	vec3_t		mins, maxs;

	VectorAdd( spawn->origin, playerMins, mins );
	VectorAdd( spawn->origin, playerMaxs, maxs );
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for (int i=0 ; i<num ; i++) 
	{
		gentity_t *hit = &g_entities[touch[i]];

		if ( hit->client) 
		{
			if ( G_IsClientSpectating ( hit->client ) )
			{
				continue;
			}

			if ( G_IsClientDead ( hit->client ) )
			{
				continue;
			}

			return true;
		}
	}

	return false;
}

/*
================
G_SelectRandomSpawnPoint

go to a random point that doesn't telefrag
================
*/
gspawn_t* G_SelectRandomSpawnPoint ( team_t team ) 
{	
	int			count;
	int			tfcount;
	gspawn_t	*spawns[MAX_SPAWNS];
	gspawn_t	*tfspawns[MAX_SPAWNS];

	count = 0;
	tfcount = 0;

	for (int i = 0; i < level.spawnCount; i ++ )
	{
		gspawn_t* spawn = &level.spawns[i];

		if ( team != -1 && team != spawn->team )
		{
			continue;
		}

		if ( G_SpotWouldTelefrag( spawn ) ) 
		{
			tfspawns[tfcount++] = spawn;
			continue;
		}

		spawns[ count++ ] = spawn;
	}

	// no spots that won't telefrag so just pick one that will
	if ( !count )
	{	
		// No telefrag spots, just return NULL since there is no more to find
		if ( !tfcount ) 
		{
			return NULL;
		}

		// telefrag someone
		return tfspawns[ rand() % tfcount ];
	}

	return spawns[ rand() % count ];
}

/*
===========
G_SelectRandomSafeSpawnPoint

Select a random spawn point that is safe for the client to spawn at.  A safe spawn point 
is one that is at least a certain distance from another client.
============
*/
gspawn_t* G_SelectRandomSafeSpawnPoint ( team_t team, float safeDistance )
{
	gspawn_t*	spawns[MAX_SPAWNS];
	float		safeDistanceSquared;
	int			count = -1;

	// Square the distance for faster comparisons
	safeDistanceSquared = safeDistance * safeDistance;

	// Build a list of spawns
	for (int i = 0, count = 0; i < level.spawnCount; i ++ )
	{
		gspawn_t* spawn = &level.spawns[i];
		int		  j;

		// Ensure the team matches
		if ( team != -1 && team != spawn->team )
		{
			continue;
		}

		// Make sure this spot wont kill another player
		if ( G_SpotWouldTelefrag( spawn ) ) 
		{
			continue;
		}

		// Loop through connected clients 
		for ( j = 0; j < level.numConnectedClients && count < MAX_SPAWNS; j ++ )
		{
			gentity_t* other = &g_entities[level.sortedClients[j]];
			vec3_t	   diff;

			if ( other->client->pers.connected != CON_CONNECTED )
			{
				continue;
			}

			// Skip clients that are spectating or dead
			if ( G_IsClientSpectating ( other->client ) || G_IsClientDead ( other->client ) )
			{
				continue;
			}

			// on safe team, dont count this guy
			if ( level.gametypeData->teams && team == other->client->sess.team )
			{
				continue;
			}

			VectorSubtract ( other->r.currentOrigin, spawn->origin, diff );

			// Far enough away to qualify
			if ( VectorLengthSquared ( diff ) < safeDistanceSquared )
			{
				break;
			}
		}

		// If we didnt go through the whole list of clients then we must
		// have hit one that was too close.  But if we did go through teh whole
		// list then this spawn point is good to go
		if ( j >= level.numConnectedClients )
		{
			spawns[count++] = spawn;
		}	
	}

	// Nothing found, try it at half the safe distance
	if ( !count )
	{
		// Gotta stop somewhere
		if ( safeDistance / 2 < 250 )
		{
			return G_SelectRandomSpawnPoint ( team );
		}
		else
		{
			return G_SelectRandomSafeSpawnPoint ( team, safeDistance / 2 );
		}
	}

	// Spawn them at one of the spots
	return spawns[ rand() % count ];
}

/*
===========
G_SelectSpectatorSpawnPoint
============
*/
gspawn_t* G_SelectSpectatorSpawnPoint( void ) 
{
	static gspawn_t spawn;

	FindIntermissionPoint();

	VectorCopy( level.intermission_origin, spawn.origin );
	VectorCopy( level.intermission_angle, spawn.angles );

	return &spawn;
}

// Reserves a few spots for bodys in the entities structure
void G_InitBodyQueue (void) 
{
	int			max;

	if ( level.gametypeData->respawnType == RT_NONE )
	{
		level.bodySinkTime = 0;
		max = BODY_QUEUE_SIZE_MAX;
	}
	else
	{
		level.bodySinkTime = BODY_SINK_DELAY;
		max = BODY_QUEUE_SIZE;
	}

	for (level.bodyQueSize = 0; level.bodyQueSize < max && level.bodyQueSize < level.maxclients; ++level.bodyQueSize) 
	{
		level.bodyQue[level.bodyQueSize] = G_Spawn();
		level.bodyQue[level.bodyQueSize]->classname = "bodyque";
		level.bodyQue[level.bodyQueSize]->neverFree = true;
	}
}

/*
=============
BodySink

After sitting around for five seconds, fall into the ground and dissapear
=============
*/
void BodySink( gentity_t *ent ) 
{
	if ( level.time - ent->timestamp > level.bodySinkTime + BODY_SINK_TIME ) 
	{
		// the body ques are never actually freed, they are just unlinked
		trap_UnlinkEntity( ent );
		ent->physicsObject = false;
		return;	
	}

	ent->s.eFlags |= EF_NOSHADOW;

	ent->nextthink = level.time + 100;
	ent->s.pos.trBase[2] -= 1;
}

/*
=============
CopyToBodyQue

A player is respawning, so make an entity that looks
just like the existing corpse to leave behind.
=============
*/
void CopyToBodyQue( gentity_t *ent, int hitLocation, vec3_t direction ) 
{
	gentity_t	*body;
	int			contents;
	int			parm;

	trap_UnlinkEntity (ent);

	// if client is in a nodrop area, don't leave the body
	contents = trap_PointContents( ent->r.currentOrigin, -1 );
	if ( contents & CONTENTS_NODROP ) 
	{
		return;
	}

	// grab a body que and cycle to the next one
	body = level.bodyQue[ level.bodyQueIndex ];
	level.bodyQueIndex = (level.bodyQueIndex + 1) % level.bodyQueSize;

	trap_UnlinkEntity (body);

	body->s					= ent->s;
	body->s.eType			= ET_BODY;
	body->s.eFlags			= EF_DEAD;
	body->s.gametypeitems	= 0;
	body->s.loopSound		= 0;
	body->s.number			= body - g_entities;
	body->timestamp			= level.time;
	body->physicsObject		= true;
	body->physicsBounce		= 0;
	body->s.otherEntityNum  = ent->s.clientNum;

	if ( body->s.groundEntityNum == ENTITYNUM_NONE ) 
	{
		body->s.pos.trType = TR_GRAVITY;
		body->s.pos.trTime = level.time;
		VectorCopy( ent->client->ps.velocity, body->s.pos.trDelta );
	} 
	else 
	{
		body->s.pos.trType = TR_STATIONARY;
	}
	
	body->s.event = 0;

	parm  = (DirToByte( direction )&0xFF);
	parm += (hitLocation<<8);
	G_AddEvent(body, EV_BODY_QUEUE_COPY, parm);

	body->r.svFlags = ent->r.svFlags | SVF_BROADCAST;
	VectorCopy (ent->r.mins, body->r.mins);
	VectorCopy (ent->r.maxs, body->r.maxs);
	VectorCopy (ent->r.absmin, body->r.absmin);
	VectorCopy (ent->r.absmax, body->r.absmax);

	body->s.torsoAnim = body->s.legsAnim = ent->client->ps.legsAnim & ~ANIM_TOGGLEBIT;

	body->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	body->r.contents = 0; // CONTENTS_CORPSE;
	body->r.ownerNum = ent->s.number;

	if ( level.bodySinkTime )
	{
		body->nextthink = level.time + level.bodySinkTime;
		body->think = BodySink;
		body->s.time2 = 0;
	}
	else
	{
		// Store the time the body was spawned so the client can make them
		// dissapear if need be.
		body->s.time2 = level.time;
	}

	body->die = body_die;
	body->takedamage = true;

	body->s.apos.trBase[PITCH] = 0;

	body->s.pos.trBase[2] = ent->client->ps.origin[2];
	VectorCopy ( body->s.pos.trBase, body->r.currentOrigin );

	trap_LinkEntity (body);
}

//======================================================================


/*
==================
SetClientViewAngle

==================
*/
void SetClientViewAngle( gentity_t *ent, vec3_t angle ) 
{
	// set the delta angle
	for (int i=0 ; i<3 ; i++) 
	{
		int		cmdAngle;

		cmdAngle = ANGLE2SHORT(angle[i]);
		ent->client->ps.delta_angles[i] = cmdAngle - ent->client->pers.cmd.angles[i];
	}

	VectorCopy( angle, ent->s.angles );
	VectorCopy (ent->s.angles, ent->client->ps.viewangles);
}

/*
================
G_SetRespawnTimer
================
*/
void G_SetRespawnTimer ( gentity_t* ent )
{
	// Start a new respawn interval if the old one has passed
	if ( level.time > level.gametypeRespawnTime[ent->client->sess.team] )
	{
		level.gametypeRespawnTime[ent->client->sess.team] = level.time + g_respawnInterval.integer * 1000;
	}

	// start the interval if its not already started
	ent->client->ps.respawnTimer = level.gametypeRespawnTime[ent->client->sess.team] + 1000;
}

/*
================
respawn
================
*/
void respawn( gentity_t *ent )
{
	gentity_t	*tent;
	bool	ghost = false;

	// No respawning when intermission is queued
	if ( level.intermissionQueued )
	{
		return;
	}

	// When we get here the user has just accepted their fate and now
	// needs to wait for the ability to respawn
	switch ( level.gametypeData->respawnType )
	{
		case RT_INTERVAL:
			G_SetRespawnTimer ( ent );
			ghost = true;
			break;

		case RT_NONE:

			// Turn into a ghost
			ghost = true;
			break;
	}

	// If they are a ghost then give a health point, but dont respawn
	if ( ghost )
	{
		G_StartGhosting ( ent );
		return;
	}

	trap_UnlinkEntity (ent);
	ClientSpawn(ent);

	// Add a teleportation effect.
	tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_IN );
	tent->s.clientNum = ent->s.clientNum;
}

/*
================
G_IsClientDead

Returns true if the client is dead and false if not
================
*/
bool G_IsClientDead ( const gclient_t* client )
{
	if ( client->ps.stats[STAT_HEALTH] <= 0 )
	{
		return true;
	}

	if ( client->ps.pm_type == PM_DEAD )
	{
		return true;
	}

	if ( client->sess.ghost )
	{
		return true;
	}

	return false;
}

/*
================
G_IsClientSpectating

Returns true if the client is spectating and false if not
================
*/
bool G_IsClientSpectating ( gclient_t* client )
{
	if ( client->pers.connected != CON_CONNECTED )
	{
		return true;
	}

	if ( client->sess.team == TEAM_SPECTATOR )
	{
		return true;
	}

	if ( client->sess.ghost )
	{
		return true;
	}

	return false;
}

/*
================
TeamCount

Returns number of players on a team
================
*/
int TeamCount( int ignoreClientNum, team_t team, int *alive ) 
{
	int		count = 0;

	if ( alive )
	{
		*alive = 0;
	}

	for (int i = 0 ; i < level.maxclients ; i++ ) 
	{
		if ( i == ignoreClientNum ) 
		{
			continue;
		}
		if ( level.clients[i].pers.connected == CON_DISCONNECTED ) 
		{
			continue;
		}
	
		if ( level.clients[i].sess.team == team ) 
		{
			if ( !level.clients[i].sess.ghost && alive )
			{
				(*alive)++;
			}

			count++;
		}
	}

	return count;
}

/*
================
PickTeam
================
*/
team_t PickTeam( int ignoreClientNum ) 
{
	int		counts[TEAM_NUM_TEAMS];

	counts[TEAM_BLUE] = TeamCount( ignoreClientNum, TEAM_BLUE, NULL );
	counts[TEAM_RED] = TeamCount( ignoreClientNum, TEAM_RED, NULL );

	if ( counts[TEAM_BLUE] > counts[TEAM_RED] ) 
	{
		return TEAM_RED;
	}

	if ( counts[TEAM_RED] > counts[TEAM_BLUE] ) 
	{
		return TEAM_BLUE;
	}

	// equal team count, so join the team with the lowest score
	if ( level.teamScores[TEAM_BLUE] > level.teamScores[TEAM_RED] ) 
	{
		return TEAM_RED;
	}

	return TEAM_BLUE;
}

/*
===========
Updates the clients current outfittin
===========
*/
void G_UpdateOutfitting ( int clientNum )
{
	gentity_t	*ent;
	gclient_t	*client;
	int			ammoIndex;
	int			idle;

	int			equipWeapon;
	int			equipWeaponGroup;

	ent    = g_entities + clientNum;
	client = ent->client;

	// No can do if 
	if ( client->noOutfittingChange )
	{
		return;
	}

	// Clear all ammo, clips, and weapons
	client->ps.stats[STAT_WEAPONS] = 0;
	memset ( client->ps.ammo, 0, sizeof(client->ps.ammo) );
	memset ( client->ps.clip, 0, sizeof(client->ps.clip) );

	// Everyone gets some knives
	client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_KNIFE );
	ammoIndex=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].ammoIndex;
	client->ps.clip[ATTACK_NORMAL][WP_KNIFE]=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].clipSize;
	client->ps.firemode[WP_KNIFE] = BG_FindFireMode ( WP_KNIFE, ATTACK_NORMAL, WP_FIREMODE_AUTO );

	if ( BG_IsWeaponAvailableForOutfitting ( WP_KNIFE, 2 ) )
	{
		client->ps.ammo[ammoIndex]=ammoData[ammoIndex].max;
	}

	equipWeapon = WP_KNIFE;
	equipWeaponGroup = OUTFITTING_GROUP_KNIFE;

	// Give all the outfitting groups to the player		
	for ( int group = 0; group < OUTFITTING_GROUP_ACCESSORY; group ++ )
	{
		gitem_t* item;
		int		 ammoIndex;
		
		// Nothing available in this group
		if ( client->pers.outfitting.items[group] == -1 )
		{
			continue;
		}

		// Grab the item that represents the weapon
		item = &bg_itemlist[bg_outfittingGroups[group][client->pers.outfitting.items[group]]];

		client->ps.stats[STAT_WEAPONS] |= (1 << item->giTag);
		ammoIndex = weaponData[item->giTag].attack[ATTACK_NORMAL].ammoIndex;
		client->ps.ammo[ammoIndex] += weaponData[item->giTag].attack[ATTACK_NORMAL].extraClips * weaponData[item->giTag].attack[ATTACK_NORMAL].clipSize;
		client->ps.clip[ATTACK_NORMAL][item->giTag] = weaponData[item->giTag].attack[ATTACK_NORMAL].clipSize;

		// Lower group numbers are bigger guns
		if ( group < equipWeaponGroup )
		{
			equipWeaponGroup = group;
			equipWeapon = item->giTag;
		}

		// alt-fire ammo
		ammoIndex = weaponData[item->giTag].attack[ATTACK_ALTERNATE].ammoIndex;
		if ( weaponData[item->giTag].attack[ATTACK_ALTERNATE].fireAmount && AMMO_NONE != ammoIndex )
		{
			client->ps.clip[ATTACK_ALTERNATE][item->giTag] = weaponData[item->giTag].attack[ATTACK_ALTERNATE].clipSize;
			client->ps.ammo[ammoIndex] += weaponData[item->giTag].attack[ATTACK_ALTERNATE].extraClips * weaponData[item->giTag].attack[ATTACK_ALTERNATE].clipSize;
		}

		// Set the default firemode for this weapon
		if ( client->ps.firemode[item->giTag] == WP_FIREMODE_NONE )
		{
			client->ps.firemode[item->giTag] = BG_FindFireMode ( (weapon_t)item->giTag, ATTACK_NORMAL, WP_FIREMODE_AUTO );
		}
	}

	client->ps.weapon = equipWeapon;
	client->ps.weaponstate = WEAPON_READY; //WEAPON_SPAWNING;
	client->ps.weaponTime = 0;
	client->ps.weaponAnimTime = 0;

	// Bot clients cant use the spawning state
#ifdef _SOF2_BOTS
	if ( ent->r.svFlags & SVF_BOT )
	{
		client->ps.weaponstate = WEAPON_READY;
	}
#endif

	// Default to auto (or next available fire mode).
	BG_GetInviewAnim(client->ps.weapon,"idle",&idle);
	client->ps.weaponAnimId = idle;
	client->ps.weaponAnimIdChoice = 0;
	client->ps.weaponCallbackStep = 0;

	// Armor?
	client->ps.stats[STAT_ARMOR]   = 0;
	client->ps.stats[STAT_GOGGLES] = GOGGLES_NONE;
	switch ( bg_outfittingGroups[OUTFITTING_GROUP_ACCESSORY][client->pers.outfitting.items[OUTFITTING_GROUP_ACCESSORY]] )
	{
		default:
		case MODELINDEX_ARMOR:
			client->ps.stats[STAT_ARMOR] = MAX_HEALTH;
			break;

		case MODELINDEX_THERMAL:
			client->ps.stats[STAT_GOGGLES] = GOGGLES_INFRARED;
			break;

		case MODELINDEX_NIGHTVISION:
			client->ps.stats[STAT_GOGGLES] = GOGGLES_NIGHTVISION;
			break;
	}

	// Stuff which grenade is being used into stats for later use by
	// the backpack code
	client->ps.stats[STAT_OUTFIT_GRENADE] = bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_GRENADE][client->pers.outfitting.items[OUTFITTING_GROUP_GRENADE]]].giTag;
}

string parseName(const string &name1)
{
	string name = string(name1);
	boost::replace_all(name, "^^", "");// replace double color tags
	boost::replace_all(name, "  ", "");// remove double spaces
	if (name.length() < 1)
		throw "Error while parsing name";
	if (name.back() == ' ' || name.back() == '\t')
		name.pop_back(); // delete last whitespace
	// after a ^3 there always has to be another character
	for (unsigned int i = 0; i<name.length() - 1; ++i){
		if (name[i] == '^' && !(name[i + 1] >= 33 && name[i + 1] < 128))
			throw "Invalid color specified";
	}
	// define a max length
	if (name.length() > 64)
		throw "Maximum name length exceeded";
	else if (name.length() < 2)
		throw "Please use a name with more than 1 character";
	return name;
}

/*
===========
ClientUserInfoChanged

Called from ClientConnect when the player first connects and
directly by the server system when the player updates a userinfo variable.

The game can override any of the settings and call trap_SetUserinfo
if desired.
============
*/
void ClientUserinfoChanged( int clientNum, userinfo *userInfo ) 
{
	gentity_t	*ent;
	int			team;
	gclient_t	*client;
	string oldname;
	TIdentity	*oldidentity;

	ent = g_entities + clientNum;
	client = ent->client;
	if (!userInfo){ // fetch again
		userInfo = new userinfo(clientNum);
	}

	ent->client->pers.predictItemPickup = userInfo->cg_predictItems;
	ent->client->pers.autoReload = userInfo->cg_autoReload;
	ent->client->pers.antiLag = userInfo->cg_antiLag;

	oldname = client->pers.netname;

	//set name
	try{
		client->pers.netname = parseName(userInfo->name);
	}
	catch (const char *err){
		client->pers.netname = "Unnamed Player";
		if (oldname != client->pers.netname)
			trap_SendServerCommand(clientNum, va("print \"^_[Error] ^7Rename failed: %s.\n\"", err));
	}
	if ( client->sess.team == TEAM_SPECTATOR ) 
	{
		if ( client->sess.spectatorState == SPECTATOR_SCOREBOARD ) 
		{
			client->pers.netname = "scoreboard";
		}
	}

	team = client->sess.team;

	// Enforce the identities
	oldidentity = client->pers.identity;

	if( level.gametypeData->teams ) 
	{
		// Lookup the identity by name and if it cant be found then pick a random one
		client->pers.identity = BG_FindIdentity ( userInfo->team_identity.c_str() );

		if ( team != TEAM_SPECTATOR )
		{
			// No identity or a team mismatch means they dont get to be that skin
			if ( !client->pers.identity || strcmp ( level.gametypeTeam[team], client->pers.identity->mTeam ) )
			{
				// Get first matching team identity
				client->pers.identity = BG_FindTeamIdentity ( level.gametypeTeam[team], -1 );
			}
		}
		else
		{	
			// Spectators are going to have to choose one of the two team skins and
			// the chance of them having the proper one in team_identity is slim, so just
			// give them a model they may use later
			client->pers.identity = BG_FindTeamIdentity ( level.gametypeTeam[TEAM_RED], 0 );
		}
	} 
	else 
	{
		// Lookup the identity by name and if it cant be found then pick a random one
		client->pers.identity = BG_FindIdentity ( userInfo->identity.c_str() );
	}

	// If the identity wasnt in the list then just give them the first identity.  We could
	// be fancy here and give them a random one, but this way you get less unwanted models 
	// loaded
	if ( !client->pers.identity  )
	{
		client->pers.identity = &bg_identities[0];
	}

	// Report the identity change
	if ( client->pers.connected == CON_CONNECTED ) 
	{
		if ( client->pers.identity && oldidentity && client->pers.identity != oldidentity && team != TEAM_SPECTATOR )
		{
			infoMsgToClients(-1, va("%s ^7has changed identities", client->pers.netname.c_str()));
		}

		// If the client is changing their name then handle some delayed name changes
		if ( client->pers.netname.compare(oldname) ) 
		{
			// Dont let them change their name too much
			if ( level.time - client->pers.netnameTime < 5000 )
			{
				infoMsgToClients(client->ps.clientNum, "You must wait 5 seconds before changing your name again");
				client->pers.netname = oldname;
			}
			// If they are a ghost or spectating in an inf game their name is deferred
			else if ( level.gametypeData->respawnType == RT_NONE && (client->sess.ghost || G_IsClientDead ( client ) ) )
			{
				infoMsgToClients(client->ps.clientNum, "Name changes while dead will be deferred until you spawn again");
				strcpy ( client->pers.deferredname, client->pers.netname.c_str() );
				client->pers.netname = oldname;
			}
			else
			{
				infoMsgToClients(-1, va("%s ^7renamed to %s", oldname.c_str(), client->pers.netname.c_str()));
				client->pers.netnameTime = level.time;
			}
		}
	}

	// Outfitting if pickups are disabled
	if ( level.pickupsDisabled )
	{
		// Parse out the new outfitting
		BG_DecompressOutfitting ( userInfo->outfitting.c_str(), &client->pers.outfitting );
		G_UpdateOutfitting ( clientNum );
	}

	// send over a subset of the userinfo keys so other clients can
	// print scoreboards, display models, and play custom sounds
	char s[1024];
	if (ent->r.svFlags & SVF_BOT)
		sprintf_s(s, 1024, "n\\%s\\t\\%i\\identity\\%s\\skill\\5", ent->client->pers.netname.c_str(), ent->client->sess.team, ent->client->pers.identity->mName);
	else
		sprintf_s(s, 1024, "n\\%s\\t\\%i\\identity\\%s", ent->client->pers.netname.c_str(), ent->client->sess.team, ent->client->pers.identity->mName);

	trap_SetConfigstring( CS_PLAYERS+clientNum, s );

	G_LogPrintf( "ClientUserinfoChanged: %i %s\n", clientNum, s );
}


/*
===========
ClientConnect

Called when a player begins connecting to the server.
Called again for every map change or map restart.

The session information will be valid after exit.

Return NULL if the client should be allowed, otherwise return
a string with the reason for denial.

Otherwise, the client will be sent the current gamestate
and will eventually get to ClientBegin.

firstTime will be true the very first time a client connects
to the server machine, but false on map changes and map
restarts.
============
*/
char *ClientConnect( int clientNum, bool firstTime, bool isBot ) 
{
	char		userinfoBuf[MAX_INFO_STRING];
	gentity_t	*ent = &g_entities[ clientNum ];

	trap_GetUserinfo(clientNum, userinfoBuf, MAX_INFO_STRING);

	userinfo userInfo(clientNum); // exception will be catched one call up so connect returns the reason why it failed
	bool isLocal = userInfo.ip.compare("localhost") != 0;

	if (!isBot && !isLocal)
	{
		// check for a password
		if (g_password.string[0] && strcmp(g_password.string, "none") != 0 &&
			strcmp(g_password.string, userInfo.password.c_str()) != 0)
		{
			return va("Invalid password: %s", userInfo.password.c_str());
		}
	}

	// they can connect
	ent->client = level.clients + clientNum;
	gclient_t *client = ent->client;

	memset( client, 0, sizeof(*client) );
	client->pers.connected = CON_CONNECTING;
	client->sess.team = TEAM_SPECTATOR;
	client->pers.localClient = isLocal;

	// read or initialize the session data
	if ( firstTime || level.newSession ) 
	{
		//G_InitSessionData( client, userinfo );
	}
	
	//G_ReadSessionData( client );

	// get and distribute relevent paramters
	G_LogPrintf( "ClientConnect: %i\n", clientNum );
	ClientUserinfoChanged( clientNum, &userInfo);

	// don't do the "xxx connected" messages if they were caried over from previous level
	if ( firstTime ) 
	{
		infoMsgToClients(-1, va("%s ^7is connecting..", client->pers.netname.c_str()));
	}

	// Broadcast team change if not going to spectator
	if ( level.gametypeData->teams && client->sess.team != TEAM_SPECTATOR ) 
	{
		BroadcastTeamChange( client, -1 );
	}

	// count current clients and rank for scoreboard
	CalculateRanks();

	// Make sure they are unlinked
	trap_UnlinkEntity ( ent );

	return NULL;
}

/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the level.  This will happen every level load,
and on transition between teams, but doesn't happen on respawns
============
*/
void ClientBegin( int clientNum ) 
{
	gentity_t	*ent;
	gclient_t	*client;
	gentity_t	*tent;
	int			flags;
	int			spawncount;

	ent = g_entities + clientNum;

	client = level.clients + clientNum;

	if ( ent->r.linked ) 
	{
		trap_UnlinkEntity( ent );
	}

	// Run a gametype check just in case the game hasnt started yet
	if ( !level.gametypeStartTime )
	{
		gtCore->update();
	}

	G_InitGentity( ent );
	ent->touch = 0;
	ent->pain = 0;
	ent->client = client;

	client->pers.connected = CON_CONNECTED;
	client->pers.enterTime = level.time;
	client->pers.teamState.state = TEAM_BEGIN;

	// save eflags around this, because changing teams will
	// cause this to happen with a valid entity, and we
	// want to make sure the teleport bit is set right
	// so the viewpoint doesn't interpolate through the
	// world to the new position
	flags = client->ps.eFlags;
	spawncount = client->ps.persistant[PERS_SPAWN_COUNT];
	memset( &client->ps, 0, sizeof( client->ps ) );
	client->ps.eFlags = flags;
	client->ps.persistant[PERS_SPAWN_COUNT] = spawncount;

	// locate ent at a spawn point
	ClientSpawn( ent );

	if ( client->sess.team != TEAM_SPECTATOR ) 
	{
		// send event
		tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_IN );
		tent->s.clientNum = ent->s.clientNum;

		infoMsgToClients(-1, va("%s ^7entered the game", client->pers.netname.c_str()));
	}
	
	G_LogPrintf( "ClientBegin: %i\n", clientNum );

	// count current clients and rank for scoreboard
	CalculateRanks();
}

/*
===========
G_SelectClientSpawnPoint

Selects a spawn point for the given client entity
============
*/
gspawn_t* G_SelectClientSpawnPoint ( gentity_t* ent )
{
	gclient_t*	client = ent->client;
	gspawn_t*	spawnPoint;

		// find a spawn point
	// do it before setting health back up, so farthest
	// ranging doesn't count this client
	if ( client->sess.team == TEAM_SPECTATOR ) 
	{
		spawnPoint = G_SelectSpectatorSpawnPoint ( );
	}
	else 
	{
		if ( level.gametypeData->teams && level.gametypeData->respawnType != RT_NORMAL )
		{
			// Dont bother selecting a safe spawn on non-respawn games, the map creator should
			// have done this for us.
			if ( level.gametypeData->respawnType == RT_NONE )
			{
				spawnPoint = G_SelectRandomSpawnPoint ( ent->client->sess.team );
			}
			else
			{
				spawnPoint = G_SelectRandomSafeSpawnPoint ( ent->client->sess.team, 1500 );
			}

			if ( !spawnPoint )
			{
				// don't spawn near other players if possible
				spawnPoint = G_SelectRandomSpawnPoint ( ent->client->sess.team );
			}

			// Spawn at any deathmatch spawn, telefrag if needed
			if ( !spawnPoint )
			{
				spawnPoint = G_SelectRandomSpawnPoint ( TEAM_FREE );
			}
		}
		else
		{
			// Try deathmatch spawns first
			spawnPoint = G_SelectRandomSafeSpawnPoint ( TEAM_FREE, 1500 );

			// If none found use any spawn
			if ( !spawnPoint )
			{
				spawnPoint = G_SelectRandomSafeSpawnPoint ( (team_t)-1, 1500 );
			}

			// Spawn at any deathmatch spawn, telefrag if needed
			if ( !spawnPoint )
			{
				spawnPoint = G_SelectRandomSpawnPoint ( TEAM_FREE );
			}

			// Spawn at any gametype spawn, telefrag if needed
			if ( !spawnPoint )
			{
				spawnPoint = G_SelectRandomSpawnPoint ( (team_t)-1 );
			}
		}		
	}

	return spawnPoint;
}

/*
===========
ClientSpawn

Called every time a client is placed fresh in the world:
after the first ClientBegin, and after each respawn
Initializes all non-persistant parts of playerState
============
*/

void ClientSpawn(gentity_t *ent) 
{
	int					index;
	vec3_t				spawn_origin;
	vec3_t				spawn_angles;
	gclient_t			*client;
	clientPersistant_t	saved;
	clientSession_t		savedSess;
	int					persistant[MAX_PERSISTANT];
	gspawn_t			*spawnPoint;
	int					flags;
	int					savedPing;
	int					eventSequence;
	int					start_ammo_type;
	int					ammoIndex;
	int					idle;

	index  = ent - g_entities;
	client = ent->client;

	// Where do we spawn?
	spawnPoint = G_SelectClientSpawnPoint ( ent );
	if ( spawnPoint )
	{
		VectorCopy ( spawnPoint->angles, spawn_angles );
		VectorCopy ( spawnPoint->origin, spawn_origin );
		spawn_origin[2] += 9;
	}
	else
	{
		SetTeam ( ent, "s", NULL );
		return;
	}

	client->pers.teamState.state = TEAM_ACTIVE;

	// toggle the teleport bit so the client knows to not lerp
	// and never clear the voted flag
	flags = ent->client->ps.eFlags & (EF_TELEPORT_BIT | EF_VOTED);
	flags ^= EF_TELEPORT_BIT;

	// clear everything but the persistant data
	saved = client->pers;
	savedSess = client->sess;
	savedPing = client->ps.ping;
	for ( int i = 0 ; i < MAX_PERSISTANT ; i++ ) 
	{
		persistant[i] = client->ps.persistant[i];
	}
	eventSequence = client->ps.eventSequence;

	memset (client, 0, sizeof(*client)); 

	client->pers = saved;
	client->sess = savedSess;
	client->ps.ping = savedPing;
	client->lastkilled_client = -1;

	for (int i = 0 ; i < MAX_PERSISTANT ; i++ ) 
	{
		client->ps.persistant[i] = persistant[i];
	}
	client->ps.eventSequence = eventSequence;
	
	// increment the spawncount so the client will detect the respawn
	client->ps.persistant[PERS_SPAWN_COUNT]++;
	client->ps.persistant[PERS_TEAM] = client->sess.team;
	client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;

	client->airOutTime = level.time + 12000;
	
	userinfo userInfo(client->ps.clientNum);

	// clear entity values
	client->ps.eFlags = flags;

	ent->s.groundEntityNum = ENTITYNUM_NONE;
	ent->client = &level.clients[index];
	ent->takedamage = true;
	ent->inuse = true;
	ent->classname = "player";
	ent->r.contents = CONTENTS_BODY;
	ent->clipmask = MASK_PLAYERSOLID;
	ent->die = player_die;
	ent->waterlevel = 0;
	ent->watertype = 0;
	ent->flags = 0;
	
	VectorCopy (playerMins, ent->r.mins);
	VectorCopy (playerMaxs, ent->r.maxs);

	client->ps.clientNum = index;

	// Bring back the saved firemodes
	memcpy ( client->ps.firemode, client->pers.firemode, sizeof(client->ps.firemode) );

	//give default weapons
	client->ps.stats[STAT_WEAPONS] = ( 1 << WP_NONE );

   	client->noOutfittingChange = false;

	// Give the client their weapons depending on whether or not pickups are enabled
	if ( level.pickupsDisabled )
	{
		G_UpdateOutfitting ( ent->s.number );

		// Prevent the client from picking up a whole bunch of stuff
		client->ps.pm_flags |= PMF_LIMITED_INVENTORY;
	}
	else
	{
		// Knife.
		client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_KNIFE );
		ammoIndex=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].ammoIndex;
		client->ps.ammo[ammoIndex]=ammoData[ammoIndex].max;
		client->ps.clip[ATTACK_NORMAL][WP_KNIFE]=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].clipSize;
		client->ps.firemode[WP_KNIFE] = BG_FindFireMode ( WP_KNIFE, ATTACK_NORMAL, WP_FIREMODE_AUTO );

		// Set up some weapons and ammo for the player.
		client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_USSOCOM_PISTOL );
		start_ammo_type = weaponData[WP_USSOCOM_PISTOL].attack[ATTACK_NORMAL].ammoIndex;
		client->ps.ammo[start_ammo_type] = weaponData[WP_USSOCOM_PISTOL].attack[ATTACK_NORMAL].clipSize;
		client->ps.clip[ATTACK_NORMAL][WP_USSOCOM_PISTOL] = weaponData[WP_USSOCOM_PISTOL].attack[ATTACK_NORMAL].clipSize;
		client->ps.firemode[WP_USSOCOM_PISTOL] = BG_FindFireMode ( WP_USSOCOM_PISTOL, ATTACK_NORMAL, WP_FIREMODE_AUTO );

		// alt-fire ammo
		start_ammo_type = weaponData[WP_USSOCOM_PISTOL].attack[ATTACK_ALTERNATE].ammoIndex;
		if (AMMO_NONE != start_ammo_type)
		{
			client->ps.ammo[start_ammo_type] = ammoData[start_ammo_type].max;
		}

		// Everyone gets full armor in deathmatch
		client->ps.stats[STAT_ARMOR] = MAX_HEALTH;
	}

	client->ps.stats[STAT_HEALTH] = ent->health = MAX_HEALTH;

	G_SetOrigin( ent, spawn_origin );
	VectorCopy( spawn_origin, client->ps.origin );

	// the respawned flag will be cleared after the attack and jump keys come up
	client->ps.pm_flags |= PMF_RESPAWNED;
	if ( client->pers.autoReload )
	{
		client->ps.pm_flags |= PMF_AUTORELOAD;
	}
	else
	{
		client->ps.pm_flags &= ~PMF_AUTORELOAD;
	}

	trap_GetUsercmd( client - level.clients, &ent->client->pers.cmd );
	SetClientViewAngle( ent, spawn_angles );

	if ( ent->client->sess.team != TEAM_SPECTATOR ) 
	{
		G_KillBox( ent );
		trap_LinkEntity (ent);

		// force the base weapon up
		if ( !level.pickupsDisabled )
		{
			client->ps.weapon = WP_USSOCOM_PISTOL;
			client->ps.weaponstate = WEAPON_RAISING;
			client->ps.weaponTime = 500;

			// Default to auto (or next available fire mode).
			client->ps.firemode[client->ps.weapon] = BG_FindFireMode ( (weapon_t)client->ps.weapon, ATTACK_NORMAL, WP_FIREMODE_AUTO );
			BG_GetInviewAnim(client->ps.weapon,"idle",&idle);					
			client->ps.weaponAnimId = idle;
			client->ps.weaponAnimIdChoice = 0;
			client->ps.weaponCallbackStep = 0;
		}
	}
	else
	{
		client->ps.weapon = WP_KNIFE;
		BG_GetInviewAnim(client->ps.weapon,"idle",&idle);
		client->ps.weaponAnimId = idle;
	}

	// don't allow full run speed for a bit
	client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
	client->ps.pm_time = 100;

	client->respawnTime = level.time;
	client->invulnerableTime = level.time;
	client->ps.eFlags |= EF_INVULNERABLE;
	client->inactivityTime = level.time + g_inactivity.integer * 1000;
	client->latched_buttons = 0;

	// set default animations
	client->ps.weaponstate = WEAPON_READY;
	client->ps.torsoAnim = -1;
	client->ps.legsAnim = LEGS_IDLE;

	// Not on a ladder
	client->ps.ladder = -1;

	// Not leaning
	client->ps.leanTime = LEAN_TIME;

	if ( level.intermissiontime ) 
	{
		MoveClientToIntermission( ent );
	} 

	// run a client frame to drop exactly to the floor,
	// initialize animations and other things
	client->ps.commandTime = level.time - 100;
	ent->client->pers.cmd.serverTime = level.time;
	ClientThink( ent-g_entities );

	// positively link the client, even if the command times are weird
	if ( ent->client->sess.team != TEAM_SPECTATOR ) 
	{
		BG_PlayerStateToEntityState( &client->ps, &ent->s, true );
		VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );
		trap_LinkEntity( ent );
	}

	// run the presend to set anything else
	ClientEndFrame( ent );

	// clear entity state values
	BG_PlayerStateToEntityState( &client->ps, &ent->s, true );

	// Frozen?
	if ( level.gametypeDelayTime > level.time )
	{
		ent->client->ps.stats[STAT_FROZEN] = level.gametypeDelayTime - level.time;
	}

	// Handle a deferred name change
	if ( client->pers.deferredname[0] )
	{
		infoMsgToClients(-1, va("%s ^7renamed to %s", client->pers.netname.c_str(), client->pers.deferredname));
		client->pers.netname = client->pers.deferredname;
		client->pers.deferredname[0] = '\0';
		client->pers.netnameTime = level.time;
		ClientUserinfoChanged ( client->ps.clientNum, &userInfo );
	}

	// Update the time when other people can join the game
	if ( !level.gametypeJoinTime && level.gametypeData->teams )
	{
		// As soon as both teams have people on them the counter starts
		if ( TeamCount ( -1, TEAM_RED, NULL ) && TeamCount ( -1, TEAM_BLUE, NULL ) )
		{
			level.gametypeJoinTime = level.time;
		}
	}
}


/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.

This should NOT be called directly by any game logic,
call trap_DropClient(), which will call this and do
server system housekeeping.
============
*/
void ClientDisconnect( int clientNum ) 
{
	gentity_t	*ent;

	ent = g_entities + clientNum;
	if ( !ent->client ) 
	{
		return;
	}

	// stop any following clients
	for (int i = 0 ; i < level.maxclients ; i++ ) 
	{
		if ( G_IsClientSpectating ( &level.clients[i] ) &&
			 level.clients[i].sess.spectatorState == SPECTATOR_FOLLOW &&
			 level.clients[i].sess.spectatorClient == clientNum ) 
		{
			G_StopFollowing( &g_entities[i] );
		}
	}

	// send effect if they were completely connected
	if ( ent->client->pers.connected == CON_CONNECTED && 
		 !G_IsClientSpectating ( ent->client )  &&
		 !G_IsClientDead ( ent->client ) )
	{
		gentity_t	*tent = G_TempEntity(ent->client->ps.origin, EV_PLAYER_TELEPORT_OUT);
		tent->s.clientNum = ent->s.clientNum;

		// Dont drop weapons
		ent->client->ps.stats[STAT_WEAPONS] = 0;

		// Get rid of things that need to drop
		TossClientItems( ent );
	}

	G_LogPrintf( "ClientDisconnect: %i\n", clientNum );

	trap_UnlinkEntity (ent);
	ent->s.modelindex = 0;
	ent->inuse = false;
	ent->classname = "disconnected";
	ent->client->pers.connected = CON_DISCONNECTED;
	ent->client->ps.persistant[PERS_TEAM] = TEAM_FREE;
	ent->client->sess.team = TEAM_FREE;

	trap_SetConfigstring( CS_PLAYERS + clientNum, "");

	CalculateRanks();
}

/*
===========
G_UpdateClientAnimations

Updates the animation information for the client
============
*/
void G_UpdateClientAnimations ( gentity_t* ent )
{
	gclient_t*	client = ent->client;
	
	// Check for anim change in the legs
	if ( client->legs.anim != ent->s.legsAnim )
	{
		client->legs.anim = ent->s.legsAnim;
		client->legs.animTime = level.time;
	}

	// Check for anim change in the torso
	if ( client->torso.anim != ent->s.torsoAnim )
	{
		client->torso.anim = ent->s.torsoAnim;
		client->torso.animTime = level.time;
	}

	// Force the legs and torso to stay aligned for now to ensure the client
	// and server are in sync with the angles.  
	// TODO: Come up with a way to keep these in sync on both client and server
	client->torso.yawing   = true;
	client->torso.pitching = true;
	client->legs.yawing    = true;

	// Calculate the real torso and leg angles
	BG_PlayerAngles ( ent->client->ps.viewangles, 
					  NULL, 
					  
					  ent->client->ghoulLegsAngles,
					  ent->client->ghoulLowerTorsoAngles,
					  ent->client->ghoulUpperTorsoAngles,
					  ent->client->ghoulHeadAngles,

					  (float)(ent->client->ps.leanTime - LEAN_TIME) / LEAN_TIME * LEAN_OFFSET,
					 
					  0, 
					  0, 
					  level.time,
					 
					  &client->torso,
					  &client->legs,
					 
					  level.time - level.previousTime, 
					 
					  client->ps.velocity,

					  false, 
					  ent->s.angles2[YAW],
					  NULL );
}

/*
===========
G_FindNearbyClient

Locates a client thats near the given origin
============
*/
gentity_t* G_FindNearbyClient ( vec3_t origin, team_t team, float radius, gentity_t* ignore )
{
	for (int i = 0; i < level.numConnectedClients; i ++ )
	{
		gentity_t* other = &g_entities[ level.sortedClients[i] ];
		vec3_t	   diff;

		if ( other->client->pers.connected != CON_CONNECTED )
		{
			continue;
		}

		if ( other == ignore )
		{
			continue;
		}

		if ( G_IsClientSpectating ( other->client ) || G_IsClientDead ( other->client ) )
		{
			continue;
		}

		if ( other->client->sess.team != team )
		{
			continue;
		}

		// See if this client is close enough to yell sniper
		VectorSubtract ( other->r.currentOrigin, origin, diff );
		if ( VectorLengthSquared ( diff ) < radius * radius )
		{
			return other;
		}
	}

	return NULL;
}
