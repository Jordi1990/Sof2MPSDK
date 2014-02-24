// Copyright (C) 2001-2002 Raven Software.
//
#include "g_local.h"

// Henk 23/02/14 -> New command system for normal commands
typedef struct
{
	string	cmd; // command
	void(*Function)(gentity_t *ent); // store pointer to the given function so we can call it later
} cmd_t;

/*
==================
DeathmatchScoreboardMessage
==================
*/
void DeathmatchScoreboardMessage( gentity_t *ent ) 
{
	string scoreboardString;

	for (int i = 0; i < level.numConnectedClients; i++)
	{
		int	ping;
		char entry[36];
		gclient_t *cl = &level.clients[level.sortedClients[i]];

		if (cl->pers.connected == CON_CONNECTING)
		{
			ping = -1;
		}
		else
		{
			ping = cl->ps.ping < 999 ? cl->ps.ping : 999;
		}

		if (cl->rpmClient > 0.5)
		sprintf_s(entry, 36,
			" %i %i %i %i %i %i %i %i %i %.2f %i",
			level.sortedClients[i],
			cl->sess.score,
			cl->sess.kills,
			cl->sess.deaths,
			ping,
			(level.time - cl->pers.enterTime) / 60000,
			(cl->sess.ghost || cl->ps.pm_type == PM_DEAD) ? true : false,
			g_entities[level.sortedClients[i]].s.gametypeitems,
			g_teamkillDamageMax.integer ? 100 * cl->sess.teamkillDamage / g_teamkillDamageMax.integer : 0,
			cl->pers.statinfo.accuracy,
			cl->pers.statinfo.headShotKills,
			cl->pers.statinfo.damageDone
			);
		else
			sprintf_s(entry, 36,
			" %i %i %i %i %i %i %i %i %i",
			level.sortedClients[i],
			cl->sess.score,
			cl->sess.kills,
			cl->sess.deaths,
			ping,
			(level.time - cl->pers.enterTime)/60000,
			(cl->sess.ghost || cl->ps.pm_type == PM_DEAD) ? true : false,
			g_entities[level.sortedClients[i]].s.gametypeitems,
			g_teamkillDamageMax.integer ? 100 * cl->sess.teamkillDamage / g_teamkillDamageMax.integer : 0
			);

		scoreboardString.append(entry);
	}
	trap_SendServerCommand(ent->s.number, va("scores %i %i %i%s", level.numConnectedClients, level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE],
		scoreboardString.c_str()));
}


/*
==================
Cmd_Score_f

Request current scoreboard information
==================
*/
void Cmd_Score_f( gentity_t *ent ) 
{
	DeathmatchScoreboardMessage( ent );
}

/*
==================
CheatsOk
==================
*/
bool	CheatsOk( gentity_t *ent ) {
	if ( !g_cheats.integer ) {
		infoMsgToClients(ent->client->ps.clientNum, va("Cheats are not enabled on this server"));
		return false;
	}
	if ( ent->health <= 0 ) {
		infoMsgToClients(ent->client->ps.clientNum, va("You must be alive to use this command"));
		return false;
	}
	return true;
}


/*
==================
ConcatArgs
==================
*/
char	*ConcatArgs( int start ) {
	int		c, tlen;
	static char	line[MAX_STRING_CHARS];
	int		len;
	char	arg[MAX_STRING_CHARS];

	len = 0;
	c = trap_Argc();
	for (int i = start ; i < c ; i++ ) {
		trap_Argv( i, arg, sizeof( arg ) );
		tlen = strlen( arg );
		if ( len + tlen >= MAX_STRING_CHARS - 1 ) {
			break;
		}
		memcpy( line + len, arg, tlen );
		len += tlen;
		if ( i != c - 1 ) {
			line[len] = ' ';
			len++;
		}
	}

	line[len] = 0;

	return line;
}

/*
==================
Cmd_Drop_f

Drops the currenty selected weapon
==================
*/
void Cmd_Drop_f ( gentity_t* ent )
{
	gentity_t* dropped;

	// spectators cant drop anything since they dont have anything
	if ( ent->client->sess.team == TEAM_SPECTATOR )
	{
		return;
	}

	// Ghosts and followers cant drop stuff
	if ( ent->client->ps.pm_flags & (PMF_GHOST|PMF_FOLLOW) )
	{
		return;
	}

	// Drop the weapon the client wanted to drop
	dropped = G_DropWeapon ( ent, (weapon_t)atoi(ConcatArgs( 1 )), 3000 );
	if ( !dropped )
	{
		return;
	}
}

void Cmd_DropItem_f(gentity_t* ent)
{
	// spectators cant drop anything since they dont have anything
	if (ent->client->sess.team == TEAM_SPECTATOR)
		return;

	// Ghosts and followers cant drop stuff
	if (ent->client->ps.pm_flags & (PMF_GHOST | PMF_FOLLOW))
		return;

	// Nothing to drop
	if (!ent->client->ps.stats[STAT_GAMETYPE_ITEMS]){
		infoMsgToClients(ent->s.number, va("You don't have any gametype item to drop"));
		return;
	}

	G_DropGametypeItems(ent, 3000);
}

