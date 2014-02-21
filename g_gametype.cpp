// Copyright (C) 2001-2002 Raven Software
//
// g_gametype.c -- dynamic gametype handling

#include "g_local.h"
#include "q_shared.h"
#include "g_team.h"
#include "inv.h"

/*QUAKED gametype_player (0 1 0) (-16 -16 -46) (16 16 48) REDTEAM BLUETEAM
Potential spawning position for red or blue team in custom gametype games.
*/
void SP_gametype_player ( gentity_t *ent ) 
{
	team_t	team;

	// Cant take any more spawns!!
	if ( level.spawnCount >= MAX_SPAWNS )
	{
		G_FreeEntity ( ent );
		return;
	}
	
	// If a team filter is set then override any team settings for the spawns
	if ( level.mTeamFilter[0] )
	{
		if ( strcmp ( level.mTeamFilter, "red") == 0 )
		{
			team = TEAM_RED;
		}
		else if ( strcmp ( level.mTeamFilter, "blue") == 0 )
		{
			team = TEAM_BLUE;
		}
		else
		{
			G_FreeEntity ( ent );
			return;
		}
	}
	else
	{
		// Red team only
		if ( ent->spawnflags & 0x1 )
		{
			team = TEAM_RED;
		}
		else if ( ent->spawnflags & 0x2 )
		{
			team = TEAM_BLUE;
		}
	}

	G_AddClientSpawn ( ent, team );

	G_FreeEntity ( ent );
}

void SP_mission_player ( gentity_t* ent )
{
	ent->classname = "gametype_player";

	SP_gametype_player ( ent );
}

void gametype_trigger_touch ( gentity_t *self, gentity_t *other, trace_t *trace ) 
{
	if ( level.gametypeResetTime )
	{
		return;
	}

	if (gtCore->onTriggerTouch(self, other))
		G_UseTargets(self, other);
}

/*QUAKED gametype_trigger (0 0 .8) ? 
*/
void SP_gametype_trigger ( gentity_t* ent )
{
	// The target name is only used by the gametype system, so it shouldnt
	// be adjusted by the RMG
	if ( level.mTargetAdjust )
	{
		if ( ent->targetname )
			ent->targetname = strchr ( ent->targetname, '-' ) + 1;
	}

	InitTrigger (ent);
}

static gentity_t* G_RealSpawnGametypeItem ( gentity_t* ent, bool dropped )
{
	gentity_t* it_ent;

	it_ent = G_Spawn();

	it_ent->flags |= FL_DROPPED_ITEM;
	it_ent->item = ent->item;

	VectorCopy( ent->r.currentOrigin, it_ent->s.origin );
	VectorCopy ( ent->s.angles, it_ent->s.apos.trBase );
	it_ent->classname = ent->item->classname;
	G_SpawnItem ( it_ent, it_ent->item );
	FinishSpawningItem(it_ent);	
	
	VectorSet( it_ent->r.mins, -ITEM_RADIUS * 4 / 3, -ITEM_RADIUS * 4 / 3, -ITEM_RADIUS );
	VectorSet( it_ent->r.maxs, ITEM_RADIUS * 4 / 3, ITEM_RADIUS * 4 / 3, ITEM_RADIUS );

	// Red team only
	if ( ent->s.eFlags & EF_REDTEAM )
	{
		it_ent->s.eFlags |= EF_REDTEAM;
	}
	
	if ( ent->s.eFlags & EF_BLUETEAM )
	{
		it_ent->s.eFlags |= EF_BLUETEAM;
	}

	return it_ent;
}

