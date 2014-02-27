#include "g_local.h"

typedef struct chatSounds_s
{
	string text;
	int	 sound;
} chatSounds_t;

chatSounds_t chatSounds[MAX_CHAT_SOUNDS];

string defaultChatSounds[][2] =
{
	{ "Call for reinforcements", "sound/enemy/english/male/call_for_reinforcements.mp3" },
	{ "Advance", "sound/enemy/english/male/advance.mp3" },
	{ "Awaiting orders", "sound/enemy/english/male/awaiting_orders.mp3" },
	{ "Check the perimiter", "sound/enemy/english/male/check_peri.mp3" },
	{ "Go check that out", "sound/enemy/english/male/check_that_out.mp3" },
	{ "He's dangerous", "sound/enemy/english/male/dangerous.mp3" },
	{ "Did you hear that", "sound/enemy/english/male/did_you_hear.mp3" },
	{ "He's disappeared", "sound/enemy/english/male/disappeared.mp3" },
	{ "Drop your weapon", "sound/enemy/english/male/drop_your_weapon.mp3" },
	{ "Eliminate target", "sound/enemy/english/male/eliminate.mp3" },
	{ "GET HIM", "sound/enemy/english/male/get_him.mp3" },
	{ "I got that bastard", "sound/enemy/english/male/got_bastard.mp3" },
	{ "Hold here", "sound/enemy/english/male/hold_here.mp3" },
	{ "Hurry", "sound/enemy/english/male/hurry.mp3" },
	{ "I'm hit", "sound/npc/col8/blakely/imhit.mp3" },
	{ "Investigate that area", "sound/enemy/english/male/investigate_area.mp3" },
	{ "Keep looking", "sound/enemy/english/male/keep_looking.mp3" },
	{ "KILL HIM", "sound/enemy/english/male/kill_him" },
	{ "I killed him", "sound/enemy/english/male/killed_him.mp3" },
	{ "I lost him", "sound/enemy/english/male/lost_him.mp3" },
	{ "TAKING SERIOUS CASUALTIES", "sound/enemy/english/male/man_down03.mp3" },
	{ "I need help here", "sound/enemy/english/male/need_help.mp3" },
	{ "Medic Medic", "sound/enemy/english/male/medic_medic.mp3" },
	{ "Open Fire", "sound/enemy/english/male/open_fire.mp3" },
	{ "I'm out of Ammo", "sound/enemy/english/fmale/out_ammo.mp3" },
	{ "He's over here", "sound/enemy/english/male/over_here.mp3" },
	{ "Over there", "sound/enemy/english/male/over_there.mp3" },
	{ "Plug him", "sound/enemy/english/male/plug_him.mp3" },
	{ "Position reached", "sound/enemy/english/male/pos_reached.mp3" },
	{ "Secure the area", "sound/enemy/english/male/secure_the_area.mp3" },
	{ "We're getting slaughtered", "sound/enemy/english/male/slaughtered.mp3" },
	{ "Somethings not right here", "sound/enemy/english/male/something_not_right.mp3" },
	{ "Spread out", "sound/enemy/english/male/spread_out.mp3" },
	{ "Surround him", "sound/enemy/english/male/surround_him.mp3" },
	{ "Take cover", "sound/enemy/english/male/take_cover2.mp3" },
	{ "Take him out", "sound/enemy/english/male/take_him_out.mp3" },
	{ "Take Position", "sound/enemy/english/male/take_position.mp3" },
	{ "Take them out", "sound/enemy/english/male/take_them_out.mp3" },
	{ "Target has been eliminated", "sound/enemy/english/male/target_eliminate.mp3" },
	{ "There he is", "sound/enemy/english/male/there_he_is.mp3" },
	{ "Taking alot of fire", "sound/enemy/english/male/underfire03.mp3" },
	{ "WATCH OUT", "sound/enemy/english/male/watch_out.mp3" },
	{ "What are you doing here", "sound/enemy/english/male/what_are_you_doing.mp3" },
	{ "What the...", "sound/enemy/english/male/what_the.mp3" },
	{ "What was that", "sound/enemy/english/male/what_was_that.mp3" },
	{ "Whats happening here", "sound/enemy/english/male/whats_happening_here.mp3" },
	{ "Who are you", "sound/enemy/english/male/who_are_you.mp3" },
	{ "You're not supposed to be here", "sound/enemy/english/male/youre_not_supposed.mp3" },
	{ "Hey did you see that guy, He wet his pants ha haha ha ha..", "sound/npc/air1/guard01/01wetpants.mp3" },
	{ "Hey honey..", "sound/npc/air1/guard05/01honey.mp3" },
	{ "Huh I think i can help you", "sound/npc/gladstone/arm1/03canhelp.mp3" },
	{ "If you look at me 1 more time, I swear I'll BLOW YOUR HEAD OFF", "sound/npc/air1/terrorists/blowheadoff.mp3" },
	{ "How bout we see if you can dodge a BULLET", "sound/npc/air1/terrorists/dodgebullet.mp3" },
	{ "That was a close one", "sound/npc/fritsch/closeone.mp3" },
	{ "What are you lookin at", "sound/npc/air1/terrorists/lookingat.mp3" },
	{ "You scared?, You should be", "sound/npc/air1/terrorists/scared02.mp3" },
	{ "Leave me alone", "sound/npc/air1/woman01/01leaveme.mp3" },
	{ "Please just leave me be..", "sound/npc/air1/woman01/02please.mp3" },
	{ "What? Please explain.. over", "sound/npc/air4/hansen/04what.mp3" },
	{ "Attention", "sound/npc/cem1/comm/01attention.mp3" },
	{ "Someone call 911", "sound/npc/civ/english/male/call_911.mp3" },
	{ "Call the police", "sound/npc/civ/english/male/callpolice.mp3" },
	{ "Comin thru..", "sound/npc/civ/english/male/coming_thru.mp3" },
	{ "NICE SHOT", "sound/npc/col8/washington/niceshot.mp3" },
	{ "Excuse me", "sound/npc/civ/english/male/excuse_me.mp3" },
	{ "Don't hurt me", "sound/npc/civ/english/male/dont_hurt.mp3" },
	{ "I don't understand", "sound/npc/civ/english/male/dont_understand.mp3" },
	{ "I don't have any money", "sound/npc/civ/english/male/no_money.mp3" },
	{ "Those men have guns", "sound/npc/civ/english/male/thosemen.mp3" },
	{ "Take my wallet, Just don't hurt me", "sound/npc/civ/english/male/wallet.mp3" },
	{ "I'm unarmed", "sound/npc/civ/english/male/unarmed.mp3" },
	{ "Watchout", "sound/npc/civ/english/male/watchout.mp3" },
	{ "Pairup", "sound/npc/col2/butch/pairup.mp3" },
	{ "Fanout", "sound/npc/col2/butch/fanout.mp3" },
	{ "We got company, Take cover", "sound/npc/col3/peterson/12company.mp3" },
	{ "Enemy has been neutralized, base camp is secure. over", "sound/npc/col3/radio/02enemy.mp3" },
	{ "BANG! Your dead", "sound/npc/col8/blakely/bang.mp3" },
	{ "DAMN! that was close", "sound/npc/col8/blakely/close.mp3" },
	{ "GET DOWN", "sound/npc/col8/blakely/getdown.mp3" },
	{ "Come get some", "sound/npc/col8/blakely/getsome.mp3" },
	{ "Incoming", "sound/npc/col8/blakely/incoming.mp3" },
	{ "Go cry to mama", "sound/npc/col8/blakely/mama.mp3" },
	{ "We showed them", "sound/npc/col8/blakely/showed.mp3" },
	{ "Take that", "sound/npc/col8/blakely/takethat.mp3" },
	{ "That did it", "sound/npc/col8/blakely/thatdidit.mp3" },
	{ "You want some of this", "sound/npc/col8/blakely/wantsome.mp3" },
	{ "Yeah", "sound/npc/col8/blakely/yeah.mp3" },
	{ "WOOOO", "sound/npc/col8/blakely/woo.mp3" },
	{ "Don't worry girls it'll all be over soon", "sound/npc/col8/peterson/01girls.mp3" },
	{ "Cover my left", "sound/npc/col8/peterson/coverleft.mp3" },
	{ "Cover my right", "sound/npc/col8/peterson/coverright.mp3" },
	{ "Keep your eyes open boys", "sound/npc/col8/peterson/eyesopen.mp3" },
	{ "Get moving", "sound/npc/col8/peterson/getmoving02.mp3" },
	{ "Shudd-up man", "sound/npc/col8/washington/02shuddup.mp3" },
	{ "Eat lead sukka", "sound/npc/col8/washington/eatlead.mp3" },
	{ "Kickin ass, and takin names", "sound/npc/col8/washington/takingnames.mp3" },
	{ "How'd you like that", "sound/npc/col8/washington/youlike.mp3" },
	{ "WoOoOo", "sound/npc/col8/washington/woo.mp3" },
	{ "O.K, lets do it", "sound/npc/col9/pilot/03letsdoit.mp3" },
	{ "We can't take much more of this", "sound/npc/col9/pilot/canttake.mp3" },

	{"Hey, We've Got Company", "sound/npc/air3/terrorist01/02hey.mp3"   },
	{ "Jesus", "sound/npc/air3/janitor/01jesus.mp3" },
	{ "You Should Be More Polite I'm the One with a Gun", "sound/npc/air1/guard05/02gun.mp3" },
	{ "Let Him Go Son, Just let Him Go", "sound/npc/gladstone/cem1/03lethimgo.mp3" },
	{ "I say We Kill Him Right now", "sound/npc/hk7/terrorist04/01killhim.mp3" },
	{ "With Pleasure..", "sound/npc/hk7/torturer/04pleasure.mp3" },
	{ "Of Course", "sound/npc/hk7/torturer/03ofcourse.mp3" },
	{ "Don't Worry About it", "sound/npc/tut1/npc02/01dontworry.mp3" },
	{ "What was that, what did you say?", "sound/npc/tut1/npc01/03what.mp3" },
	{ "I thought i saw something", "sound/npc/team/orders/thought_i_saw.mp3" },
	{ "What was that noise", "sound/npc/team/orders/what_was_noise.mp3" },
	{ "Alright, Nothing to see here", "sound/npc/arm2/agent01/01nothingtosee.mp3" },
	{ "Sir, I think they went this way", "sound/npc/col3/faber/01thisway.mp3" },
	{ "HaHaHa", "sound/npc/col8/peterson/02laugh.mp3" },
	{ "Mullins, go in there and clean out that building", "sound/npc/col8/peterson/clean01.mp3" },
	{ "Mullins, Clean out this building", "sound/npc/col8/peterson/clean02.mp3" },
	{ "Fire In the Hole!", "sound/npc/col8/peterson/firehole.mp3" },
	{ "Go Check that out", "sound/npc/col8/peterson/gocheck.mp3" },
	{ "Good work Marines!", "sound/npc/col8/peterson/goodwork.mp3" },
	{ "Nice Shot", "sound/npc/col8/peterson/niceshot.mp3" },
	{ "Man, this place is really getting on my nerves ", "sound/npc/col8/washington/01place.mp3" },
	{ "Seen anything like this?", "sound/npc/col10/scientist04/01seenanything.mp3" },
	{ "Roger That, over.", "sound/npc/hos3/terr/02terr1.mp3" },
	{ "Hey Mister, you dropped your Briefcase", "sound/npc/pra1/bum/01heymister.mp3" }
};