/*
==================
Cmd_Give_f

Give items to a client
==================
*/
void Cmd_Give_f (gentity_t *ent)
{
	char		*name;
	gitem_t		*it;
	bool	give_all;
	gentity_t		*it_ent;
	trace_t		trace;
	char		arg[MAX_QPATH];

	int start;
	int end;

	trap_Argv( 1, arg, sizeof( arg ) );

	if ( !strcmp ( arg, "me" ) )
	{
		start = ent->s.number;
		end = start + 1;
	}
	else if ( !strcmp ( arg, "all" ) )
	{
		start = 0;
		end   = MAX_CLIENTS;
	}
	else
	{
		start = atoi ( arg );
		end = start + 1;
	}

	for (int l = start; l < end; l ++ )
	{
		ent = &g_entities[l];

		if ( !ent->inuse )
		{
			continue;
		}

		if ( G_IsClientDead ( ent->client ) )
		{
			continue;
		}

		if ( !CheatsOk( ent ) ) {
			return;
		}

	name = ConcatArgs( 2 );

	if (strcmp(name, "all") == 0)
		give_all = true;
	else
		give_all = false;

	if (give_all || strcmp( name, "health") == 0)
	{
		ent->health = MAX_HEALTH;
		if (!give_all)
			continue;
	}

	if (give_all || strcmp(name, "weapons") == 0)
	{
		ent->client->ps.stats[STAT_WEAPONS] = (1 << WP_NUM_WEAPONS) - 1 - ( 1 << WP_NONE );
		if (!give_all)
			continue;
	}

	if (give_all || strcmp(name, "ammo") == 0)
	{
		for (int i = WP_NONE + 1 ; i < WP_NUM_WEAPONS ; i++ ) 
		{
			for ( int a = ATTACK_NORMAL; a < ATTACK_MAX; a ++ )
			{
				ent->client->ps.clip[a][i] = weaponData[i].attack[a].clipSize;
				ent->client->ps.ammo[weaponData[i].attack[a].ammoIndex] = ammoData[weaponData[i].attack[a].ammoIndex].max;
			}
		}

		if (!give_all)
			continue;
	}

	if (give_all || strcmp(name, "armor") == 0)
	{
		ent->client->ps.stats[STAT_ARMOR] = MAX_ARMOR;

		if (!give_all)
			continue;
	}

	// spawn a specific item right on the player
	if ( !give_all ) 
	{
		it = BG_FindItem (name);
		if (!it) 
		{
			continue;
		}

		if ( it->giType == IT_GAMETYPE )
		{
			continue;
		}

		it_ent = G_Spawn();
		VectorCopy( ent->r.currentOrigin, it_ent->s.origin );
		it_ent->classname = it->classname;
		G_SpawnItem (it_ent, it);
		FinishSpawningItem(it_ent );
		memset( &trace, 0, sizeof( trace ) );
		Touch_Item (it_ent, ent, &trace);
		if (it_ent->inuse) 
		{
			G_FreeEntity( it_ent );
		}
	}

	}
}


/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/
void Cmd_God_f (gentity_t *ent)
{
	char	*msg;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	ent->flags ^= FL_GODMODE;
	if (!(ent->flags & FL_GODMODE) )
		msg = "godmode OFF\n";
	else
		msg = "godmode ON\n";

	infoMsgToClients(ent->client->ps.clientNum, va("%s", msg));
}

/*
==================
Cmd_Noclip_f

argv(0) noclip
==================
*/
void Cmd_Noclip_f( gentity_t *ent ) {
	char	*msg;

	if ( !CheatsOk( ent ) ) {
		return;
	}

	if ( ent->client->noclip ) {
		msg = "noclip OFF\n";
	} else {
		msg = "noclip ON\n";
	}
	ent->client->noclip = !ent->client->noclip;

	infoMsgToClients(ent->client->ps.clientNum, va("%s", msg));
}


/*
==================
Cmd_LevelShot_f

This is just to help generate the level pictures
for the menus.  It goes to the intermission immediately
and sends over a command to the client to resize the view,
hide the scoreboard, and take a special screenshot
==================
*/
void Cmd_LevelShot_f( gentity_t *ent ) 
{
	if ( !CheatsOk( ent ) ) 
	{
		return;
	}

	BeginIntermission();

	trap_SendServerCommand( ent-g_entities, "clientLevelShot" );
}

/*
=================
Cmd_Kill_f
=================
*/
void Cmd_Kill_f( gentity_t *ent ) 
{
	// No killing yourself if your a spectator
	if ( G_IsClientSpectating ( ent->client ) )
	{
		return;
	}

	// No killing yourself if your dead
	if ( G_IsClientDead ( ent->client ) ) 
	{
		return;
	}

	ent->flags &= ~FL_GODMODE;
	ent->client->ps.stats[STAT_HEALTH] = ent->health = -999;
	player_die (ent, ent, ent, 100000, MOD_SUICIDE, HL_NONE, vec3_origin );
}

/*
=================
BroadCastTeamChange

Let everyone know about a team change
=================
*/
void BroadcastTeamChange( gclient_t *client, int oldTeam )
{
	switch ( client->sess.team )
	{
		case TEAM_RED:
			trap_SendServerCommand(-1, va("cp \"%s ^7joined the red team.\n\"", client->pers.netname.c_str()));
			break;

		case TEAM_BLUE:
			trap_SendServerCommand(-1, va("cp \"%s ^7joined the blue team.\n\"", client->pers.netname.c_str()));
			break;

		case TEAM_SPECTATOR:
			if ( oldTeam != TEAM_SPECTATOR )
			{
				trap_SendServerCommand(-1, va("cp \"%s ^7joined the spectators.\n\"", client->pers.netname.c_str()));
			}
			break;

		case TEAM_FREE:
			trap_SendServerCommand( -1, va("cp \"%s ^7joined the battle.\n\"", client->pers.netname.c_str()));
			break;
	}
}