gentity_t* G_SpawnGametypeItem ( const char* pickup_name, bool dropped )
{
	gentity_t* ent;

	// Look for the gametype item in the map
	ent = NULL;
	while ( NULL != (ent = G_Find ( ent, FOFS(classname), "gametype_item" ) ) )
	{
		// Match?
		if ( !strcmp ( ent->item->pickup_name, pickup_name ) )
		{
			break;
		}
	}
	
	// If we couldnt find the item spawner then we have a problem	
	if ( !ent )
	{
		Com_Error ( ERR_FATAL, "Could not spawn gametype item '%s'\n", pickup_name );
		return NULL;
	}		

	return G_RealSpawnGametypeItem ( ent, dropped );
}

void G_GametypeItemThink ( gentity_t* ent )
{
	G_RealSpawnGametypeItem ( ent, false );
}

/*QUAKED gametype_item (0 0 1) (-16 -16 -16) (16 16 16) 
"name"			name of the item to spawn (defined in gametype script)
*/
void SP_gametype_item ( gentity_t* ent )
{
	// TEMPORARY HACK
	if ( level.mTargetAdjust )
	{
		if ( ent->targetname )
			ent->targetname = strchr ( ent->targetname, '-' ) + 1;
	
		if ( ent->target )
			ent->target = strchr ( ent->target, '-' ) + 1;
	}

	G_SetOrigin( ent, ent->s.origin );
}

/*
===============
G_ResetGametypeItem
===============
*/
void G_ResetGametypeItem ( gitem_t* item )
{
	gentity_t *find;

	// Convience check
	if ( !item )
	{
		return;
	}

	// Remove all spawned instances of the item on the map
	find = NULL;
	while ( NULL != (find = G_Find ( find, FOFS(classname), item->classname ) ) )
	{
		// Free this entity soon
		find->nextthink = level.time + 10;
		find->think = G_FreeEntity;
	}

	// Strip the item from all connected clients
	for (int i = 0; i < level.numConnectedClients; i ++ )
	{
		g_entities[level.sortedClients[i]].client->ps.stats[STAT_GAMETYPE_ITEMS] &= ~(1<<item->giTag);
	}

	// Respawn the item in all of its locations
	find = NULL;
	while ( NULL != (find = G_Find ( find, FOFS(classname), "gametype_item" ) ) )
	{
		if ( find->item != item )
		{
			continue;
		}

		G_RealSpawnGametypeItem ( find, false );
	}
}

/*
===============
G_ResetGametypeEntities
===============
*/
void G_ResetGametypeEntities ( void )
{
	gentity_t* find;
	bool   initVisible;

	// Show total time
	initVisible = true;
	trap_SetConfigstring ( CS_GAMETYPE_TIMER, "0" );

	// Reset all of the gametype items.  This must be done last because the spawn
	// function may alter enabled states of triggers or scripts
	find = NULL;
	while ( NULL != (find = G_Find ( find, FOFS(classname), "gametype_item" ) ) )
	{
		G_ResetGametypeItem ( find->item );
	}
}

/*
===============
G_RespawnClients
===============
*/
void G_RespawnClients ( bool force, team_t team )
{
	// Respawn all clients back at a spawn pointer
	for (int i = 0; i < level.numConnectedClients; i ++ )
	{
		// Save the clients weapons
		playerState_t	ps;
		bool		ghost;

		gentity_t *	ent = &g_entities[level.sortedClients[i]];

		// Make sure they are actually connected
		if ( ent->client->pers.connected != CON_CONNECTED )
		{
			continue;
		}

		// Save the entire player state so certain things
		// can be maintained across rounds
		ps = ent->client->ps;

		ghost = ent->client->sess.ghost;

		// Spawn the requested team
		if ( ent->client->sess.team != team )
		{
			continue;
		}

		// Dont force them to respawn if they arent a ghost
		if ( !ghost && !force )
		{
			continue;
		}

		// If the client is a ghost then revert them
		if ( ent->client->sess.ghost )
		{
			// Clean up any following monkey business
			G_StopFollowing ( ent );

			// Disable being a ghost
			ent->client->ps.pm_flags &= ~PMF_GHOST;
			ent->client->ps.pm_type = PM_NORMAL;
			ent->client->sess.ghost = false;
		}
		
		ent->client->sess.noTeamChange = false;

		trap_UnlinkEntity (ent);
		ClientSpawn ( ent );
	}
}

