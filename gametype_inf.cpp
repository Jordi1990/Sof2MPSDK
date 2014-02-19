#include "gametype_inf.h"
#define ITEM_BRIEFCASE 100
#define TRIGGER_EXTRACTION 200

Gametype_inf::Gametype_inf() {
	Com_Printf("Initializing gametype...\n");
	int start = level.time;
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
		//find->touch = gametype_trigger_touch;
		trap_LinkEntity(find);
	}

	caseTakenSound = G_SoundIndex("sound/ctf_flag.mp3");
	caseCaptureSound = G_SoundIndex("sound/ctf_win.mp3");
	caseReturnSound = G_SoundIndex("sound/ctf_return.mp3");

	Com_Printf("Gametype INF initialized in %dms\n", level.time - start);
}

void Gametype_inf::onItemStuck(gitem_t *item){
	G_ResetGametypeItem(item);
	trap_SetConfigstring(CS_GAMETYPE_MESSAGE, va("%i,%s", level.time + 5000, va("@The Briefcase has %sr%se%st%su%sr%sned!", server_color1.string, server_color2.string, server_color3.string, server_color4.string, server_color5.string, server_color6.string)));
	trap_SendServerCommand(-1, va("print \"^3[INF] ^7The briefcase has returned.\n\""));
	// Boe!Man 11/29/12: Global sound.
	if (!level.intermissionQueued && !level.intermissiontime){
		gentity_t* tent;
		tent = G_TempEntity(vec3_origin, EV_GLOBAL_SOUND);
		tent->s.eventParm = caseReturnSound;
		tent->r.svFlags = SVF_BROADCAST;
	}
}

bool Gametype_inf::onItemTouch(gitem_t *item, gclient_s *other){
	if (other->sess.team == TEAM_BLUE){
		trap_SetConfigstring(CS_GAMETYPE_MESSAGE, va("%i,%s", level.time + 5000, va("@%s ^7has %st%sa%sk%se%sn the briefcase!", other->pers.netname.c_str(), server_color2.string, server_color3.string, server_color4.string, server_color5.string, server_color6.string))); // Taken.
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
	
}

void Gametype_inf::onGametimeExpired(){

}