/*
=================
SetTeam
=================
*/
void SetTeam( gentity_t *ent, char *s, const char* identity ) 
{
	int					team;
	int					oldTeam;
	gclient_t			*client;
	int					clientNum;
	spectatorState_t	specState;
	int					specClient;
	bool			ghost;
   	bool			noOutfittingChange = false;

	// see what change is requested
	//
	client = ent->client;

	clientNum = client - level.clients;
	specClient = 0;
	specState = SPECTATOR_NOT;
	userinfo userInfo(clientNum);
	// If an identity was specified then inject it into
	// the clients userinfo
	if ( identity )
	{
		if ( strcmp ( identity, userInfo.identity.c_str()))
		{
			userInfo.setIdentity(clientNum, identity);
		}
		else
		{
			identity = NULL;
		}
	}

	if ( !strcmp( s, "follow1" ) ) 
	{
		team = TEAM_SPECTATOR;
		specState = SPECTATOR_FOLLOW;
		specClient = -1;
	} 
	else if ( !strcmp( s, "follow2" ) ) 
	{
		team = TEAM_SPECTATOR;
		specState = SPECTATOR_FOLLOW;
		specClient = -2;
	} 
	else if ( !strcmp( s, "spectator" ) || !strcmp( s, "s" ) ) 
	{
		team = TEAM_SPECTATOR;
		specState = SPECTATOR_FREE;
	} 
	else if ( level.gametypeData->teams ) 
	{
		// if running a team game, assign player to one of the teams
		specState = SPECTATOR_NOT;
		if ( !strcmp( s, "red" ) || !strcmp( s, "r" ) ) 
		{
			team = TEAM_RED;
		} 
		else if ( !strcmp( s, "blue" ) || !strcmp( s, "b" ) ) 
		{
			team = TEAM_BLUE;
		} 
		else 
		{
			// pick the team with the least number of players
			team = PickTeam( clientNum );
		}

		if ( g_teamForceBalance.integer  ) 
		{
			int		counts[TEAM_NUM_TEAMS];

			counts[TEAM_BLUE] = TeamCount( ent->client->ps.clientNum, TEAM_BLUE, NULL );
			counts[TEAM_RED] = TeamCount( ent->client->ps.clientNum, TEAM_RED, NULL );

			// We allow a spread of two
			if ( team == TEAM_RED && counts[TEAM_RED] - counts[TEAM_BLUE] > 1 ) 
			{
				trap_SendServerCommand( ent->client->ps.clientNum, 
										"cp \"Red team has too many players.\n\"" );

				// ignore the request
				return; 
			}
			if ( team == TEAM_BLUE && counts[TEAM_BLUE] - counts[TEAM_RED] > 1 ) 
			{
				trap_SendServerCommand( ent->client->ps.clientNum, 
										"cp \"Blue team has too many players.\n\"" );

				// ignore the request
				return; 
			}

			// It's ok, the team we are switching to has less or same number of players
		}
	} 
	else 
	{
		// force them to spectators if there aren't any spots free
		team = TEAM_FREE;
	}

	// override decision if limiting the players
	if ( g_maxGameClients.integer > 0 && level.numNonSpectatorClients >= g_maxGameClients.integer ) 
	{
		team = TEAM_SPECTATOR;
	}

	// decide if we will allow the change
	oldTeam = client->sess.team;
	ghost   = client->sess.ghost;

	if ( team == oldTeam && team != TEAM_SPECTATOR )
	{
		if ( identity )
		{
			// get and distribute relevent paramters
			client->pers.identity = NULL;
			ClientUserinfoChanged( clientNum, &userInfo );
		}
		
		return;
	}

   	noOutfittingChange = ent->client->noOutfittingChange;

	// he starts at 'base'
	client->pers.teamState.state = TEAM_BEGIN;
	
	if ( oldTeam != TEAM_SPECTATOR ) 
	{
		if ( ghost )
		{
			G_StopGhosting ( ent );
		}
		else if ( !G_IsClientDead ( client ) )
		{
			// Kill him (makes sure he loses flags, etc)
			ent->flags &= ~FL_GODMODE;
			ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
			player_die (ent, ent, ent, 100000, MOD_TEAMCHANGE, HL_NONE, vec3_origin );

			ent->client->sess.ghost = false;
		}
	}

	// If respawn interval start as a ghost
	if ( level.gametypeRespawnTime[ team ] )
	{
		ghost = true;
	}

	// they go to the end of the line
	if ( team == TEAM_SPECTATOR ) 
	{
		client->sess.spectatorTime = level.time;
	}

	client->sess.team = (team_t)team;
	client->sess.spectatorState = specState;
	client->sess.spectatorClient = specClient;

	// Always spawn into a ctf game using a respawn timer.
	if ( team != TEAM_SPECTATOR && level.gametypeData->respawnType == RT_INTERVAL )
	{
		G_SetRespawnTimer ( ent );
		ghost = true;
	}

	BroadcastTeamChange( client, oldTeam );

	// See if we should spawn as a ghost
	if ( team != TEAM_SPECTATOR && level.gametypeData->respawnType == RT_NONE )
	{
		// If there are ghosts already then spawn as a ghost because
		// the game is already in progress.
		if ( (level.gametypeJoinTime && (level.time - level.gametypeJoinTime) > 20000) || noOutfittingChange || client->sess.noTeamChange )
		{
			ghost = true;
		}

		// Spectator to a team doesnt count
		if ( oldTeam != TEAM_SPECTATOR )
		{
			client->sess.noTeamChange = true;
		}
	}

	// If a ghost, enforce it
	if ( ghost )
	{
		// Make them a ghost again
		if ( team != TEAM_SPECTATOR )
		{
			G_StartGhosting ( ent );

			// get and distribute relevent paramters
			client->pers.identity = NULL;
			ClientUserinfoChanged( clientNum, &userInfo );

			CalculateRanks();

			return;
		}
	}	

	// get and distribute relevent paramters
	client->pers.identity = NULL;
	ClientUserinfoChanged( clientNum, &userInfo );

	CalculateRanks();

	// Begin the clients new life on the their new team
	ClientBegin( clientNum );
}

/*
=================
G_StartGhosting

Starts a client ghosting.  This essentially will kill a player which is alive
=================
*/
void G_StartGhosting ( gentity_t* ent )
{
	// Dont start ghosting if already ghosting
	if ( ent->client->sess.ghost )
	{
		return;
	}

	ent->client->sess.ghost = true;
	ent->client->sess.spectatorState = SPECTATOR_FREE;
	ent->client->sess.spectatorClient = -1;
	ent->client->ps.pm_flags |= PMF_GHOST;
	ent->client->ps.stats[STAT_HEALTH] = 100;
	ent->client->ps.pm_type = PM_SPECTATOR;
	ent->client->ps.pm_flags &= ~PMF_FOLLOW;

	trap_UnlinkEntity (ent);

	// stop any following clients
	for (int i = 0 ; i < level.maxclients ; i++ ) 
	{
		if ( G_IsClientSpectating ( &level.clients[i] )
			&& level.clients[i].sess.spectatorState == SPECTATOR_FOLLOW
			&& level.clients[i].sess.spectatorClient == ent->s.number ) 
		{
			G_StopFollowing( &g_entities[i] );
		}
	}
}

