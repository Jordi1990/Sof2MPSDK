#include "gametype_inf.h"
#define ITEM_BRIEFCASE 100
#define TRIGGER_EXTRACTION 200

Gametype_inf::Gametype_inf() {
	// Register the items
	// Boe!Man 11/29/12: Register items per gametype.
	gitem_t *item = BG_FindItem("briefcase");
	if (item){
		item->quantity = ITEM_BRIEFCASE;
	}
	// Boe!Man 11/29/12: Register triggers per gametype.
	gentity_t *find = NULL;
	while (NULL != (find = G_Find(find, FOFS(classname), "gametype_trigger")))
	{
		if (strcmp(find->targetname, (const char*) "briefcase_destination"))
		{
			continue;
		}

		// Assign the id to it.
		find->health = TRIGGER_EXTRACTION;
		find->touch = gametype_trigger_touch;
		trap_LinkEntity(find);
	}

	caseTakenSound = G_SoundIndex("sound/ctf_flag.mp3");
	caseCaptureSound = G_SoundIndex("sound/ctf_win.mp3");
	caseReturnSound = G_SoundIndex("sound/ctf_return.mp3");
}

void Gametype_inf::onItemStuck(gitem_t *item){
	G_ResetGametypeItem(item);
	trap_SetConfigstring(CS_GAMETYPE_MESSAGE, va("%i,%s", level.time + 5000, va("@The Briefcase has returned!")));
	trap_SendServerCommand(-1, va("print \"^3[INF] ^7The briefcase has returned.\n\""));
	// Boe!Man 11/29/12: Global sound.
	if (!level.intermissionQueued && !level.intermissiontime){
		gentity_t* tent;
		tent = G_TempEntity(vec3_origin, EV_GLOBAL_SOUND);
		tent->s.eventParm = caseReturnSound;
		tent->r.svFlags = SVF_BROADCAST;
	}
	G_ResetGametypeItem(item);
}

bool Gametype_inf::onItemTouch(gitem_t *item, gclient_s *other){
	if (other->sess.team == TEAM_BLUE){
		trap_SetConfigstring(CS_GAMETYPE_MESSAGE, va("%i,%s", level.time + 5000, va("@%s ^7has taken the briefcase!", other->pers.netname.c_str()))); // Taken.
		trap_SendServerCommand(-1, va("print\"^3[INF] %s ^7has taken the briefcase.\n\"", other->pers.netname.c_str()));
		// Boe!Man 11/29/12: Global sound.
		if (!level.intermissionQueued && !level.intermissiontime){
			gentity_t* tent;
			tent = G_TempEntity(vec3_origin, EV_GLOBAL_SOUND);
			tent->s.eventParm = caseTakenSound;
			tent->r.svFlags = SVF_BROADCAST;
		}

		// Boe!Man 11/29/12: Radio message.
		G_Voice(&g_entities[other->ps.clientNum], NULL, SAY_TEAM, "got_it", qfalse);
		return true;
	}
	else
		return false;
}

void Gametype_inf::update(){
	Gametype::defaultUpdate();
}

void Gametype_inf::onTeamEliminated(team_t team){
	trap_SetConfigstring(CS_GAMETYPE_MESSAGE, va("%i,%s", level.time + 5000, va("@%s ^7team eliminated!", (team == TEAM_RED)?"Red":"Blue"))); // Red team eliminated.
	trap_SendServerCommand(-1, va("print\"^3[INF] ^7%s team eliminated.\n\"", (team == TEAM_RED) ? "Red" : "Blue"));
	G_AddTeamScore(team, 1);
	// Boe!Man 11/29/12: Global sound.
	if (!level.intermissionQueued && !level.intermissiontime){
		gentity_t* tent;
		tent = G_TempEntity(vec3_origin, EV_GLOBAL_SOUND);
		tent->s.eventParm = caseCaptureSound;
		tent->r.svFlags = SVF_BROADCAST;
	}

	// Boe!Man 11/29/12: Reset gametype.
	level.gametypeResetTime = level.time + 5000;
}

void Gametype_inf::onGametimeExpired(){
	trap_SetConfigstring(CS_GAMETYPE_MESSAGE, va("%i,%s", level.time + 5000, va("@Red ^7team has defended the briefcase!"))); // Red team defended the briefcase.
	trap_SendServerCommand(-1, va("print\"^3[INF] ^7Red team has defended the briefcase.\n\""));
	G_AddTeamScore((team_t)TEAM_RED, 1);

	// Boe!Man 11/29/12: Reset gametype.
	level.gametypeResetTime = level.time + 5000;
}

void Gametype_inf::onItemDropped(gentity_t *ent){
	trap_SetConfigstring(CS_GAMETYPE_MESSAGE, va("%i,%s", level.time + 5000, va("@%s ^7has dropped the briefcase!", ent->client->pers.netname.c_str()))); // Dropped.
}

bool Gametype_inf::onTriggerTouch(gentity_t *trigger, gentity_t *other){
	if (trigger->health == TRIGGER_EXTRACTION){
		gitem_t*	item;

		item = BG_FindGametypeItemByID(ITEM_BRIEFCASE);

		if (item)
		{
			if (other->client->ps.stats[STAT_GAMETYPE_ITEMS] & (1 << item->giTag))
			{
				trap_SetConfigstring(CS_GAMETYPE_MESSAGE, va("%i,%s", level.time + 5000, va("@%s ^7has captured the briefcase!", other->client->pers.netname.c_str())));
				trap_SendServerCommand(-1, va("print\"^3[INF] %s ^7has captured the briefcase.\n\"", other->client->pers.netname.c_str()));
				// Boe!Man 11/29/12: Global sound.
				if (!level.intermissionQueued && !level.intermissiontime){
					gentity_t* tent;
					tent = G_TempEntity(vec3_origin, EV_GLOBAL_SOUND);
					tent->s.eventParm = caseCaptureSound;
					tent->r.svFlags = SVF_BROADCAST;
				}

				G_AddTeamScore(other->client->sess.team, 1);
				G_AddScore(other, 10);

				// Boe!Man 11/29/12: Reset gametype.
				level.gametypeResetTime = level.time + 5000;
			}
		}
		return true;
	}
	return false;
}

void Gametype_inf::onItemDefend(gentity_t *ent){
	G_AddScore(ent, 5);
}