// Copyright (C) 2001-2002 Raven Software
//
// g_bot.c

#include "g_local.h"

int				g_numArenas;
static char		*g_arenaInfos[MAX_ARENAS];

float trap_Cvar_VariableValue( const char *var_name ) {
	char buf[128];

	trap_Cvar_VariableStringBuffer(var_name, buf, sizeof(buf));
	return atof(buf);
}

/*
===============
G_ParseInfos
===============
*/
int G_ParseInfos( const char *buf, int max, char *infos[] ) {
	int			count;
	char		key[MAX_TOKEN_CHARS];
	char		info[MAX_INFO_STRING];

	count = 0;

	while ( 1 ) {
		const char *token = COM_Parse(&buf);
		if ( !token[0] ) {
			break;
		}
		if ( strcmp( token, "{" ) ) {
			Com_Printf( "Missing { in info file\n" );
			break;
		}

		if ( count == max ) {
			Com_Printf( "Max infos exceeded\n" );
			break;
		}

		info[0] = '\0';
		while ( 1 ) {
			token = COM_ParseExt( &buf, true );
			if ( !token[0] ) {
				Com_Printf( "Unexpected end of info file\n" );
				break;
			}
			if ( !strcmp( token, "}" ) ) {
				break;
			}
			strncpy( key, token, sizeof( key ) );

			token = COM_ParseExt( &buf, false );
			if ( !token[0] ) 
			{
				Info_SetValueForKey( info, key, "<NULL>" );
			}
			else
			{
				Info_SetValueForKey( info, key, token );
			}
		}
		//NOTE: extra space for arena number
		infos[count] = (char *)trap_VM_LocalAlloc(strlen(info) + strlen("\\num\\") + strlen(va("%d", MAX_ARENAS)) + 1);
		if (infos[count]) {
			strcpy(infos[count], info);
			count++;
		}
	}
	return count;
}

/*
===============
G_LoadArenasFromFile
===============
*/
static void G_LoadArenasFromFile( char *filename ) {
	int				len;
	fileHandle_t	f;
	char			buf[MAX_ARENAS_TEXT];

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( !f ) {
		trap_Print( va( S_COLOR_RED "file not found: %s\n", filename ) );
		return;
	}
	if ( len >= MAX_ARENAS_TEXT ) {
		trap_Print( va( S_COLOR_RED "file too large: %s is %i, max allowed is %i", filename, len, MAX_ARENAS_TEXT ) );
		trap_FS_FCloseFile( f );
		return;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );

	g_numArenas += G_ParseInfos( buf, MAX_ARENAS - g_numArenas, &g_arenaInfos[g_numArenas] );
}

/*
===============
G_LoadArenas
===============
*/
void G_LoadArenas( void ) 
{
	char		dirlist[1024];
	int			dirlen;

	g_numArenas = 0;

	// get all arenas from .arena files
	int numdirs = trap_FS_GetFileList("scripts", ".arena", dirlist, 1024 );
	char *dirptr  = dirlist;
	for (int i = 0; i < numdirs; i++, dirptr += dirlen+1) 
	{
		char filename[128];
		dirlen = strlen(dirptr);
		strcpy(filename, "scripts/");
		strcat_s<128>(filename, dirptr);
		G_LoadArenasFromFile(filename);
	}
	
	for(int n = 0; n < g_numArenas; n++ ) 
	{
		Info_SetValueForKey( g_arenaInfos[n], "num", va( "%i", n ) );
	}
}

/*
===============
G_GetArenaInfoByNumber
===============
*/
const char *G_GetArenaInfoByMap( const char *map ) 
{
	for(int n = 0; n < g_numArenas; n++ ) 
	{
		if( strcmp( Info_ValueForKey( g_arenaInfos[n], "map" ), map ) == 0 ) 
		{
			return g_arenaInfos[n];
		}
	}

	return NULL;
}

/*
===============
G_DoesMapSupportGametype

determines whether or not the current map supports the given gametype
===============
*/
bool G_DoesMapSupportGametype ( const char* gametype )
{
	char		mapname[MAX_QPATH];
	const char* info;
	const char*	type;

	// Figure out the current map name first
	if ( RMG.integer )
	{
		sprintf_s ( mapname, MAX_QPATH, "*random" );
	}
	else
	{
		trap_Cvar_VariableStringBuffer ( "mapname", mapname, MAX_QPATH );
	}

	// Get the arena info for the current map 
	info = G_GetArenaInfoByMap ( mapname );
	if ( !info )
	{
		// If they dont have an area file for this map then
		// just assume it supports all gametypes
		return true;
	}

	// Get the supported gametypes
	type = Info_ValueForKey( info, "type" );

	while ( 1 )
	{
		char* token = COM_Parse(&type);
		if ( !token || !*token )
		{
			break;
		}

		if ( strcmp ( gametype, token ) == 0 )
		{
			return true;
		}
	}

	return false;
}