/*
=================
G_StopGhosting

Stops a client from ghosting.  The client will be dead after this
call
=================
*/
void G_StopGhosting ( gentity_t* ent )
{
	// Dont stop someone who isnt ghosting in the first place
	if ( !ent->client->sess.ghost )
	{
		return;
	}

	ent->client->sess.ghost = false;
	ent->client->ps.pm_flags &= ~PMF_GHOST;
	ent->client->ps.pm_flags &= ~PMF_FOLLOW;

	if ( ent->client->sess.team == TEAM_SPECTATOR )
	{
		ent->client->ps.pm_type = PM_SPECTATOR;
	}
	else
	{
		ent->client->ps.pm_type = PM_DEAD;
		ent->health = ent->client->ps.stats[STAT_HEALTH] = 0;
	}
}

/*
=================
G_StopFollowing

If the client being followed leaves the game, or you just want to drop
to free floating spectator mode
=================
*/
void G_StopFollowing( gentity_t *ent ) 
{
	// Cant stop following if not following in the first place
	if ( !(ent->client->ps.pm_flags&PMF_FOLLOW) )
	{
		return;
	}

	// Clear the following variables
	ent->client->ps.pm_flags &= ~PMF_FOLLOW;
	ent->client->sess.spectatorState = SPECTATOR_FREE;
	ent->client->ps.clientNum = ent - g_entities;
	ent->client->ps.zoomFov = 0;
	ent->client->ps.pm_flags &= ~(PMF_GOGGLES_ON|PMF_ZOOM_FLAGS);
	ent->client->ps.persistant[PERS_TEAM] = ent->client->sess.team;
	ent->r.svFlags &= ~SVF_BOT;

	// Ghots dont really become spectators, just psuedo spectators
	if ( ent->client->sess.ghost )
	{
		// Do a start and stop to ensure the variables are all set properly
		G_StopGhosting ( ent );
		G_StartGhosting ( ent );
	}
	else
	{
		ent->client->sess.team = TEAM_SPECTATOR;	
		ent->client->ps.persistant[ PERS_TEAM ] = TEAM_SPECTATOR;	
	}

	// If we were in fact following someone, then make the angles and origin nice for
	// when we stop
   	if ( ent->client->sess.spectatorClient != -1 )
   	{
   		gclient_t* cl = &level.clients[ent->client->sess.spectatorClient];
    
   		for (int i = 0; i < 3; i ++ )
		{
   			ent->client->ps.delta_angles[i] = ANGLE2SHORT(cl->ps.viewangles[i] - SHORT2ANGLE(ent->client->pers.cmd.angles[i]));
		}
    
   		VectorCopy ( cl->ps.viewangles, ent->client->ps.viewangles );
   		VectorCopy ( cl->ps.origin, ent->client->ps.origin );
   		VectorClear ( ent->client->ps.velocity );
   		ent->client->ps.movementDir = 0;
    
   		BG_PlayerStateToEntityState( &ent->client->ps, &ent->s, true );
   	}		

	ent->client->sess.spectatorClient = -1;
}

/*
=================
Cmd_Team_f
=================
*/
void Cmd_Team_f( gentity_t *ent ) 
{
	char team[MAX_TOKEN_CHARS];
	char identity[MAX_TOKEN_CHARS];

	// Need at least the team specified in the arguments
	if ( trap_Argc() < 2 ) 
	{
		int oldTeam = ent->client->sess.team;
		switch ( oldTeam ) 
		{
			case TEAM_BLUE:
				trap_SendServerCommand( ent-g_entities, "print \"Blue team\n\"" );
				break;
			
			case TEAM_RED:
				trap_SendServerCommand( ent-g_entities, "print \"Red team\n\"" );
				break;
			
			case TEAM_FREE:
				trap_SendServerCommand( ent-g_entities, "print \"Free team\n\"" );
				break;
		
			case TEAM_SPECTATOR:
				trap_SendServerCommand( ent-g_entities, "print \"Spectator team\n\"" );
				break;
		}
		
		return;
	}

	// Limit how often one can switch team
	if ( ent->client->switchTeamTime > level.time ) 
	{
		infoMsgToClients(ent->client->ps.clientNum, "May not switch teams more than once per 5 seconds");
		return;
	}

	trap_Argv( 1, team, sizeof( team ) );
	trap_Argv( 2, identity, sizeof( identity ) );

	SetTeam( ent, team, identity[0]?identity:NULL );

	// Remember the team switch time so they cant do it again really quick
	ent->client->switchTeamTime = level.time + 5000;
}


/*
=================
Cmd_Follow_f
=================
*/
void Cmd_Follow_f( gentity_t *ent ) 
{
	int		i;
	char	arg[MAX_TOKEN_CHARS];

	if ( trap_Argc() != 2 ) 
	{
		if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) 
		{
			G_StopFollowing( ent );
		}
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	i = atoi(arg);
	if ( i == -1 ) 
	{
		return;
	}

	// can't follow self
	if ( &level.clients[ i ] == ent->client ) 
	{
		return;
	}

	// cant cycle to dead people
	if ( level.clients[i].ps.pm_type == PM_DEAD )
	{
		return;
	}

	// can't follow another spectator
	if ( G_IsClientSpectating ( &level.clients[ i ] ) )
	{
		return;
	}

	// first set them to spectator as long as they arent a ghost
	if ( !ent->client->sess.ghost && ent->client->sess.team != TEAM_SPECTATOR ) 
	{
		SetTeam( ent, "spectator", NULL );
	}

	ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
	ent->client->sess.spectatorClient = i;
}

