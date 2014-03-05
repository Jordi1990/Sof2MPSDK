#include "g_local.h"
#include <boost/algorithm/string.hpp>

// Henk 04/05/10 -> New command system
// Henk 28/2/14 -> Updated
typedef struct
{
	string	shortCmd; // short admin command, ex: !uc, !p(with space)
	string	adminCmd; // full adm command for /adm and rcon
	int		*adminLevel; // pointer to cvar value because we can't store a non constant value, so we store a pointer :).
	void(*Function)(gentity_t *, gentity_t *, int); // store pointer to the given function so we can call it later
	int param;
	bool aliveOnly; // commands only on alive ppl?
	bool otherAdmins; // command can be used on other admins
	bool getClient; // true if a commands needs a client parameter, false for commands with a string parameter or no parameter
} admCmd_t;

int getParamClient(gentity_t *ent, int argNum, bool shortCmd, bool aliveOnly, bool otherAdmins){
	char argBuf[16];
	bool space = false;
	string multipleClientString;
	trap_Argv(argNum, argBuf, 16);
	string arg = argBuf;
	int num = -1;
	bool secondTry = false;
	if (shortCmd){ // Henk 03/03/14 -> Handle the short admin commands.
		secondtry:
		for (unsigned int i = 0; i < arg.length(); i++){
			if (arg[i] == ' ' && arg.length() > 3){
				string idStr;
				idStr = arg[i + 1] + arg[i+2];
				try{
					num = boost::lexical_cast<int>(idStr);
				}
				catch (...){
					string argName;
					for (unsigned int z = i + 1; z < arg.length(); z++){
						if (arg[z] == ' ')
							break;
						argName += arg[z];
					}
					num = -1;
					string multipleMessage;
					int numClientsFound = 0;
					for (int i = 0; i<level.numConnectedClients; i++){
						//trap_SendServerCommand(-1, va("print\"^3[Debug] ^7%s comparing with %s.\n\"", g_entities[level.sortedClients[i]].client->pers.cleanName.c_str(), arg.c_str()));
						if (boost::icontains(g_entities[level.sortedClients[i]].client->pers.cleanName.c_str(), argName.c_str())){
							num = level.sortedClients[i];
							numClientsFound++;
							char test[64];
							sprintf_s<64>(test, "^1[#%i] ^7 %s, ", level.sortedClients[i], g_entities[level.sortedClients[i]].client->pers.cleanName.c_str());
							multipleMessage += test;
						}
					}
					if (numClientsFound > 1){
						multipleMessage.pop_back(); // pop space
						multipleMessage.pop_back(); // pop comma
						if (ent && ent->client)
							infoMsgToClients(ent->s.number, va("Multiple names found with: ^3%s^7: %s", arg.c_str(), multipleMessage.c_str()));
						else
							Com_Printf("Multiple names found with: ^3%s^7: %s", arg.c_str(), multipleMessage.c_str());
						return -1;
					}
				}
			}
			else if (isdigit(arg[i]) && isalpha(arg[i - 1])){ // Henk 10/03/11 -> Add support for !u1/!r1 commands
				string strId;
				for (unsigned int x = i; x <= arg.length(); x++){
					if (isdigit(arg[x])){
						strId += arg[x];
					}
					else
						break;
				}
				try{
					num = boost::lexical_cast<int>(strId);
				}
				catch (...){
					num = -1;
				}
				trap_SendServerCommand(ent->s.number, va("print\"^3[Debug] ^7Short command style, Client number: %i.\n\"", num));
			}
		}

		if (num == -1 && !secondTry){ // Get second argument because they use it from the console
			trap_Argv(2, argBuf, 16);
			arg = argBuf;
			secondTry = true;
			goto secondtry;
		}
	}
	else{
		try {
			num = boost::lexical_cast<int>(arg);
		}
		catch (...){
			int numClientsFound = 0;
			num = -1;
			string multipleMessage;
			for (int i = 0; i<level.numConnectedClients; i++){
				//trap_SendServerCommand(-1, va("print\"^3[Debug] ^7%s comparing with %s.\n\"", g_entities[level.sortedClients[i]].client->pers.cleanName.c_str(), arg.c_str()));
				if (boost::icontains(g_entities[level.sortedClients[i]].client->pers.cleanName.c_str(), arg.c_str())){
					num = level.sortedClients[i];
					numClientsFound++;
					char test[64];
					sprintf_s<64>(test, "^1[#%i] ^7%s, ", level.sortedClients[i], g_entities[level.sortedClients[i]].client->pers.cleanName.c_str());
					multipleMessage += test;
				}
			}
			if (numClientsFound > 1){
				multipleMessage.pop_back(); // pop space
				multipleMessage.pop_back(); // pop comma
				if (ent && ent->client)
					infoMsgToClients(ent->s.number, va("Multiple names found with: ^3%s^7: %s", arg.c_str(), multipleMessage.c_str()));
				else
					Com_Printf("Multiple names found with: ^3%s^7: %s", arg.c_str(), multipleMessage.c_str());
				return -1;
			}
		}
	}

	if (num < 0 || num >= g_maxclients.integer)
	{
		if (ent && ent->client)
			infoMsgToClients(ent - g_entities, va("You haven't entered a valid player ID/player name"));
		else
			Com_Printf("You haven't entered a valid player ID/player name.\n");
		return -1;
	}
	else if (g_entities[num].client->pers.connected != CON_CONNECTED){
		if (ent && ent->client)
			infoMsgToClients(ent - g_entities, va("This player is not connected"));
		else
			Com_Printf("This player is not connected.\n");
		return -1;
	}
	else if ((ent && ent->client) && g_entities[num].client->pers.adminLevel >= ent->client->pers.adminLevel && !otherAdmins && ent && ent->client){
		infoMsgToClients(ent - g_entities, "You cannot use this command on other Admins");
		return -1;
	}
	else if (G_IsClientDead(g_entities[num].client) && aliveOnly){
		if (ent && ent->client)
			infoMsgToClients(ent - g_entities, va("You cannot use this command on a dead player"));
		else
			Com_Printf("You cannot use this command on a dead player.\n");
		return -1;
	}
	else if (G_IsClientSpectating(g_entities[num].client) && aliveOnly){
		if (ent && ent->client)
			infoMsgToClients(ent - g_entities, va("You cannot use this command on a spectator"));
		else
			Com_Printf("You cannot use this command on a spectator.\n");
		return -1;
	}

	return num;
}