/*
===============
G_ResetPickups
===============
*/
void G_ResetEntities ( void )
{
	// Run through all the entities in the level and reset those which
	// need to be reset
	for (int i = 0; i < level.num_entities; i ++ )
	{
		gentity_t* ent;

		ent = &g_entities[i];

		// Skip entities not in use
		if ( !ent->inuse )
		{
			continue;
		}

		// If this is a player then unlink it so then clients
		// spawned in dont telefrag
		if ( ent->s.eType == ET_PLAYER || ent->s.eType == ET_BODY )
		{
			trap_UnlinkEntity ( ent );
		}
		// If this is a missile
		else if ( ent->s.eType == ET_MISSILE )
		{
			G_FreeEntity ( ent );
		}
		// If the dropped flag is set then free it
		else if ( ent->flags & FL_DROPPED_ITEM )
		{
			G_FreeEntity ( ent );
		}
		// If this item is waiting to be respawned, then respawn it
		else if ( ent->think == RespawnItem )
		{
			RespawnItem ( ent );
		}
		else if ( ent->s.eType == ET_DAMAGEAREA )
		{
			G_FreeEntity ( ent );
		}
	}
}

/*
===============
G_ResetGametype
===============
*/
void G_ResetGametype ( void )
{
	gentity_t*	tent;

	// Reset the glass in the level
	G_ResetGlass ( );

	// Reset all pickups in the world
	G_ResetEntities ( );

	// Reset the gametype itself
	G_ResetGametypeEntities ( );

	// Initialize the respawn interval since this is a interval gametype
	switch ( level.gametypeData->respawnType )
	{
		case RT_INTERVAL:
			level.gametypeRespawnTime[TEAM_RED]  = 0;
			level.gametypeRespawnTime[TEAM_BLUE] = 0;
			level.gametypeRespawnTime[TEAM_FREE] = 0;
			break;

		case RT_NONE:
			level.gametypeDelayTime = level.time + g_roundstartdelay.integer * 1000;
			level.gametypeRoundTime = level.time + (g_roundtimelimit.integer * 60000);

			if ( level.gametypeDelayTime != level.time )
			{
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,@Get Ready", level.gametypeDelayTime ) );
			}

			trap_SetConfigstring ( CS_GAMETYPE_TIMER, va("%i", level.gametypeRoundTime) );
			break;
	}

	level.gametypeJoinTime  = 0;

	// Allow people to join 20 seconds after both teams have people on them
	if ( !level.gametypeData->teams )
	{
		level.gametypeJoinTime = level.time;
	}

	// Respawn all clients
	G_RespawnClients ( true, TEAM_RED );
	G_RespawnClients ( true, TEAM_BLUE );

	level.gametypeStartTime = level.time;
	level.gametypeResetTime = 0;

	// Reset the clients local effects
	tent = G_TempEntity( vec3_origin, EV_GAMETYPE_RESTART );
	tent->r.svFlags |= SVF_BROADCAST;
}