/*
=================
Cmd_FollowCycle_f
=================
*/
void Cmd_FollowCycle_f( gentity_t *ent, int dir ) 
{
	int		clientnum;
	int		deadclient;
	int		original;

	// first set them to spectator
	if ( !ent->client->sess.ghost && ent->client->sess.team != TEAM_SPECTATOR ) 
	{
		SetTeam( ent, "spectator", NULL );
	}

	if ( dir != 1 && dir != -1 ) 
	{
		Com_Error( ERR_FATAL, "Cmd_FollowCycle_f: bad dir %i", dir );
	}

	if ( ent->client->sess.spectatorClient == -1 )
	{
		clientnum = original = ent->s.number;
	}
	else
	{
		clientnum = original = ent->client->sess.spectatorClient;
	}

	deadclient = -1;
	do 
	{
		clientnum += dir;
		if ( clientnum >= level.maxclients ) 
		{
			clientnum = 0;
		}
		if ( clientnum < 0 ) 
		{
			clientnum = level.maxclients - 1;
		}

		// can only follow connected clients
		if ( level.clients[ clientnum ].pers.connected != CON_CONNECTED ) 
		{
			continue;
		}

		// can't follow another spectator
		if ( G_IsClientSpectating ( &level.clients[ clientnum ] ) ) 
		{
			continue;
		}

		// Cant switch to dead people unless there is nobody else to switch to
		if ( G_IsClientDead ( &level.clients[clientnum] ) )
		{		
			deadclient = clientnum;
			continue;
		}

		// Dissallow following of the enemy if the cvar is set
		if ( level.gametypeData->teams && !g_followEnemy.integer && ent->client->sess.team != TEAM_SPECTATOR )
		{
			// Are they on the same team?
			if ( level.clients[ clientnum ].sess.team != ent->client->sess.team )
			{
				continue;
			}
		}
			
		// this is good, we can use it
		ent->client->sess.spectatorClient = clientnum;
		ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
		return;

	} while ( clientnum != original );

	// If being forced to follow and there is a dead client to jump to, then jump to them now
	if ( deadclient != -1 && g_forceFollow.integer )
	{
		// this is good, we can use it
		ent->client->sess.spectatorClient = deadclient;
		ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
		return;
	}

   	G_StopFollowing( ent );

	// leave it where it was
}

/*
==================
G_SayTo
==================
*/
static void G_SayTo( gentity_t *ent, gentity_t *other, int mode, const char *name, const char *message ) 
{
	bool	 ghost = false;
	bool	 spec  = false;
	const char*  type;

	if (!other) 
	{
		return;
	}

	if (!other->inuse) 
	{
		return;
	}

	if (!other->client) 
	{
		return;
	}

	if ( other->client->pers.connected != CON_CONNECTED ) 
	{
		return;
	}

	if ( mode == SAY_TEAM  && !OnSameTeam(ent, other) ) 
	{
		return;
	}

	if ( !level.intermissiontime && !level.intermissionQueued )
	{
		// Spectators cant talk to alive people
		if ( ent->client->sess.team == TEAM_SPECTATOR )
		{
			spec = true;
		}

		if ( level.gametypeData->respawnType == RT_NONE )
		{
			// Dead people cant talk to alive people
			if ( !spec && G_IsClientDead ( ent->client ) )
			{
				ghost = true;
			}

			// If the client we are talking to is alive then a check
			// must be made to see if this talker is alowed to speak to this person
			if ( ent->s.number != other->s.number && !G_IsClientDead ( other->client ) && !G_IsClientSpectating( other->client) && (ghost || spec))
			{
				return;
			}
		}
	}

	type = "";
	if ( ghost )
	{
		type = "*ghost* ";
	}
	else if ( spec )
	{
		type = "*spec* ";
	}

	trap_SendServerCommand( other-g_entities, va("%s %d \"%s%s%s\"", 
							mode == SAY_TEAM ? "tchat" : "chat",
							ent->s.number,
							type, name, message));
}

/*
==================
G_GetChatPrefix
==================
*/
void G_GetChatPrefix ( gentity_t* ent, gentity_t* target, int mode, char* name, int nameSize )
{
	const char* namecolor;
	char		location[64];
	bool	locationOk = true;

	// Spectators and ghosts dont show locations
	if ( ent->client->ps.pm_type == PM_DEAD || G_IsClientSpectating ( ent->client ) )
	{
		locationOk = false;
	}

	if ( !level.gametypeData->teams && mode == SAY_TEAM ) 
	{
		mode = SAY_ALL;
	}

	if ( level.gametypeData->teams )
	{
		switch ( ent->client->sess.team )
		{
			case TEAM_BLUE:
				namecolor = S_COLOR_BLUE;
				break;

			case TEAM_RED:
				namecolor = S_COLOR_RED;
				break;

			default:
				namecolor = S_COLOR_WHITE;
				break;
		}
	}
	else
	{
		namecolor = S_COLOR_WHITE;
	}

	switch ( mode ) 
	{
		default:
		case SAY_ALL:

			sprintf_s (name, nameSize, "%s%s%s: ", namecolor, ent->client->pers.netname.c_str(), S_COLOR_WHITE );

			break;

		case SAY_TEAM:

			if ( locationOk && Team_GetLocationMsg(ent, location, sizeof(location)))
			{
				sprintf_s ( name, nameSize, "%s(%s%s) %s(%s): ", 
							  namecolor, 
							  ent->client->pers.netname.c_str(),
							  namecolor,
							  S_COLOR_WHITE, location );
			}
			else
			{
				sprintf_s ( name, nameSize, "%s(%s%s)%s: ", 
							  namecolor, 
							  ent->client->pers.netname.c_str(),
							  namecolor,
							  S_COLOR_WHITE );
			}
			break;

		case SAY_TELL:

			if ( locationOk && target && level.gametypeData->teams   && 
				 target->client->sess.team == ent->client->sess.team  &&
				 Team_GetLocationMsg(ent, location, sizeof(location))    )
			{
				sprintf_s ( name, nameSize, "%s[%s%s] %s(%s): ", 
							  namecolor,
							  ent->client->pers.netname.c_str(),
							  namecolor,
							  S_COLOR_WHITE, location );
			}
			else
			{
				sprintf_s ( name, nameSize, "%s[%s%s]%s: ", 
							  namecolor,
							  ent->client->pers.netname.c_str(),
							  namecolor,
							  S_COLOR_WHITE );
			}
			break;
	}

	strcat ( name, S_COLOR_GREEN );
}

