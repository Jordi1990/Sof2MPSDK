// Copyright (C) 2001-2002 Raven Software
//

// this file holds commands that can be executed by the server console, but not remote clients

#include "g_local.h"

char	*ConcatArgs( int start );

/*
===================
Svcmd_EntityList_f
===================
*/
void	Svcmd_EntityList_f (void) {
	gentity_t		*check;

	check = g_entities+1;
	for (int e = 1; e < level.num_entities ; e++, check++) {
		if ( !check->inuse ) {
			continue;
		}
		Com_Printf("%3i:", e);
		switch ( check->s.eType ) {
		case ET_GENERAL:
			Com_Printf("ET_GENERAL          ");
			break;
		case ET_PLAYER:
			Com_Printf("ET_PLAYER           ");
			break;
		case ET_ITEM:
			Com_Printf("ET_ITEM             ");
			break;
		case ET_MISSILE:
			Com_Printf("ET_MISSILE          ");
			break;
		case ET_MOVER:
			Com_Printf("ET_MOVER            ");
			break;
		case ET_BEAM:
			Com_Printf("ET_BEAM             ");
			break;
		case ET_PORTAL:
			Com_Printf("ET_PORTAL           ");
			break;
		case ET_SPEAKER:
			Com_Printf("ET_SPEAKER          ");
			break;
		case ET_PUSH_TRIGGER:
			Com_Printf("ET_PUSH_TRIGGER     ");
			break;
		case ET_TELEPORT_TRIGGER:
			Com_Printf("ET_TELEPORT_TRIGGER ");
			break;
		case ET_INVISIBLE:
			Com_Printf("ET_INVISIBLE        ");
			break;
		case ET_GRAPPLE:
			Com_Printf("ET_GRAPPLE          ");
			break;
		default:
			Com_Printf("%3i                 ", check->s.eType);
			break;
		}

		if ( check->classname ) {
			Com_Printf("%s", check->classname);
		}
		Com_Printf("\n");
	}
}


void Svcmd_ExtendTime_f (void) 
{
	char str[MAX_TOKEN_CHARS];
	int	 time;

	if ( trap_Argc() < 2 ) 
	{
		Com_Printf("Usage:  extendtime <minutes>\n");
		return;
	}

	trap_Argv( 1, str, sizeof( str ) );

	time = atoi(str);
	level.timeExtension += time;

	G_LogPrintf ( "timelimit extended by %d minutes\n", time );

	trap_SendServerCommand( -1, va("print \"timelimit extended by %d minutes\n\"", time) );
}

gclient_t	*ClientForString( const char *s ) {
	int			idnum;

	// numeric values are just slot numbers
	if ( s[0] >= '0' && s[0] <= '9' ) {
		idnum = atoi( s );
		if ( idnum < 0 || idnum >= level.maxclients ) {
			Com_Printf( "Bad client slot: %i\n", idnum );
			return NULL;
		}

		gclient_t *cl = &level.clients[idnum];
		if ( cl->pers.connected == CON_DISCONNECTED ) {
			Com_Printf( "Client %i is not connected\n", idnum );
			return NULL;
		}
		return cl;
	}

	// check for a name match
	for (int i=0 ; i < level.maxclients ; i++ ) {
		gclient_t *cl = &level.clients[i];
		if ( cl->pers.connected == CON_DISCONNECTED ) {
			continue;
		}
		if ( !cl->pers.netname.compare(s ) ) {
			return cl;
		}
	}

	Com_Printf( "User %s is not on the server\n", s );

	return NULL;
}

/*
===================
Svcmd_ForceTeam_f

forceteam <player> <team>
===================
*/
void Svcmd_ForceTeam_f( void ) 
{
	gclient_t	*cl;
	char		str[MAX_TOKEN_CHARS];

	// find the player
	trap_Argv( 1, str, sizeof( str ) );
	cl = ClientForString( str );
	if ( !cl ) 
	{
		return;
	}

	// set the team
	trap_Argv( 2, str, sizeof( str ) );
	SetTeam( &g_entities[cl - level.clients], str, NULL );
}

/*
===================
Svcmd_CancelVote_f

cancels the vote in progress
===================
*/
void Svcmd_CancelVote_f ( void )
{
	level.voteTime = 0;

	trap_SetConfigstring( CS_VOTE_TIME, "" );	

	trap_SendServerCommand( -1, "print \"Vote cancelled by admin.\n\"" );
}

/*
=================
ConsoleCommand
=================
*/
bool ConsoleCommand( void ) 
{
	char cmd[MAX_TOKEN_CHARS];

	trap_Argv(0, cmd, MAX_TOKEN_CHARS);

	if ( strcmp (cmd, "entitylist") == 0 ) 
	{
		Svcmd_EntityList_f();
		return true;
	}

	if ( strcmp (cmd, "forceteam") == 0 ) 
	{
		Svcmd_ForceTeam_f();
		return true;
	}

	if ( strcmp ( cmd, "cancelvote" ) == 0 )
	{
		Svcmd_CancelVote_f();
		return true;
	}

	if (strcmp (cmd, "gametype_restart" ) == 0 )
	{
		G_ResetGametype ( );
		return true;
	}

	if (strcmp (cmd, "extendtime" ) == 0 )
	{
		Svcmd_ExtendTime_f();
		return true;
	}

	if (g_dedicated.integer) 
	{
		if (strcmp (cmd, "say") == 0) 
		{
			trap_SendServerCommand( -1, va("chat -1 \"server: %s\n\"", ConcatArgs(1) ) );
			return true;
		}

		// everything else will also be printed as a say command
		trap_SendServerCommand( -1, va("chat -1 \"server: %s\n\"", ConcatArgs(0) ) );
		return true;
	}

	return false;
}

