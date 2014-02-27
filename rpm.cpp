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
		if (client->pers.rpmClient > 0.6)
			trap_SendServerCommand(i, va("tmi %i%s", level.numConnectedClients, tmiString.c_str()));
		//else if (cl->sess.proClient > 4.0)
		//	trap_SendServerCommand(level.sortedClients[i], va("tmi %i%s", numAdded, infoString));

	}

	level.lastTMIUpdate = level.time;
}

void clientRefresh(gentity_t *ent)
{
	if (level.time <= level.gametypeStartTime + 5000){
		infoMsgToClients(ent->s.number, "You shouldn't refresh at the start of a new round");
		return;
	}
	if (!G_IsClientSpectating(ent->client) && !G_IsClientDead(ent->client))
	{
		ent->flags &= ~FL_GODMODE;
		ent->client->ps.stats[STAT_HEALTH] = ent->health = -999;
		player_die(ent, ent, ent, 100000, MOD_REFRESH, HL_NONE, vec3_origin);
	}

	memset(&ent->client->pers.statinfo, 0, sizeof(ent->client->pers.statinfo));
	ent->client->pers.enterTime = level.time;
	ent->client->ps.persistant[PERS_SCORE] = 0;
	ent->client->sess.score = 0;
	ent->client->sess.teamkillDamage = 0;
	ent->client->sess.teamkillForgiveTime = 0;
	ent->client->pers.statinfo.lastclient_hurt = -1;
	ent->client->pers.statinfo.lasthurtby = -1;
}


const char	*G_PlaceString(int rank) {
	static char	str[64];
	char	*s, *t;

	if (rank & RANK_TIED_FLAG) {
		rank &= ~RANK_TIED_FLAG;
		t = "Tied for ";
	}
	else {
		t = "";
	}

	if (rank == 1) {
		s = S_COLOR_BLUE "1st" S_COLOR_WHITE;		// draw in blue
	}
	else if (rank == 2) {
		s = S_COLOR_RED "2nd" S_COLOR_WHITE;		// draw in red
	}
	else if (rank == 3) {
		s = S_COLOR_YELLOW "3rd" S_COLOR_WHITE;		// draw in yellow
	}
	else if (rank == 11) {
		s = "11th";
	}
	else if (rank == 12) {
		s = "12th";
	}
	else if (rank == 13) {
		s = "13th";
	}
	else if (rank % 10 == 1) {
		s = va("%ist", rank);
	}
	else if (rank % 10 == 2) {
		s = va("%ind", rank);
	}
	else if (rank % 10 == 3) {
		s = va("%ird", rank);
	}
	else {
		s = va("%ith", rank);
	}

	sprintf_s(str, sizeof(str), "%s%s", t, s);
	return str;
}