void addAdmin(gentity_t *other, gentity_t *ent, int admLevel){
	other->client->pers.adminLevel = (adm_t)admLevel;
	string adminLevel = adminLevelToString((adm_t)admLevel);

	if (ent && ent->client)
		trap_SendServerCommand(-1, va("print\"^3[Admin Action] ^7%s ^7was made %s ^7by %s.\n\"", other->client->pers.netname.c_str(), adminLevel.c_str(), ent->client->pers.netname.c_str()));
	else
		trap_SendServerCommand(-1, va("print\"^3[Rcon Action] ^7%s ^7is now a %s.\n\"", other->client->pers.netname.c_str(), adminLevel.c_str()));

	trap_SetConfigstring(CS_GAMETYPE_MESSAGE, va("%i,@^7%s ^7is now a %s", level.time + 5000, other->client->pers.netname.c_str(), adminLevel.c_str()));
}

void uppercut(gentity_t *other, gentity_t *ent, int param){
	other->client->ps.pm_flags |= PMF_JUMPING;
	other->client->ps.groundEntityNum = ENTITYNUM_NONE;
	other->client->ps.velocity[2] = 1000;

	G_Sound(other, G_SoundIndex("sound/weapons/rpg7/fire01.mp3"));

	if (ent && ent->client)
		trap_SendServerCommand(-1, va("print\"^3[Admin Action] %s ^7was uppercut by %s.\n\"", other->client->pers.netname.c_str(), ent->client->pers.netname.c_str()));
	else
		trap_SendServerCommand(-1, va("print\"^3[Rcon Action] %s ^7was uppercut.\n\"", other->client->pers.netname.c_str()));

	trap_SetConfigstring(CS_GAMETYPE_MESSAGE, va("%i,@%s ^7was uppercut", level.time + 5000, other->client->pers.netname.c_str()));
}