/*
==================
G_Say
==================
*/
void G_Say ( gentity_t *ent, gentity_t *target, int mode, const char *chatText ) 
{
	gentity_t	*other;
	char		text[MAX_SAY_TEXT];
	char		name[64];

	// Logging stuff
	switch ( mode )
	{
		case SAY_ALL:
			G_LogPrintf("say: %s: %s\n", ent->client->pers.netname.c_str(), chatText);
			break;

		case SAY_TEAM:
			G_LogPrintf("sayteam: %s: %s\n", ent->client->pers.netname.c_str(), chatText);
			break;
	}

	// Generate the chat prefix
	G_GetChatPrefix ( ent, target, mode, name, sizeof(name) );

	// Save off the chat text
	strncpy( text, chatText, sizeof(text) );

	if ( target ) 
	{
		G_SayTo( ent, target, mode, name, text );
		return;
	}

	// echo the text to the console
	if ( g_dedicated.integer ) 
	{
		Com_Printf( "%s%s\n", name, text);
	}

	// send it to all the apropriate clients
	for (int j = 0; j < level.numConnectedClients; j++) 
	{
		other = &g_entities[level.sortedClients[j]];
		G_SayTo( ent, other, mode, name, text );
	}
}


/*
==================
Cmd_Say_f
==================
*/
static void Cmd_Say_f( gentity_t *ent, int mode, bool arg0 ) {
	char		*p;

	if ( trap_Argc () < 2 && !arg0 ) {
		return;
	}

	if (arg0)
	{
		p = ConcatArgs( 0 );
	}
	else
	{
		p = ConcatArgs( 1 );
	}

	G_Say( ent, NULL, mode, p );
}

/*
==================
Cmd_Tell_f
==================
*/
static void Cmd_Tell_f( gentity_t *ent ) {
	int			targetNum;
	gentity_t	*target;
	char		*p;
	char		arg[MAX_TOKEN_CHARS];

	if ( trap_Argc () < 2 ) {
		return;
	}

	trap_Argv( 1, arg, sizeof( arg ) );
	targetNum = atoi( arg );
	if ( targetNum < 0 || targetNum >= level.maxclients ) {
		return;
	}

	target = &g_entities[targetNum];
	if ( !target || !target->inuse || !target->client ) {
		return;
	}

	p = ConcatArgs( 2 );

	G_LogPrintf("tell: %s to %s: %s\n", ent->client->pers.netname.c_str(), target->client->pers.netname.c_str(), p);
	G_Say( ent, target, SAY_TELL, p );
	// don't tell to the player self if it was already directed to this player
	// also don't send the chat back to a bot
	if ( ent != target && !(ent->r.svFlags & SVF_BOT)) {
		G_Say( ent, ent, SAY_TELL, p );
	}
}


static void G_VoiceTo ( gentity_t *ent, gentity_t *other, int mode, const char* name, const char *id, bool voiceonly ) 
{
	// Only team say is supported right now for voice chatting
	if (mode != SAY_TEAM) 
	{
		return;
	}

	if (!other || !other->inuse || !other->client) 
	{
		return;
	}

	if ( !OnSameTeam(ent, other) ) 
	{
		return;
	}

	trap_SendServerCommand( other-g_entities, va("%s %d %d \"%s\" \"%s\"", "vtchat", voiceonly, ent->s.number, name, id));
}

/*
==================
G_CanVoiceGlobal

Can we globaly speak right now
==================
*/
bool G_CanVoiceGlobal ( void )
{
	if ( level.gametypeData->teams && level.time - level.globalVoiceTime > 5000 )
	{
		return true;
	}

	return false;
}

/*
==================
G_VoiceGlobal

says something out loud that everyone in the radius can hear
==================
*/
void G_VoiceGlobal ( gentity_t* ent, const char* id, bool force )
{
	if ( !ent )
	{
		return;
	}

	if ( !level.gametypeData->teams )
	{
		return;
	}

	if ( !force && level.time - level.globalVoiceTime < 5000 )
	{
		return;
	}
		
	level.globalVoiceTime = level.time;

	trap_SendServerCommand( -1, va("vglobal %d \"%s\"", ent->s.number, id));
}

/*
==================
G_Voice
==================
*/
void G_Voice( gentity_t *ent, gentity_t *target, int mode, const char *id, bool voiceonly ) 
{
	gentity_t	*other;
	char		name[MAX_SAY_TEXT];

	// Spectators and ghosts dont talk
	if ( ent->client->ps.pm_type == PM_DEAD || G_IsClientSpectating ( ent->client ) )
	{
		return;
	}

	// Voice flooding protection on?
	if ( g_voiceFloodCount.integer )
	{
		// If this client has been penalized for voice chatting to much then dont allow the voice chat
		if ( ent->client->voiceFloodPenalty )
		{
			if ( ent->client->voiceFloodPenalty > level.time )
			{
				return;
			}

			// No longer penalized
			ent->client->voiceFloodPenalty = 0;
		}

		// See if this client flooded with voice chats
		ent->client->voiceFloodCount++;
		if ( ent->client->voiceFloodCount >= g_voiceFloodCount.integer )
		{
			ent->client->voiceFloodCount = 0;
			ent->client->voiceFloodTimer = 0;
			ent->client->voiceFloodPenalty = level.time + g_voiceFloodPenalty.integer * 1000;

			infoMsgToClients(ent->client->ps.clientNum, va("Voice chat flooded, you will be able use voice chats again in (%d) seconds", g_voiceFloodPenalty.integer));

			return;
		}
	}

	G_GetChatPrefix ( ent, target, mode, name, sizeof(name) );

	if ( target ) 
	{
		G_VoiceTo( ent, target, mode, name, id, voiceonly );
		return;
	}

	// send it to all the apropriate clients
	for (int j = 0; j < level.maxclients; j++) 
	{
		other = &g_entities[j];
		G_VoiceTo( ent, other, mode, name, id, voiceonly );
	}
}

/*
==================
Cmd_Voice_f
==================
*/
static void Cmd_Voice_f( gentity_t *ent, int mode, bool arg0, bool voiceonly ) 
{
	char		*p;

	if ( trap_Argc () < 2 && !arg0 ) {
		return;
	}

	if (arg0)
	{
		p = ConcatArgs( 0 );
	}
	else
	{
		p = ConcatArgs( 1 );
	}

	G_Voice( ent, NULL, mode, p, voiceonly );
}

/*
==================
Cmd_Where_f
==================
*/
void Cmd_Where_f( gentity_t *ent ) 
{
	trap_SendServerCommand( ent-g_entities, va("print \"%s\n\"", vtos( ent->s.origin ) ) );
}

/*
============
G_VoteDisabled

determins if the given vote is disabled
============
*/
int G_VoteDisabled ( const char* callvote ) 
{
	return trap_Cvar_VariableIntegerValue( va("novote_%s", callvote) );
}

