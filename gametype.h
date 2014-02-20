#ifndef GAMETYPE_H
#define GAMETYPE_H

#include "g_local.h"

class Gametype {
public:
	virtual void onItemStuck(gitem_t *item) = 0;
	virtual bool onItemTouch(gitem_t *item, gclient_s *other) = 0;
	virtual void update() = 0;
	virtual void onTeamEliminated(team_t team) = 0;
	virtual void onGametimeExpired() = 0;
	virtual void onItemDropped(gentity_t *ent) = 0;
	virtual bool onTriggerTouch(gentity_t *trigger, gentity_t *other) = 0;
	virtual void onItemDefend(gentity_t *ent) = 0;

	void defaultUpdate(){
		// If the level is over then forget checking gametype stuff.
		if (level.intermissiontime)
		{
			return;
		}

		// Dont bother starting the gametype until
		// the first person joins
		if (!level.gametypeStartTime)
		{
			int counts[TEAM_NUM_TEAMS];

			memset(counts, 0, sizeof(counts));

			for (int i = 0; i < level.numConnectedClients; i++)
			{
				gentity_t* other = &g_entities[level.sortedClients[i]];

				if (other->client->pers.connected != CON_CONNECTED)
				{
					continue;
				}

				counts[other->client->sess.team]++;
			}

			if (counts[TEAM_RED] || counts[TEAM_BLUE])
			{
				G_ResetGametype();
				return;
			}
		}

		// Check for delayed gametype reset
		if (level.gametypeResetTime)
		{
			if (level.time > level.gametypeResetTime)
			{
				// Dont do this again
				level.gametypeResetTime = 0;

				G_ResetGametype();
			}

			return;
		}

		// Handle respawn interval spawning
		if (level.gametypeData->respawnType == RT_INTERVAL)
		{
			for (int team = TEAM_RED; team < TEAM_SPECTATOR; team++)
			{
				if (level.gametypeRespawnTime[team] && level.time > level.gametypeRespawnTime[team])
				{
					// Respawn all dead clients
					G_RespawnClients(false, (team_t)team);

					// Next interval
					level.gametypeRespawnTime[team] = 0;
				}
			}
		}

		// If we are in RT_NONE respawn mode then we need to look for everyone being dead
		if (level.time > level.gametypeDelayTime && level.gametypeData->respawnType == RT_NONE && level.gametypeStartTime)
		{
			int alive[TEAM_NUM_TEAMS];
			int dead[TEAM_NUM_TEAMS];

			memset(&alive[0], 0, sizeof(alive));
			memset(&dead[0], 0, sizeof(dead));
			for (int i = 0; i < level.numConnectedClients; i++)
			{
				gentity_t* ent = &g_entities[level.sortedClients[i]];

				if (ent->client->pers.connected != CON_CONNECTED)
				{
					continue;
				}

				if (G_IsClientDead(ent->client))
				{
					dead[ent->client->sess.team] ++;
				}
				else
				{
					alive[ent->client->sess.team] ++;
				}
			}

			// If everyone is dead on a team then reset the gametype, but only if 
			// there was someone on that team to begin with.
			if (!alive[TEAM_RED] && dead[TEAM_RED])
			{
				onTeamEliminated(TEAM_RED);
			}
			else if (!alive[TEAM_BLUE] && dead[TEAM_BLUE])
			{
				onTeamEliminated(TEAM_BLUE);
			}

			// See if the time has expired
			if (level.time > level.gametypeRoundTime)
			{
				onGametimeExpired();
			}
		}
	}
	int caseTakenSound;
	int caseCaptureSound;
	int caseReturnSound;
};
#endif