void weaponEdit(gentity_t *, gentity_t *ent, int argNum){
	char argBuf[16];
	trap_Argv(argNum, argBuf, 16);
	string arg = argBuf;
	string weaponName;
	bool spaceDone = false;
	for (unsigned int i = 0; i < arg.length(); i++){
		if (arg[i] == ' ')
			spaceDone = true;
		else if (spaceDone)
			weaponName += arg[i];			
	}

	for (int weapon = WP_KNIFE; weapon < WP_NUM_WEAPONS; weapon++)
	{
		gitem_t* item = BG_FindWeaponItem((weapon_t)weapon);
		if (!item)
		{
			continue;
		}

		if (boost::icontains(item->pickup_name, weaponName.c_str())){
			// disable or enable weapon
			int value = (int)trap_Cvar_VariableValue(va("disable_%s", item->classname));
			trap_Cvar_Set(va("disable_%s", item->classname), (value == 0)?"1":"0");
			if (ent && ent->client)
				trap_SendServerCommand(-1, va("print\"^3[Admin Action] ^7Weapon %s has been %s by %s.\n\"", item->pickup_name, (value == 1) ? "enabled" : "disabled", ent->client->pers.netname.c_str()));
			else
				trap_SendServerCommand(-1, va("print\"^3[Rcon Action] ^7Weapon %s has been %s.\n\"", item->pickup_name, (value == 1) ? "enabled" : "disabled"));

			trap_SetConfigstring(CS_GAMETYPE_MESSAGE, va("%i,@Weapon %s has been %s", level.time + 5000, item->pickup_name, (value == 1) ? "enabled" : "disabled"));
			globalSound(G_SoundIndex("sound/misc/menus/click.wav"));
			BG_UpdateAvailableWeapons();
			return;
		}
	}
	infoMsgToClients(ent->s.clientNum, va("Weapon %s does not exist", weaponName.c_str()));
}

void gameTypeRestart(gentity_t *, gentity_t *ent, int argNum){
		trap_SetConfigstring(CS_GAMETYPE_MESSAGE, va("%i,@Gametype restart!", level.time + 5000));
		globalSound(G_SoundIndex("sound/misc/menus/click.wav"));
		if (ent && ent->client){
			trap_SendServerCommand(-1, va("print\"^3[Admin Action] ^7Gametype restart by %s.\n\"", ent->client->pers.netname.c_str()));
		}
		else{
			trap_SendServerCommand(-1, va("print\"^3[Rcon Action] ^7Gametype restart.\n\""));
		}
		trap_SendConsoleCommand(EXEC_APPEND, va("gametype_restart\n"));
}

