// Copyright (C) 2001-2002 Raven Software
//

const char*		TeamName				( team_t team );
void			G_AddTeamScore			( team_t team, int score );

gentity_t*		Team_GetLocation		( gentity_t *ent );
bool		Team_GetLocationMsg		( gentity_t *ent, char *loc, int loclen );


