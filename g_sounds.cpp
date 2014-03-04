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

	{ "Ya Ya Ya just just Wait", "sound/npc/air1/guard04/02wait.mp3" },
	{ "go away!", "sound/npc/civ/english/male/go_away.mp3" },
	{ "dude whats going on" "sound/npc/civ/english/male/goingon.mp3" },
	{ "HELP!", "sound/npc/civ/english/male/help.mp3" },
	{ "no, NOOOO", "sound/npc/civ/english/male/no.mp3" },
	{ "RUN", "sound/npc/civ/english/male/run.mp3" },
	{ "lol", "sound/npc/col10/scientist02/02laugh.mp3" },
	{ "hello", "sound/npc/civ/english/male/hello.mp3" },
	{ "Ha great shot", "sound/npc/col9/pilot/greatshot.mp3" },
	{ "behind you", "sound/npc/fritsch/behindyou.mp3" },
	{ "Ahh SHUTUP", "sound/ahshutup.mp3" },
	{ "I don't remember askin you a goddamn thing", "sound/askinyou.mp3" },
	{ "I do the ass kicking around here", "sound/asskicking.mp3" },
	{ "Awwww Crap", "sound/awwcrap.mp3" },
	{ "if I may be so blunt, who gives a shit", "sound/beblunt.mp3" },
	{ "Bullshit", "sound/Bullshit.mp3" },
	{ "hehe, you can't kill me suckka", "sound/cantkillme.mp3" },
	{ "you COCK SUCKER", "sound/cocksucker.mp3" },
	{ "DIE, you son of a bitch", "sound/diesob.mp3" },
	{ "look at the screen you dumbshit, I'm tryin to get your attention", "sound/dumbshit.mp3" },
	{ "the following people are gay", "sound/gaypeople.mp3" },
	{ "aww, jesus fuck, now he's gonna cry", "sound/gonnacry.mp3" },
	{ "he's campin again", "sound/hes_campin.wav" },
	{ "HOLY SHIT", "sound/holyshit.mp3" },
	{ "huh", "sound/huh.mp3" },
	{ "I'll be back", "sound/beback.mp3" },
	{ "I'm gonna take these bastards out", "sound/bastards_out.wav" },
	{ "You suck, ya jackass", "sound/jackass.mp3" },
	{ "why don't you, kiss my ass", "sound/kma.mp3" },
	{ "ha ha ha ha haa.. SHUTUP", "sound/lolshutup.mp3" },
	{ "hm hm hmhm hm.. yeah", "sound/lolyeah.mp3" },
	{ "looser", "sound/loser.wav" },
	{ "nailed those bastards", "sound/nailed_bastards.wav" },
	{ "oh my god", "sound/ohmygod.mp3" },
	{ "ok bitch", "sound/ok_bitch.wav" },
	{ "oh oh that pisses me off, that pisses me right off", "sound/pissesmeoff.mp3" },
	{ "relax tootse, don't get your panties in a bunch", "sound/relaxt.mp3" },
	{ "just when I thought you said the stupidest thing ever, you keep talkin", "sound/stupidest_thing.mp3" },
	{ "son of a bitch", "sound/sonofabitch.mp3" },
	{ "I'm on you like stink on shit", "sound/stinkonshit.mp3" },
	{ "stop whining", "sound/stopwhining.mp3" },
	{ "I knew it I'm surrounded by assholes", "sound/surrounded.mp3" },
	{ "you all SUCK", "sound/uallsuck.mp3" },
	{ "you are a BITCH", "sound/urabitch.mp3" },
	{ "I'm gonna have ta open up a can a whoopass on ya now", "sound/whoopass.mp3" },
	{ "yeah baby..haha ha ha ha", "sound/yeah_baby.wav" },
	{ "you guys suck", "sound/youguyssuck.mp3" },
	{ "damn.. you suck", "sound/yousuck.mp3" },
	{ "lol", "sound/lolbeavis.mp3" },
	{ "this is some bullshit", "sound/thissome.mp3" },
	{ "can you believe this ass hole", "sound/believethisasshole.mp3" },
	{ "alright..i'm callin the shots", "sound/callingshots.mp3" },
	{ "chalk me up another one boys", "sound/chalkanotherboys.mp3" },
	{ "com'ere you double crossing piece of shit", "sound/com'heredoublecross.mp3" },
	{ "I'm comin for ya", "sound/comin'4ya.mp3" },
	{ "you're a dead bitch", "sound/deadbitch.mp3" },
	{ "for feck's sake", "sound/feck'sake.mp3" },
	{ "fight like men with HUGE COHONAS", "sound/fightlikemenhugecohon.mp3" },
	{ "you're goin' down you backstabbing prick", "sound/goin'downprick.mp3" },
	{ "good shooting my friend", "sound/goodshootingfriend.mp3" },
	{ "Hey..he's gotta blade", "sound/gotablade.mp3" },
	{ "Got me another one", "sound/gotanotherone.mp3" },
	{ "we got ourselves a bleeder", "sound/gotbleeder.mp3" },
	{ "that's gotta hurt", "sound/gottahurt.mp3" },
	{ "greased his ass", "sound/greasedhisass.mp3" },
	{ "com'on hotshot", "sound/hotshot.mp3" },
	{ "I own you", "sound/iownyou.mp3" },
	{ "Keep away from me you cheap bastard", "sound/keepawaybastard.mp3" },
	{ "Let's Do It", "sound/lets do it.mp3" },
	{ "Kiss My Ass", "sound/kissass.mp3" },
	{ "zip or fly yer little bastard", "sound/littlebastard.mp3" },
	{ "you feel lucky punk", "sound/luckypunk.mp3" },
	{ "NOW I'M GONNA MAKE YOU PAY", "sound/makeyoupay.mp3" },
	{ "you're mama put's out on weekends", "sound/mamaweekends.mp3" },
	{ "we need reinforcements from the cafe", "sound/needreinforcments.mp3" },
	{ "NEXT", "sound/next.mp3" },
	{ "OH!..You Think So", "sound/youthinkso.mp3" },
	{ "woohiihaaa...this is to easy", "sound/ooohiihaatoeasy.mp3" },
	{ "He's Mine..that punk's mine", "sound/punkmine.mp3" },
	{ "I suggest you run away...Fast", "sound/runfast.mp3" },
	{ "your a little scardy kitten..baby boy", "sound/scaredy_kitten.mp3" },
	{ "SHIT!..he's gotta gun", "sound/shitgottagun.mp3" },
	{ "I'm gonna shut that big mouth of your's", "sound/shutthatbigmouth.mp3" },
	{ "Sniper on the roof", "sound/sniponroof.mp3" },
	{ "stand still and I'll make it quick", "sound/standstill.mp3" },
	{ "should'a stayed at home today", "sound/stayedathome.mp3" },
	{ "you still don't get it do ya", "sound/stilldon'tgetit.mp3" },
	{ "stop running yer fat slim-ball", "sound/stoprunningslimball.mp3" },
	{ "take out that cowardly sniper", "sound/takecowardlysnip.mp3" },
	{ "Thank-you", "sound/thankyou.mp3" },
	{ "what the hell are you doin'", "sound/thehellar'youdoing.mp3" },
	{ "this is 2 easy", "sound/toeasy01.mp3" },
	{ "you wanna go", "sound/wannago.mp3" },
	{ "you gotta small winky", "sound/winkie.mp3" },
	{ "That's Right...big yankie devil make bangbang all over your ass", "sound/yankiebangbang.mp3" },
	{ "You Bastard", "sound/youbastard.mp3" },
	{ "You Nutter", "sound/younutter.mp3" },
	{ "you're mine..MINE", "sound/yourmine.mp3" },
	{ "I'm gonna barbeque your ass", "sound/bbq ass.mp3" },
	{ "com'on let's go", "sound/comon.mp3" },

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
	bool soundDone = false;
	*outSound = -1;
	for (unsigned int i = 0; i < strlen(message); i++){
		if (message[i] == '@'){
			sound = "";
			parseSound = true;
			soundDone = false;
		}
		else if (parseSound && !isdigit(message[i]) && soundDone){
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
			soundDone = true;
		}
		else
			result.insert(result.end(), message[i]);
	}

	if (parseSound && soundDone){
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
	else if (parseSound && !soundDone)
		result.append("@");
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