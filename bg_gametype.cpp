// Copyright (C) 2001-2002 Raven Software
//
// bg_gametype.c -- dynamic gametype handling

#include "q_shared.h"
#include "bg_public.h"
#include "bg_local.h"

gametypeData_t	bg_gametypeData[MAX_GAMETYPES];
int				bg_gametypeCount;

/*
===============
BG_ParseGametypePhotos

Parses the photo information for objectives dialog
===============
*/
static bool BG_ParseGametypePhotos ( int gametypeIndex, TGPGroup group )
{
	TGPGroup photo;
	int		 index;

	// Convienience check
	if ( !group )
	{
		return true;
	}

	index = 0;
	photo = trap_GPG_GetSubGroups ( group );

	while ( photo )
	{
		char temp[1024];
		trap_GPG_GetName ( photo, temp );
		bg_gametypeData[gametypeIndex].photos[index].name = trap_VM_LocalStringAlloc ( temp );

		trap_GPG_FindPairValue ( photo, "displayname", "unknown", temp );
		bg_gametypeData[gametypeIndex].photos[index].displayName = trap_VM_LocalStringAlloc ( temp );

		index++;

		photo = trap_GPG_GetNext ( photo );
	}

	return true;
}

/*
===============
BG_ParseGametypeInfo

Parses minimal information about the given gametype.  For the most part
this information is the name of the gametype and some of its parameters.
===============
*/
static bool BG_ParseGametypeInfo ( int gametypeIndex )
{
	gametypeData_t*		gametype;
	TGenericParser2		GP2;
	TGPGroup			topGroup;
	TGPGroup			gtGroup;
	char				temp[1024];

	// Get the pointer for the gametype data
	gametype = &bg_gametypeData[gametypeIndex];
	
	// Open the gametype's script file
	GP2 = trap_GP_ParseFile( (char*)gametype->script, true, false);
	if (!GP2)
	{
		return false;
	}

	// Top group should only contain the "gametype" sub group
	topGroup = trap_GP_GetBaseParseGroup(GP2);
	if ( !topGroup )
	{
		return false;
	}

	// Grab the gametype sub group
	gtGroup = trap_GPG_FindSubGroup ( topGroup, "gametype" );
	if ( !gtGroup )
	{
		return false;
	}

	// Parse out the name of the gametype
	trap_GPG_FindPairValue ( gtGroup, "displayname", "", temp );
	if ( !temp[0] )
	{
		return false;
	}
	gametype->displayName = trap_VM_LocalStringAlloc ( temp );

	// Description
	trap_GPG_FindPairValue ( gtGroup, "description", "", temp );
	if ( temp[0] )
	{
		gametype->description = trap_VM_LocalStringAlloc ( temp );
	}

	// Are pickups enabled?
	trap_GPG_FindPairValue ( gtGroup, "pickups", "yes", temp );
	if ( !strcmp ( temp, "no" ) )
	{
		gametype->pickupsDisabled = true;
	}

	// Are teams enabled?
	trap_GPG_FindPairValue ( gtGroup, "teams", "yes", temp );
	if ( !strcmp ( temp, "yes" ) )
	{
		gametype->teams = true;
	}

	// Display kills
	trap_GPG_FindPairValue ( gtGroup, "showkills", "no", temp );
	if ( !strcmp ( temp, "yes" ) )
	{
		gametype->showKills = true;
	}

	// Look for the respawn type
	trap_GPG_FindPairValue ( gtGroup, "respawn", "normal", temp );
	if ( !strcmp ( temp, "none" ) )
	{
		gametype->respawnType = RT_NONE;
	}
	else if ( !strcmp ( temp, "interval" ) )
	{
		gametype->respawnType = RT_INTERVAL;
	}
	else
	{
		gametype->respawnType = RT_NORMAL;
	}

	// What percentage doest he backpack replenish?
	trap_GPG_FindPairValue ( gtGroup, "backpack", "0", temp );
	gametype->backpack = atoi(temp);

	// Get the photo information for objectives dialog
	BG_ParseGametypePhotos ( gametypeIndex, trap_GPG_FindSubGroup ( gtGroup, "photos" ) );

	// Cleanup the generic parser
	trap_GP_Delete ( &GP2 );

	return true;

}

/*
===============
BG_BuildGametypeList

Builds a list of the gametypes that are available and parses minimal
information about those gametypes.
===============
*/
bool BG_BuildGametypeList ( void )
{
	char		filename[MAX_QPATH];
	char		filelist[4096];
	char*		fileptr;
	char*		s;
	int			filelen;
	int			filecount;

	bg_gametypeCount = 0;

	// Retrieve the list of gametype files.  The returned list is a 
	// null separated list with the number of entries returned by the call
	filecount = trap_FS_GetFileList("scripts", ".gametype", filelist, 4096 );
	fileptr   = filelist;
	
	for (int i = 0; i < filecount; i++, fileptr += filelen+1) 
	{
		// Grab the length so we can skip this file later
		filelen = strlen(fileptr);

		// Build the full filename
		strcpy(filename, "scripts/");
		strcat(filename, fileptr );

		// Fill in what we know so far
		bg_gametypeData[bg_gametypeCount].script = trap_VM_LocalStringAlloc ( filename );

		// Kill the dot so we can use the filename as the short name
		s  = strchr ( fileptr, '.' );
		*s = '\0';
		bg_gametypeData[bg_gametypeCount].name   = trap_VM_LocalStringAlloc ( fileptr );
		
		// TODO: Parse the gametype file
		BG_ParseGametypeInfo ( bg_gametypeCount++ );
	}

	return true;
}

/*
===============
BG_FindGametype

Returns the gametype index using the given name. If the gametype isnt found
then -1 will be returned (and this is bad)
===============
*/
int BG_FindGametype ( const char* name )
{
	// Loop through the known gametypes and compare their names to 
	// the name given
	for (int i = 0; i < bg_gametypeCount; i ++ )
	{
		// Do the names match?
		if ( !strcmp ( bg_gametypeData[i].name, name )  )
		{
			return i;
		}
	}

	return -1;
}

/*
==============
BG_FindGametypeItem

Search through the item list for the gametype item with
the given index.
==============
*/
gitem_t	*BG_FindGametypeItem ( int index ) 
{
	return &bg_itemlist[index + MODELINDEX_GAMETYPE_ITEM];
}

/*
==============
BG_FindGametypeItemByID

Gametype will assign ids to the gametype items for them for future reference, the 
id is crammed into the quantity field of the gametype item.  This function will
find the gametype item with the given item id.
==============
*/
gitem_t *BG_FindGametypeItemByID ( int itemid )
{	
	for (int i = 0; i < MAX_GAMETYPE_ITEMS; i ++ )
	{
		if ( bg_itemlist[i + MODELINDEX_GAMETYPE_ITEM].quantity == itemid )
		{
			return &bg_itemlist[i + MODELINDEX_GAMETYPE_ITEM];
		}
	}

	return NULL;
}
