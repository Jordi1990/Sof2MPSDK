// Copyright (C) 2001-2002 Raven Software
//

#include "g_local.h"

/*
==============
TeamName
==============
*/
const char *TeamName(team_t team)  
{
	switch ( team )
	{
		case TEAM_RED:
			return "RED";

		case TEAM_BLUE:
			return "BLUE";

		case TEAM_FREE:
			return "FREE";

		case TEAM_SPECTATOR:
			return "SPECTATOR";
	}

	return "";
}

/*
==============
G_AddTeamScore

used for gametype > GT_TDM
for gametype GT_TDM the level.teamScores is updated in AddScore in g_combat.c
==============
*/
void G_AddTeamScore( team_t team, int score ) 
{
	// Dont allow negative scores to affect the team score.  The reason for this is 
	// that negative scores come from the actions of one bad player and a single player
	// can cause a team to loose because he/she wants to just kill the rest of their team, or
	// continue to kill themselves.
	if ( score < 0 )
	{
		return;
	}

	level.teamScores[ team ] += score;
}

/*
==============
OnSameTeam
==============
*/
bool OnSameTeam( gentity_t *ent1, gentity_t *ent2 ) 
{
	if ( !ent1->client || !ent2->client ) 
	{
		return false;
	}

	if ( !level.gametypeData->teams ) 
	{
		return false;
	}

	if ( ent1->client->sess.team == ent2->client->sess.team ) 
	{
		return true;
	}

	return false;
}

/*
===========
Team_GetLocation

Report a location for the player. Uses placed nearby target_location entities
============
*/
gentity_t *Team_GetLocation(gentity_t *ent)
{
	gentity_t		*eloc, *best;
	float			bestlen, len;
	vec3_t			origin;

	best = NULL;
	bestlen = 3*8192.0*8192.0;

	VectorCopy( ent->r.currentOrigin, origin );

	for (eloc = level.locationHead; eloc; eloc = eloc->nextTrain) 
	{
		len = ( origin[0] - eloc->r.currentOrigin[0] ) * ( origin[0] - eloc->r.currentOrigin[0] )
			+ ( origin[1] - eloc->r.currentOrigin[1] ) * ( origin[1] - eloc->r.currentOrigin[1] )
			+ ( origin[2] - eloc->r.currentOrigin[2] ) * ( origin[2] - eloc->r.currentOrigin[2] );

		if ( len > bestlen ) 
		{
			continue;
		}

		if ( !trap_InPVS( origin, eloc->r.currentOrigin ) ) 
		{
			continue;
		}

		bestlen = len;
		best = eloc;
	}

	return best;
}


/*
===========
Team_GetLocationMsg
============
*/
bool Team_GetLocationMsg ( gentity_t *ent, char *loc, int loclen )
{
	gentity_t *best;

	best = Team_GetLocation( ent );
	
	if (!best)
	{
		return false;
	}

	sprintf_s(loc, loclen, "%s", best->message);

	return true;
}