void RPM_Obituary(gentity_t *target, gentity_t *attacker, int mod, attackType_t attack, int hitLocation)
{
	int				targ, killer, attackt, weapon;
	char			*message;
	char			*message2;
	char			*message3;
	bool		headShot = false;
	bool		statOk = false;
	statinfo_t		*atrstat = &attacker->client->pers.statinfo;

	if (!attacker)
		return;
	if (!target)
		return;
	if (!attacker->client)
		return;
	message2 = "";
	message3 = "";
	if (!level.gametypeData->teams || (level.gametypeData->teams && !OnSameTeam(target, attacker)))
		statOk = true;

	hitLocation = hitLocation & (~HL_DISMEMBERBIT);
	if (hitLocation == HL_HEAD)
	{
		message3 = "^7{^3HeaDShoT^7}";
		attackt = attacker->client->pers.statinfo.attack;
		weapon = attacker->client->pers.statinfo.weapon;
		headShot = qtrue;
		//add to the total headshot count for this player
		atrstat->headShotKills++;
		atrstat->weapon_headshots[attackt][weapon]++;
	}

	targ = target->s.number;

	if (targ < 0 || targ >= MAX_CLIENTS)
	{
		G_LogPrintf("Out of range..\n");
		Com_Error(ERR_FATAL, "RPM_Obituary: target out of range");
	}

	//find out who or what killed the client
	if (attacker->client)
	{
		killer = attacker->s.number;
	}
	else
	{
		killer = ENTITYNUM_WORLD;
	}

	// Play the death sound, water if they drowned
	if (mod == MOD_WATER)
	{
		G_Sound(target, G_SoundIndex("sound/pain_death/mullins/drown_dead.mp3"));
	}
	else		//play a random death sound out of 3 possible sounds
	{
		switch (level.time % 3)
		{
		case 0:
			G_Sound(target, G_SoundIndex("sound/pain_death/male/die01.mp3"));
			break;

		case 1:
			G_Sound(target, G_SoundIndex("sound/pain_death/male/die04.mp3"));
			break;

		case 2:
			G_Sound(target, G_SoundIndex("sound/pain_death/male/die05.mp3"));
			break;

		default:
			G_Sound(target, G_SoundIndex("sound/pain_death/male/die01.mp3"));
			break;
		}
	}
	// Play the frag sound, and make sure its not played more than every 250ms
	if (attacker->client)
	{
		if (killer == targ)
		{
			G_Sound(target, G_SoundIndex("sound/self_frag.mp3"));
		}
		else if (headShot)
		{
			if (attacker->client->pers.rpmClient)
			{
				trap_SendServerCommand(attacker->s.number, va("headshot \"Headshot\n\""));
				G_Sound(attacker, G_SoundIndex("sound/npc/col8/blakely/niceshot.mp3"));
			}
			else
			{
				trap_SendServerCommand(attacker->s.number, va("cp \"Headshot\n\""));//g_headShotMessage.string));
				G_Sound(attacker, G_SoundIndex("sound/npc/col8/blakely/niceshot.mp3"));
			}

			if (level.gametypeData->showKills)
			{
				message2 = "Headshot!\n";//g_headShotMessage.string );
			}

			message3 = "^7{^3HeaDShoT^7}";
		}
		//if not headshot, suicide or tk just play the normal sound
		else
		{
			G_Sound(attacker, G_SoundIndex("sound/frag.mp3"));
		}

		//set the time here now 
		attacker->client->lastKillTime = level.time;
	}
	switch (mod)
	{
	case MOD_SUICIDE:
		message = "suicides";
		break;
	case MOD_FALLING:
		message = "fell to his death";
		break;
	case MOD_CRUSH:
		message = "was squished";
		break;
	case MOD_WATER:
		message = "sank like a rock";
		break;
	case MOD_TARGET_LASER:
		message = "saw the light";
		break;
	case MOD_TRIGGER_HURT:
		message = "was in the wrong place";
		break;
	case MOD_TEAMCHANGE:
		return;
	case MOD_CAR:
		message = "was killed in a terrible car accident";
		break;
	case MOD_POP:
		return;
		//RxCxW - 1.13.2005	- Dugup (Unplant) #MOD
	case MOD_DUGUP:
		message = "Looks like someone Dug too deep!";
		break;
	case MOD_BURN:
		message = "was BURNT to a Crisp!";
		break;
	default:
		message = NULL;
		break;
	}
	// Attacker killed themselves.  Ridicule them for it.
	if (killer == targ)
	{
		switch (mod)
		{
		case MOD_MM1_GRENADE_LAUNCHER:
		case MOD_RPG7_LAUNCHER:
		case MOD_M67_GRENADE:
		case MOD_M84_GRENADE:
		case MOD_F1_GRENADE:
		case MOD_L2A2_GRENADE:
		case MOD_MDN11_GRENADE:
		case MOD_SMOHG92_GRENADE:
		case MOD_ANM14_GRENADE:
		case MOD_M15_GRENADE:
			message = "blew himself up";
			break;

		case MOD_REFRESH:
			message = "Refreshed";
			break;

		default:
			message = "killed himself";
			break;
		}
	}
	if (message)
	{
		infoMsgToClients(-1, va("%s ^7%s", target->client->pers.netname.c_str(), message));
		return;
	}
	// check for kill messages
	if (level.gametypeData->showKills)
	{
		if (attacker && attacker->client)
		{
			//if not a team game display the kill and the rank
			if (!level.gametypeData->teams)
			{
				trap_SendServerCommand(attacker->s.number, va("cp \"You killed %s\n%s place with %i\n\"", // %s after \n
					target->client->pers.netname.c_str(),
					G_PlaceString(attacker->client->ps.persistant[PERS_RANK] + 1),
					attacker->client->ps.persistant[PERS_SCORE]));
			}
			else //if team just display the kill
			{
				trap_SendServerCommand(attacker->s.number, va("cp \"You killed %s\n\"", target->client->pers.netname.c_str()));
			}
		}
	}
	// check for double client messages
	message2 = "";

	if (killer != ENTITYNUM_WORLD)
	{
		switch (mod)
		{
		case MOD_KNIFE:
			message = "was sliced by";
			if (statOk)
				atrstat->knifeKills++;
			break;

		case MOD_USAS_12_SHOTGUN:
		case MOD_M590_SHOTGUN:
			if (attack == ATTACK_ALTERNATE)
			{
				message = "was bludgeoned by";
				message2 = va("'s %s", weaponParseInfo[mod].mName);
			}
			else
			{
				message = "was pumped full of lead by";
				message2 = va("'s %s", weaponParseInfo[mod].mName);
			}
			break;

		case MOD_M1911A1_PISTOL:
		case MOD_USSOCOM_PISTOL:
			if (attack == ATTACK_ALTERNATE)
			{
				message = "was pistol whipped by";
				message2 = va("'s %s", weaponParseInfo[mod].mName);
			}
			else
			{
				message = "was shot by";
				message2 = va("'s %s", weaponParseInfo[mod].mName);
			}
			break;

		case MOD_AK74_ASSAULT_RIFLE:
			if (attack == ATTACK_ALTERNATE)
			{
				message = "was stabbed by";
				message2 = va("'s %s", weaponParseInfo[mod].mName);
			}
			else
			{
				message = "was shot by";
				message2 = va("'s %s", weaponParseInfo[mod].mName);
			}
			break;

		case MOD_M4_ASSAULT_RIFLE:
			if (attack == ATTACK_ALTERNATE)
			{
				message = "was detonated by";
				message2 = "'s M203";

				if (statOk)
					atrstat->explosiveKills++;
			}
			else
			{
				message = "was shot by";
				message2 = va("'s %s", weaponParseInfo[mod].mName);
			}
			break;

		case MOD_M60_MACHINEGUN:
		case MOD_MICRO_UZI_SUBMACHINEGUN:
		case MOD_M3A1_SUBMACHINEGUN:
			message = "was shot by";
			message2 = va("'s %s", weaponParseInfo[mod].mName);
			break;

		case MOD_MSG90A1_SNIPER_RIFLE:
			message = "was sniped by";
			message2 = va("'s %s", weaponParseInfo[mod].mName);
			break;

		case MOD_MM1_GRENADE_LAUNCHER:
		case MOD_RPG7_LAUNCHER:
		case MOD_M67_GRENADE:
		case MOD_M84_GRENADE:
		case MOD_F1_GRENADE:
		case MOD_L2A2_GRENADE:
		case MOD_MDN11_GRENADE:
		case MOD_SMOHG92_GRENADE:
		case MOD_ANM14_GRENADE:
		case MOD_M15_GRENADE:
			message = "was detonated by";
			message2 = va("'s %s", weaponParseInfo[mod].mName);
			if (statOk)
			{
				if (mod == MOD_ANM14_GRENADE)
				{
					atrstat->hitcount++;
					atrstat->accuracy = (float)atrstat->hitcount / (float)atrstat->shotcount * 100;
					atrstat->weapon_hits[ATTACK_NORMAL][mod]++; // MIGHT BE BUG
				}

				atrstat->explosiveKills++;
			}
			break;

		case MOD_TELEFRAG:
			message = "was telefragged by";
			break;

		default:
			message = "was killed by";
			break;
		}

		if (message) {
			infoMsgToClients(-1, va("%s ^7%s %s^7%s %s", target->client->pers.netname.c_str(), message, attacker->client->pers.netname.c_str(), message2, message3));
			return;
		}
	}
	infoMsgToClients(-1, va("%s died", target->client->pers.netname.c_str()));
}