/*
==================
Cmd_CallVote_f
==================
*/
void Cmd_CallVote_f( gentity_t *ent ) 
{
	int		i;
	char	arg1[MAX_STRING_TOKENS];
	char	arg2[MAX_STRING_TOKENS];

	if ( !g_allowVote.integer ) 
	{
		infoMsgToClients(ent - g_entities, "Voting not allowed here");
		return;
	}

	if ( level.intermissiontime || level.intermissionQueued )
	{
		infoMsgToClients(ent - g_entities, "Voting not allowed during intermission");
		return;
	}

	// No voting within the minute of a map change
	if ( level.time - level.startTime < 1000 * 60 )
	{
		infoMsgToClients(ent - g_entities, "Cannot vote within the first minute of a map change");
		return;
	}

	if ( level.numConnectedClients > 1 && level.numVotingClients == 1 ) 
	{
		infoMsgToClients(ent - g_entities, "You need at least 2 clients to call a vote");
		return;
	}

	if ( level.voteTime ) 
	{
		infoMsgToClients(ent - g_entities, "A vote is already in progress");
		return;
	}

	if ( ent->client->pers.voteCount >= MAX_VOTE_COUNT ) 
	{
		infoMsgToClients(ent - g_entities, "You have called the maximum number of votes");
		return;
	}
	
	if ( ent->client->sess.team == TEAM_SPECTATOR ) 
	{
		infoMsgToClients(ent - g_entities, "Not allowed to call a vote as spectator");
		return;
	}

	if ( ent->client->voteDelayTime > level.time )
	{
		infoMsgToClients(ent - g_entities, va("You are not allowed to vote within %d minute of a failed vote", g_failedVoteDelay.integer));
		return;
	}
		
	// Save the voting client id
	level.voteClient = ent->s.number;

	// make sure it is a valid command to vote on
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );

	if( strchr( arg1, ';' ) || strchr( arg2, ';' ) ) 
	{
		infoMsgToClients(ent - g_entities, "Invalid vote string");
		return;
	}

	if ( !strcmp( arg1, "map_restart" ) ) {
	} else if ( !strcmp( arg1, "mapcycle" ) ) {
	} else if ( !strcmp( arg1, "map" ) ) {
	} else if ( !strcmp( arg1, "rmgmap" ) ) {
	} else if ( !strcmp( arg1, "g_gametype" ) ) {
	} else if ( !strcmp( arg1, "kick" ) ) {
	} else if ( !strcmp( arg1, "clientkick" ) ) {
	} else if ( !strcmp( arg1, "g_doWarmup" ) ) {
	} else if ( !strcmp( arg1, "timelimit" ) ) {
	} else if ( !strcmp( arg1, "timeextension" ) ) {
	} else if ( !strcmp( arg1, "scorelimit" ) ) {
	} else 
	{
		infoMsgToClients(ent - g_entities, "Invalid vote string");
		infoMsgToClients(ent - g_entities, "Vote commands are: map_restart, nextmap, map <mapname>, g_gametype <n>, kick <player>, clientkick <clientnum>, g_doWarmup, timelimit <time>, scorelimit <score>");
		return;
	}

	// see if this particular vote is disabled
	if ( G_VoteDisabled ( arg1 ) )
	{
		infoMsgToClients(ent - g_entities, va("The '%s' vote has been disabled on this server", arg1));
		return;
	}	

	// if there is still a vote to be executed
	if ( level.voteExecuteTime ) 
	{
		level.voteExecuteTime = 0;
		trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", level.voteString ) );
	}

	// special case for g_gametype, check for bad values
	if ( !strcmp( arg1, "g_gametype" ) ) 
	{
		// Verify the gametype
		i = BG_FindGametype ( arg2 );
		if ( i < 0 )
		{
			infoMsgToClients(ent - g_entities, "Invalid gametype");
			return;
		}	

		sprintf_s( level.voteString, sizeof( level.voteString ), "%s %s", arg1, arg2 );
		sprintf_s( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s %s", arg1, bg_gametypeData[i].name );
	} 
	else if ( !strcmp( arg1, "map" ) ) 
	{
		sprintf_s( level.voteString, sizeof( level.voteString ), "%s %s", arg1, arg2 );
		sprintf_s( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
	} 
	else if ( !strcmp( arg1, "rmgmap" ) ) 
	{
		char	arg3[MAX_STRING_TOKENS];
		char	arg4[MAX_STRING_TOKENS];

		trap_Argv( 3, arg3, sizeof( arg3 ) );
		trap_Argv( 4, arg4, sizeof( arg4 ) );

		sprintf_s( level.voteString, sizeof( level.voteString ), "rmgmap 1 %s 2 %s 3 %s 4 \"%s\" 0", arg2, arg3, arg4, ConcatArgs ( 5 ) );
		sprintf_s( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
	}
	else if ( !strcmp( arg1, "mapcycle" ) ) 
	{
		if (!*g_mapcycle.string || !strcmp ( g_mapcycle.string, "none" ) ) 
		{
			infoMsgToClients(ent - g_entities, "there is no map cycle ccurently set up");
			return;
		}

		sprintf_s( level.voteString, sizeof( level.voteString ), "mapcycle");
		sprintf_s( level.voteDisplayString, sizeof( level.voteDisplayString ), "nextmap" );
	} 
	else if ( !strcmp ( arg1, "clientkick" ) )
	{
		int n = atoi ( arg2 );

		if ( n < 0 || n >= MAX_CLIENTS )
		{
			infoMsgToClients(ent - g_entities, va("invalid client number %d", n));
			return;
		}

		if ( g_entities[n].client->pers.connected == CON_DISCONNECTED )
		{
			infoMsgToClients(ent - g_entities, va("there is no client with the client number %d", n));
			return;
		}
			
		sprintf_s ( level.voteString, sizeof(level.voteString ), "%s %s", arg1, arg2 );
		sprintf_s(level.voteDisplayString, sizeof(level.voteDisplayString), "kick %s", g_entities[n].client->pers.netname.c_str());
	}
	else if ( !strcmp ( arg1, "timeextension" ) )
	{
		if ( !g_timelimit.integer )
		{
			infoMsgToClients(ent - g_entities, va("There is no timelimit to extend"));
			return;
		}

		if ( !g_timeextension.integer )
		{
			infoMsgToClients(ent - g_entities, va("This server does not allow time extensions"));
			return;
		}
		sprintf_s ( level.voteString, sizeof(level.voteString ), "extendtime %d", g_timeextension.integer );
		sprintf_s ( level.voteDisplayString, sizeof(level.voteDisplayString), "extend timelimit by %d minutes", g_timeextension.integer );
	}
	else 
	{
		sprintf_s( level.voteString, sizeof( level.voteString ), "%s \"%s\"", arg1, arg2 );
		sprintf_s( level.voteDisplayString, sizeof( level.voteDisplayString ), "%s", level.voteString );
	}

	infoMsgToClients(-1, va("%s called a vote", ent->client->pers.netname.c_str()));

	// start the voting, the caller autoamtically votes yes
	level.voteTime = level.time;
	level.voteYes = 1;
	level.voteNo = 0;

	for ( i = 0 ; i < level.maxclients ; i++ ) 
	{
		level.clients[i].ps.eFlags &= ~EF_VOTED;
	}
	ent->client->ps.eFlags |= EF_VOTED;

	trap_SetConfigstring( CS_VOTE_TIME, va("%i,%i", level.voteTime, g_voteDuration.integer*1000 ) );
	trap_SetConfigstring( CS_VOTE_STRING, level.voteDisplayString );	
	trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteYes ) );
	trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteNo ) );	
	trap_SetConfigstring( CS_VOTE_NEEDED, va("%i", level.numVotingClients / 2 ) );
}