static admCmd_t AdminCommands[] =
{
	{ "!uppercut", "uppercut", &g_uppercut.integer, &uppercut, 0, true, true, true },
	{ "!addbadmin", "addbadmin", &g_addbadmin.integer, &addAdmin, 2, false, false, true },
	{ "!addadmin", "addadmin", &g_addadmin.integer, &addAdmin, 3, false, false, true },
	{ "!addsadmin", "addsadmin", &g_addsadmin.integer, &addAdmin, 4, false, false, true },

	{ "!gr", "gametyperestart", &g_uppercut.integer, &gameTypeRestart, 0, true, true, false },
	{ "!wp", "weapon", &g_uppercut.integer, &weaponEdit, 0, true, true, false },
	{ "!ab", "addbadmin", &g_addbadmin.integer, &addAdmin, 2, false, false, true },
	{ "!aa", "addadmin", &g_addadmin.integer, &addAdmin, 3, false, false, true },
	{ "!as", "addsadmin", &g_addsadmin.integer, &addAdmin, 4, false, false, true },
	{ "!uc", "uppercut", &g_uppercut.integer, &uppercut, 0, true, true, true },
	{ "!u", "uppercut", &g_uppercut.integer, &uppercut, 0, true, true, true },
	/*
	// Boe!Man 1/22/11: Adding full synonyms.
	{ "!pop", "pop", &g_pop.integer, &Boe_pop },
	{ "!kick", "kick", &g_kick.integer, &Boe_Kick },
	{ "!twist", "twist", &g_twist.integer, &Boe_Twist },
	{ "!untwist", "untwist", &g_twist.integer, &Boe_unTwist },
	{ "!plant", "plant", &g_plant.integer, &Boe_Plant },
	{ "!unplant", "unplant", &g_plant.integer, &Boe_unPlant },
	{ "!runover", "runover", &g_runover.integer, &Boe_Runover },
	{ "!respawn", "respawn", &g_respawn.integer, &Boe_Respawn },
	{ "!maprestart", "maprestart", &g_mapswitch.integer, &Boe_MapRestart },
	{ "!burn", "burn", &g_burn.integer, &Boe_Burn },
	{ "!mute", "mute", &g_mute.integer, &Boe_XMute },
	{ "!unmute", "unmute", &g_mute.integer, &Boe_UnMute },
	{ "!strip", "strip", &g_strip.integer, &Boe_Strip },
	{ "!removeadmin", "removeadmin", &g_removeadmin.integer, &Boe_Remove_Admin_f },
	{ "!forceteam", "forceteam", &g_forceteam.integer, &Adm_ForceTeam },
	{ "!nolower", "nolower", &g_nosection.integer, &Boe_NoLower },
	{ "!noroof", "noroof", &g_nosection.integer, &Boe_NoRoof },
	{ "!nomiddle", "nomiddle", &g_nosection.integer, &Boe_NoMiddle },
	{ "!nowhole", "nowhole", &g_nosection.integer, &Boe_NoWhole },
	{ "!shuffleteams", "shuffleteams", &g_shuffleteams.integer, &Boe_ShuffleTeams },
	{ "!shuffle", "shuffle", &g_shuffleteams.integer, &Boe_ShuffleTeams },
	{ "!nonades", "nonades", &g_nades.integer, &Boe_NoNades },
	{ "!scorelimit", "scorelimit", &g_sl.integer, &Boe_ScoreLimit },
	{ "!timelimit", "timelimit", &g_tl.integer, &Boe_TimeLimit },
	{ "!respawninterval", "respawninterval", &g_ri.integer, &Boe_RespawnInterval },
	{ "!realdamage", "realdamage", &g_damage.integer, &Boe_RealDamage },
	{ "!normaldamage", "normaldamage", &g_damage.integer, &Boe_NormalDamage },
	{ "!gametyperestart", "gametyperestart", &g_gr.integer, &Boe_GametypeRestart },
	{ "!addclan", "addclan", &g_clan.integer, &Boe_Add_Clan_Member },
	{ "!removeclan", "removeclan", &g_clan.integer, &Boe_Remove_Clan_Member },
	{ "!removeclanlist", "removeclanlist", &g_clan.integer, &Boe_removeClanMemberFromList },
	{ "!clanlistremove", "clanlistremove", &g_clan.integer, &Boe_removeClanMemberFromList },
	{ "!compmode", "compmode", &g_cm.integer, &Boe_CompMode },
	{ "!competitionmode", "compmode", &g_cm.integer, &Boe_CompMode },
	{ "!banlist", "banlist", &g_ban.integer, &Henk_BanList },
	{ "!ban", "ban", &g_ban.integer, &Boe_Ban_f },
	{ "!broadcast", "broadcast", &g_broadcast.integer, &Boe_Broadcast },
	{ "!subnetbanlist", "subnetbanlist", &g_subnetban.integer, &Henk_SubnetBanList },
	{ "!eventeams", "eventeams", &g_eventeams.integer, &Henk_EvenTeams },
	{ "!clanvsall", "clanvsall", &g_clanvsall.integer, &Henk_CVA },
	{ "!swapteams", "swapteams", &g_swapteams.integer, &Henk_SwapTeams },
	{ "!lock", "lock", &g_lock.integer, &Henk_Lock },
	{ "!unlock", "unlock", &g_lock.integer, &Henk_Unlock },
	{ "!flash", "flash", &g_flash.integer, &Henk_Flash },
	{ "!gametype", "gametype", &g_mapswitch.integer, &Henk_Gametype },
	{ "!unpause", "unpause", &g_pause.integer, &Henk_Unpause },
	{ "!pause", "pause", &g_pause.integer, &Henk_Pause },
	{ "!unban", "unban", &g_ban.integer, &Henk_Unban },
	{ "!subnetunban", "subnetunban", &g_subnetban.integer, &Henk_SubnetUnban },
	{ "!subnetban", "subnetban", &g_subnetban.integer, &Boe_subnetBan },
	{ "!pass", "passvote", &g_forcevote.integer, &Boe_passVote },
	{ "!cancel", "cancelvote", &g_forcevote.integer, &Boe_cancelVote },
	{ "!mapcycle", "mapcycle", &g_mapswitch.integer, &Boe_Mapcycle },
	{ "!adminlist", "adminlist", &g_adminlist.integer, &Henk_Admlist },
	{ "!clanlist", "clanlist", &g_clan.integer, &Boe_clanList },
	{ "!adminremove", "adminremove", &g_adminremove.integer, &Henk_AdminRemove },
	{ "!friendlyfire", "friendlyfire", &g_ff.integer, &Boe_friendlyFire },
	// Boe!Man 1/22/11: End full synonyms.

	{ "!adr", "adminremove", &g_adminremove.integer, &Henk_AdminRemove },
	{ "!tw", "twist", &g_twist.integer, &Boe_Twist },
	{ "!utw", "untwist", &g_twist.integer, &Boe_unTwist },
	{ "!pl", "plant", &g_plant.integer, &Boe_Plant },
	{ "!upl", "unplant", &g_plant.integer, &Boe_unPlant },
	{ "!rounds", "rounds", &g_cm.integer, &Boe_Rounds },
	{ "!ro", "runover", &g_runover.integer, &Boe_Runover },
	{ "!rs", "respawn", &g_respawn.integer, &Boe_Respawn }, // this is how we add synonyms
	{ "!mr", "maprestart", &g_mapswitch.integer, &Boe_MapRestart },
	{ "!mr", "map_restart", &g_mapswitch.integer, &Boe_MapRestart },
	{ "!map", "map", &g_mapswitch.integer, &Henk_Map },
	{ "!altmap", "altmap", &g_mapswitch.integer, &Henk_Map },
	{ "!devmap", "devmap", &g_mapswitch.integer, &Henk_Map },
	{ "!um", "unmute", &g_mute.integer, &Boe_UnMute },
	{ "!st", "strip", &g_strip.integer, &Boe_Strip },
	{ "!ra", "removeadmin", &g_removeadmin.integer, &Boe_Remove_Admin_f },
	{ "!ft", "forceteam", &g_forceteam.integer, &Adm_ForceTeam },
	{ "!nl", "nolower", &g_nosection.integer, &Boe_NoLower },
	{ "!nr", "noroof", &g_nosection.integer, &Boe_NoRoof },
	{ "!nm", "nomiddle", &g_nosection.integer, &Boe_NoMiddle },
	{ "!nw", "nowhole", &g_nosection.integer, &Boe_NoWhole },
	{ "!sh", "shuffleteams", &g_shuffleteams.integer, &Boe_ShuffleTeams },
	{ "!nn", "nonades", &g_nades.integer, &Boe_NoNades },
	{ "!sl", "scorelimit", &g_sl.integer, &Boe_ScoreLimit },
	{ "!tl", "timelimit", &g_tl.integer, &Boe_TimeLimit },
	{ "!ri", "respawninterval", &g_ri.integer, &Boe_RespawnInterval },
	{ "!rd", "realdamage", &g_damage.integer, &Boe_RealDamage },
	{ "!nd", "normaldamage", &g_damage.integer, &Boe_NormalDamage },
	{ "!gr", "gametyperestart", &g_gr.integer, &Boe_GametypeRestart },
	{ "!acl", "addclan", &g_clan.integer, &Boe_Add_Clan_Member },
	{ "!rc", "removeclan", &g_clan.integer, &Boe_Remove_Clan_Member },
	{ "!rcl", "removeclanlist", &g_clan.integer, &Boe_removeClanMemberFromList },
	{ "!clr", "clanlistremove", &g_clan.integer, &Boe_removeClanMemberFromList },
	{ "!3rd", "3rd", &g_3rd.integer, &Boe_Third },
	{ "!third", "third", &g_3rd.integer, &Boe_Third },
	{ "!cm", "compmode", &g_cm.integer, &Boe_CompMode },
	{ "!bl", "banlist", &g_ban.integer, &Henk_BanList },
	{ "!ba", "ban", &g_ban.integer, &Boe_Ban_f },
	{ "!br", "broadcast", &g_broadcast.integer, &Boe_Broadcast },
	{ "!sbl", "subnetbanlist", &g_subnetban.integer, &Henk_SubnetBanList },
	{ "!et", "eventeams", &g_eventeams.integer, &Henk_EvenTeams },
	{ "!cva", "clanvsall", &g_clanvsall.integer, &Henk_CVA },
	{ "!sw", "swapteams", &g_swapteams.integer, &Henk_SwapTeams },
	{ "!l", "lock", &g_lock.integer, &Henk_Lock },
	{ "!ul", "unlock", &g_lock.integer, &Henk_Unlock },
	{ "!fl", "flash", &g_flash.integer, &Henk_Flash },
	{ "!g", "gametype", &g_mapswitch.integer, &Henk_Gametype },
	{ "!gt", "gametype", &g_mapswitch.integer, &Henk_Gametype },
	{ "!pv", "passvote", &g_forcevote.integer, &Boe_passVote },
	{ "!cv", "cancelvote", &g_forcevote.integer, &Boe_cancelVote },
	{ "!upa", "unpause", &g_pause.integer, &Henk_Unpause },
	{ "!up", "unpause", &g_pause.integer, &Henk_Unpause },
	{ "!pa", "pause", &g_pause.integer, &Henk_Pause },
	{ "!uba", "unban", &g_ban.integer, &Henk_Unban },
	{ "!sbu", "subnetunban", &g_subnetban.integer, &Henk_SubnetUnban },
	{ "!ub", "unban", &g_ban.integer, &Henk_Unban },
	{ "!su", "subnetunban", &g_subnetban.integer, &Henk_SubnetUnban },
	{ "!sb", "subnetban", &g_subnetban.integer, &Boe_subnetBan },
	{ "!mc", "mapcycle", &g_mapswitch.integer, &Boe_Mapcycle },
	{ "!adl", "adminlist", &g_adminlist.integer, &Henk_Admlist },
	{ "!al", "adminlist", &g_adminlist.integer, &Henk_Admlist },
	{ "!cl", "clanlist", &g_clan.integer, &Boe_clanList },
	{ "!r", "respawn", &g_respawn.integer, &Boe_Respawn },
	{ "!b", "burn", &g_burn.integer, &Boe_Burn },
	{ "!p", "pop", &g_pop.integer, &Boe_pop },
	{ "!k", "kick", &g_kick.integer, &Boe_Kick },
	{ "!m", "mute", &g_mute.integer, &Boe_XMute },
	{ "!s", "strip", &g_strip.integer, &Boe_Strip },
	{ "!ff", "friendlyfire", &g_ff.integer, &Boe_friendlyFire },
	*/
};