static int numDefaultChatSounds = sizeof(defaultChatSounds) / sizeof(defaultChatSounds[0]);
static int numChatSounds = 0;

void ParseChatSounds(){
	G_LogPrintf("Loading default chat sounds\n");
	for (int i = 0; i < numDefaultChatSounds; i++)
	{
		chatSounds[i].text = defaultChatSounds[i][0];
		chatSounds[i].sound = G_SoundIndex(va("%s", defaultChatSounds[i][1].c_str()));
		numChatSounds++;
	}
}

bool isdigit(char c){
	if (c >= 48 && c <= 57)
		return true;
	else
		return false;
}

void globalSound(int soundIndex)
{
	gentity_t *tent;

	tent = G_TempEntity(vec3_origin, EV_GLOBAL_SOUND);
	tent->s.eventParm = soundIndex;
	tent->r.svFlags = SVF_BROADCAST;
}

string parseChatTokens(gentity_t *ent, const char *message, int *outSound){
	string result;
	string sound;
	bool parseSound = false;
	*outSound = -1;
	for (unsigned int i = 0; i < strlen(message); i++){
		if (message[i] == '@'){
			sound = "";
			parseSound = true;
		}
		else if (parseSound && !isdigit(message[i])){
			int soundIndex = boost::lexical_cast<int>(sound)-1;
			if (soundIndex >= 0 && soundIndex < numChatSounds){
				result.append(chatSounds[soundIndex].text);
				if (!isVoiceFlooded(ent))
					*outSound = chatSounds[soundIndex].sound;
			}
			else
				result.append("@" + sound); // append sound string as normal
			result.insert(result.end(), message[i]);
			parseSound = false;
		}
		else if (parseSound && isdigit(message[i])){
			sound += message[i];
		}
		else
			result.insert(result.end(), message[i]);
	}

	if (parseSound){
		int soundIndex = boost::lexical_cast<int>(sound)-1;
		if (soundIndex >= 0 && soundIndex < numChatSounds){
			result.append(chatSounds[soundIndex].text);
			if (!isVoiceFlooded(ent))
				*outSound = chatSounds[soundIndex].sound;
		}
		else
			result.append("@" + sound); // append sound string as normal
		parseSound = false;
	}
	return result;
}

bool isVoiceFlooded(gentity_t *ent){
	if (g_voiceFloodCount.integer)
	{
		// If this client has been penalized for voice chatting to much then dont allow the voice chat
		if (ent->client->voiceFloodPenalty)
		{
			if (ent->client->voiceFloodPenalty > level.time)
			{
				return true;
			}

			// No longer penalized
			ent->client->voiceFloodPenalty = 0;
		}

		// See if this client flooded with voice chats
		ent->client->voiceFloodCount++;
		if (ent->client->voiceFloodCount >= g_voiceFloodCount.integer)
		{
			ent->client->voiceFloodCount = 0;
			ent->client->voiceFloodTimer = 0;
			ent->client->voiceFloodPenalty = level.time + g_voiceFloodPenalty.integer * 1000;

			infoMsgToClients(ent->client->ps.clientNum, va("Voice chat flooded, you will be able use voice chats again in (%d) seconds", g_voiceFloodPenalty.integer));

			return true;
		}
		return false;
	}else
		return false;
}