/*
==================
Cmd_Vote_f
==================
*/
void Cmd_Vote_f( gentity_t *ent ) 
{
	char msg[64];

	if ( !level.voteTime ) 
	{
		infoMsgToClients(ent - g_entities, "No vote in progress");
		return;
	}

	if ( ent->client->ps.eFlags & EF_VOTED ) 
	{
		infoMsgToClients(ent - g_entities, "Vote already cast");
		return;
	}

	if ( ent->client->sess.team == TEAM_SPECTATOR ) 
	{
		infoMsgToClients(ent - g_entities, "Not allowed to vote as spectator");
		return;
	}

	infoMsgToClients(ent - g_entities, "Vote cast");

	ent->client->ps.eFlags |= EF_VOTED;

	trap_Argv( 1, msg, sizeof( msg ) );

	if ( msg[0] == 'y' || msg[1] == 'Y' || msg[1] == '1' ) 
	{
		level.voteYes++;
		trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteYes ) );
	} 
	else 
	{
		level.voteNo++;
		trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteNo ) );	
	}

	// a majority will be determined in CheckVote, which will also account
	// for players entering or leaving
}


/*
=================
Cmd_SetViewpos_f
=================
*/
void Cmd_SetViewpos_f( gentity_t *ent ) 
{
	vec3_t		origin, angles;
	char		buffer[MAX_TOKEN_CHARS];

	if ( !g_cheats.integer ) 
	{
		infoMsgToClients(ent - g_entities, va("Cheats are not enabled on this server"));
		return;
	}
	
	if ( trap_Argc() != 5 ) 
	{
		infoMsgToClients(ent - g_entities, va("usage: setviewpos x y z yaw"));
		return;
	}

	VectorClear( angles );
	for (int i = 0 ; i < 3 ; i++ ) 
	{
		trap_Argv( i + 1, buffer, sizeof( buffer ) );
		origin[i] = atof( buffer );
	}

	trap_Argv( 4, buffer, sizeof( buffer ) );
	angles[YAW] = atof( buffer );

	TeleportPlayer( ent, origin, angles );
}

//Some wrappers for new command system
void Cmd_Say_All(gentity_t *ent){
	Cmd_Say_f(ent, SAY_ALL, false);
}

void Cmd_Say_Team(gentity_t *ent){
	Cmd_Say_f(ent, SAY_TEAM, false);
}

void Cmd_Voice(gentity_t *ent){
	Cmd_Voice_f(ent, SAY_TEAM, false, false);
}

void Cmd_FollowCycle_next(gentity_t *ent){
	Cmd_FollowCycle_f(ent, 1);
}

void Cmd_FollowCycle_prev(gentity_t *ent){
	Cmd_FollowCycle_f(ent, -1);
}
// End wrappers

static cmd_t consoleCommands[] =
{
	{ "say", &Cmd_Say_All },
	{ "say_team", &Cmd_Say_Team },
	{ "tell", &Cmd_Tell_f },
	{ "vsay_team", &Cmd_Voice },
	{ "score", &Cmd_Score_f },
	{ "team", &Cmd_Team_f},

	{ "drop", &Cmd_Drop_f },
	{ "dropitem", &Cmd_DropItem_f },
	{ "give", &Cmd_Give_f },
	{ "god", &Cmd_God_f },
	{ "noclip", &Cmd_Noclip_f },
	{ "kill", &Cmd_Kill_f },
	{ "levelshot", &Cmd_LevelShot_f },
	{ "follow", &Cmd_Follow_f },
	{ "follownext", &Cmd_FollowCycle_next },
	{ "followprev", &Cmd_FollowCycle_prev },
	{ "where", &Cmd_Where_f },
	{ "callvote", &Cmd_CallVote_f },
	{ "vote", &Cmd_Vote_f },
	{ "setviewpos", &Cmd_SetViewpos_f },
};

static int consoleCommandsSize = sizeof(consoleCommands) / sizeof(consoleCommands[0]);

/*
=================
ClientCommand
=================
*/
void ClientCommand( int clientNum ) {
	gentity_t *ent;
	char	cmd[MAX_TOKEN_CHARS];

	ent = g_entities + clientNum;
	if ( !ent->client ) {
		return;		// not fully in game yet
	}

	trap_Argv( 0, cmd, sizeof( cmd ) );

	for (int i = 0; i < consoleCommandsSize; i++){
		if (consoleCommands[i].cmd == cmd){
			consoleCommands[i].Function(ent);
			return;
		}
	}
	infoMsgToClients(clientNum, va("Unknown command: %s", cmd));
}