static int AdminCommandsSize = sizeof(AdminCommands) / sizeof(AdminCommands[0]);

bool IsValidCommand(const string &cmd, const string &str){
	bool space = false;
	for (unsigned int i = 0; i <= str.length(); i++){
		if (str[i] == ' ')
			space = true;
		if (str[i] == '!'){
			for (unsigned int z = 1; z <= cmd.length() - 1; z++){
				if (str[i + z] != cmd[z]){
					break; // invalid command
				}
				else if (space){
					return false;
				}
				else if (z == cmd.length() - 1){
					if (isdigit(str[(i + z) + 1]))
						return true;
					else if (!str[(i + z) + 1])
						return true;
					else if (str[(i + z) + 1] != ' ')
						return false;
				}
			}
		}
	}
	return true;
}

// This command should always be used before an admin command.
// It will process the given arguments and do all the checks before executing the command
// After the execution it will call the proper logging functions
void doAdminCommand(int commandIndex, gentity_t *ent, bool shortCmd, int argNum, bool aliveOnly, bool otherAdmins, bool getClient){
	if (getClient){
			int paramClientId = getParamClient(ent, argNum, shortCmd, aliveOnly, otherAdmins);
			if (paramClientId < 0)
				return;
			gentity_t *other = &g_entities[paramClientId];
			AdminCommands[commandIndex].Function(other, ent, AdminCommands[commandIndex].param);
			globalSound(G_SoundIndex("sound/misc/menus/click.wav"));
	}
	else{ // no client, commands like !cm / !gt
		AdminCommands[commandIndex].Function(NULL, ent, argNum);
	}
}

bool doAdminCommands(char *cmd){
	for (int i = 0; i<AdminCommandsSize; i++){
		if (boost::iequals(AdminCommands[i].adminCmd, cmd)){
			doAdminCommand(i, NULL, false, 1, AdminCommands[i].aliveOnly, AdminCommands[i].otherAdmins, AdminCommands[i].getClient);
			return true;
		}
	}
	return false;
}

void doShortCommandAdminCheck(gentity_t *ent, const char *p){
	string param = p;
	for (int i = 0; i<AdminCommandsSize; i++){
		if (boost::icontains(param.c_str(), AdminCommands[i].shortCmd.c_str()) && IsValidCommand(AdminCommands[i].shortCmd, param)){
			if (ent->client->pers.adminLevel >= *AdminCommands[i].adminLevel){
				doAdminCommand(i, ent, 1, true, AdminCommands[i].aliveOnly, AdminCommands[i].otherAdmins, AdminCommands[i].getClient);
				break;
			}
			else{
				infoMsgToClients(ent - g_entities, "Your Admin level is too low to use this command");
			}
		}
	}
}