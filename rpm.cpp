#include "g_local.h"

void RPM_UpdateTMI()
{
	string tmiString;
	// if we are waiting for the level to restart, do nothing, even if forced
	if (level.restarted)
		return;

	if (level.time - level.lastTMIUpdate < g_clientTMIUpdate.integer) // Henk 06/04/10 -> Increase to reduce lag
		return;

	for (int i = 0; i < level.numConnectedClients; i++)
	{
		gclient_t *cl = &level.clients[level.sortedClients[i]];
		gentity_t *bestLoc = Team_GetLocation(&g_entities[level.sortedClients[i]]);
		int location = 0;
		if (bestLoc)
		{
			location = bestLoc->health;
		}
		userinfo userInfo(level.sortedClients[i]);

		char entry[32];
		sprintf_s(entry, 32,
			" %i %i %i %i %i %i %i %i %i",
			level.sortedClients[i],
			cl->ps.stats[STAT_HEALTH],
			cl->ps.stats[STAT_ARMOR],
			location,
			userInfo.cg_thirdPerson, // 1 = third | 0 = first | 2 = n/a, aka no client
			0, // admin
			cl->ps.weapon,
			0, // mute
			0  // clan member
			);
		tmiString.append(entry);
	}
	for (int i = 0; i < level.numConnectedClients; i++){
		gclient_t *client = &level.clients[level.sortedClients[i]];
		if (client->pers.connected != CON_CONNECTED) // Henk 24/02/2014 -> Do not send updated TMI to non connected clients
			continue;
		// Boe!Man 6/5/13: Don't send if the player is timing out..
		if (client->ps.ping >= 999)
			continue;

		//RxCxW - 1.20.2005 - #scoreboard #Version compatiblity
		if (client->rpmClient > 0.6)
			trap_SendServerCommand(i, va("tmi %i%s", level.numConnectedClients, tmiString.c_str()));
		//else if (cl->sess.proClient > 4.0)
		//	trap_SendServerCommand(level.sortedClients[i], va("tmi %i%s", numAdded, infoString));

	}

	level.lastTMIUpdate = level.time;
}