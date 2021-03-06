#include "g_local.h"

void Tokenize(const string& str,
	vector<string>& tokens,
	const string& delimiters = " ");

userinfo::userinfo(int id){
	// parse userinfo
	try{
		char userInfoBuf[MAX_INFO_STRING];
		trap_GetUserinfo(id, userInfoBuf, MAX_INFO_STRING);
		string buf = userInfoBuf;
		if (buf.length() < 1)
			throw "Userinfo fetching failed";
		vector<string> output;
		Tokenize(buf, output, "\\");
		for (unsigned int i = 0; i < output.size() - 1; i += 2){
			this->parseValue(output[i], output[i + 1]);
		}
	}
	catch (boost::bad_lexical_cast const &e){
		Com_Printf("Exception in userinfo parsing(boost::bad_lexical_cast): %s\n", e.what());
	}
	catch (std::exception const &e){
		Com_Printf("Exception in userinfo parsing(std::exception): %s\n", e.what());
	}
}

void userinfo::writeUserInfo(int id){
	char userInfoBuf[1024];
	sprintf_s(userInfoBuf, 1024, "\\ip\\%s\rate\\%i\\cg_predictItems\\%i\\cl_anonymous\\%i\\identity\\%s\\snaps\\%i\\name\\%s\\cg_thirdPerson\\%i\\cg_antiLag\\%i\\cg_autoReload\\%i\\cg_smoothClients\\%i\\cg_rpmClient\\%.2f\\team_identity\\%s\\outfitting\\%s", ip.c_str(), rate, cg_predictItems, cl_anonymous, identity.c_str(), snaps, name.c_str(), cg_thirdPerson, cg_antiLag, cg_autoReload, cg_smoothClients, cg_rpmClient, team_identity.c_str(), outfitting.c_str());
	trap_SetUserinfo(id, userInfoBuf);
}

void userinfo::setIdentity(int id, string identity){
	identity = identity;
	team_identity = identity;
	writeUserInfo(id);
	//format a new userinfo string then set it, replace identity value and team_identity value with identity value
	//trap_SetUserinfo(id, "");
}
/*\ip\192.168.1.100:20101\rate\15000\cg_predictItems\1\cl_anonymous\0\identity\marinesoldier1\snaps\20
\name\^teamtask\0\cg_thirdPerson\1\cg_disableVoiceChat\0\cg_disableSpreeSounds\0\cg_antiLag\1\cg_autoReload\1\cg_smoothClients\0
\cg_rpmClient\2.0\team_identity\shopguard1\outfitting\AAA@A*/

void userinfo::parseValue(const string &tag, const string &value){
	if (tag == "ip"){
		this->ip = value;
		if (value.find(':') != string::npos){
			for (int i = 0; i < 6; i++)
				ip.pop_back();
		}	}
	else if (tag == "challenge")
		this->challenge = boost::lexical_cast<int>(value);
	else if (tag == "qport")
		this->qport = boost::lexical_cast<int>(value);
	else if (tag == "rate"){
		try{
			this->rate = boost::lexical_cast<int>(value);
		}
		catch (boost::bad_lexical_cast const&){
			this->rate = 25000;
		}
	}
	else if (tag == "name"){
		try{
			this->name = value;
		}
		catch (boost::bad_lexical_cast const&){
			this->name = "Unnamed Player";
		}
	}
	else if (tag == "snaps"){
		try{
			this->snaps = boost::lexical_cast<int>(value);
		}
		catch (boost::bad_lexical_cast const&){
			this->snaps = 40;
		}
	}
	else if (tag == "identity")
		this->identity = value;
	else if (tag == "team_identity")
		this->team_identity = value;
	else if (tag == "cg_predictItems")
		this->cg_predictItems = boost::lexical_cast<bool>(value);
	else if (tag == "cg_thirdPerson")
		this->cg_thirdPerson = boost::lexical_cast<bool>(value);
	else if (tag == "cg_antiLag")
		this->cg_antiLag = boost::lexical_cast<bool>(value);
	else if (tag == "cg_autoReload")
		this->cg_autoReload = boost::lexical_cast<bool>(value);
	else if (tag == "cg_smoothClients")
		this->cg_smoothClients = boost::lexical_cast<bool>(value);
	else if (tag == "outfitting")
		this->outfitting = value;
	else if (tag == "password")
		this->password = value;
	else if (tag == "cg_rpmClient")
		this->cg_rpmClient = boost::lexical_cast<float>(value);
}

userinfo::~userinfo(){

}

// Not my implementation
// http://oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
// Seems like a very nice implementation(we could also do this with boost).
void Tokenize(const string& str,
	vector<string>& tokens,
	const string& delimiters)
{
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}