/*
===============
G_ParseGametypeItems
===============
*/
bool G_ParseGametypeItems ( TGPGroup* itemsGroup )
{
	TGPGroup	itemGroup;
	int			itemCount;
	char		temp[MAX_QPATH];
	gentity_t	*ent;

	// Handle NULL for convienience
	if ( !itemsGroup )
	{
		return false;
	}

	// Loop over all the items and add each 
	itemGroup = trap_GPG_GetSubGroups ( itemsGroup );
	itemCount = 0;

	while ( itemGroup )
	{	
		gitem_t*   item;
		
		// Parse out the pickup name
		trap_GPG_GetName ( itemGroup, temp );

		item = BG_FindItem ( temp );
		if ( !item )
		{
			item = &bg_itemlist[ MODELINDEX_GAMETYPE_ITEM + itemCount ];
			item->pickup_name = (char *)trap_VM_LocalStringAlloc ( temp );
			itemCount++;
		}

		// Handle the entity specific stuff by finding all matching items that 
		// were spawned.
		ent = NULL;
		while ( NULL != (ent = G_Find ( ent, FOFS(targetname), item->pickup_name ) ) )
		{
			// If not a gametype item then skip it
			if ( strcmp ( ent->classname, "gametype_item" ) )
			{
				continue;
			}

			// Setup the gametype data
			ent->item	   = item;
			ent->nextthink = level.time + 200;
			ent->think     = G_GametypeItemThink;
		}

		// Next sub group
		itemGroup = trap_GPG_GetNext(itemGroup);
	}

	return true;
}

/*
===============
G_ParseGametypeFile
===============
*/
bool G_ParseGametypeFile ( void )
{
	TGenericParser2 GP2;
	TGPGroup		topGroup;
	TGPGroup		gametypeGroup;
	char			value[4096];

	// Default the gametype config strings
	trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, "0,0" );

	// Open the given gametype file
	GP2 = trap_GP_ParseFile ( (char*)level.gametypeData->script, true, false );
	if (!GP2)
	{
		return false;
	}

	// Grab the top group and the list of sub groups
	topGroup = trap_GP_GetBaseParseGroup(GP2);
	gametypeGroup = trap_GPG_FindSubGroup(topGroup, "gametype" );
	if ( !gametypeGroup )
	{
		trap_GP_Delete(&GP2);
		return false;
	}

	// Look for the respawn type
	trap_GPG_FindPairValue ( gametypeGroup, "respawn", "normal", value );
	if ( !strcmp ( value, "none" ) )
	{
		level.gametypeData->respawnType = RT_NONE;
	}
	else if ( !strcmp ( value, "interval" ) )
	{
		level.gametypeData->respawnType = RT_INTERVAL;
	}
	else
	{
		level.gametypeData->respawnType = RT_NORMAL;
	}

	// Grab the defined items
	G_ParseGametypeItems ( (TGPGroup *)trap_GPG_FindSubGroup ( gametypeGroup, "items" ) );

	// Free up the parser
	trap_GP_Delete(&GP2);

	return true;
}

/*
=================
G_EnableGametypeItemPickup

Drops all of the gametype items held by the player
=================
*/
void G_EnableGametypeItemPickup(gentity_t* ent)
{
	ent->s.eFlags &= ~EF_NOPICKUP;
}

/*
=================
G_DropGametypeItems

Drops all of the gametype items held by the player
=================
*/
void G_DropGametypeItems(gentity_t* self, int delayPickup)
{
	// drop all custom gametype items
	for (int i = 0; i < MAX_GAMETYPE_ITEMS; i++)
	{
		// skip this gametype item if the client doenst have it
		if (!(self->client->ps.stats[STAT_GAMETYPE_ITEMS] & (1 << i)))
		{
			continue;
		}

		gitem_t *item = BG_FindGametypeItem(i);
		if (!item)
		{
			continue;
		}
		float angle = 0;
		gentity_t *drop = G_DropItem(self, item, angle);
		drop->count = 1;
		angle += 45;

		if (delayPickup)
		{
			drop->nextthink = level.time + delayPickup;
			drop->s.eFlags |= EF_NOPICKUP;
			drop->think = G_EnableGametypeItemPickup;
		}

		// TAke it away from the client just in case
		self->client->ps.stats[STAT_GAMETYPE_ITEMS] &= ~(1 << i);

		if (self->enemy && self->enemy->client && !OnSameTeam(self->enemy, self))
		{
			gtCore->onItemDefend(self);
		}
	}

	self->client->ps.stats[STAT_GAMETYPE_ITEMS] = 0;
}