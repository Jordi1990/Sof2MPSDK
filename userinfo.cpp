#include "g_local.h"

void Tokenize(const string& str,
	vector<string>& tokens,
	const string& delimiters = " ");

userinfo::userinfo(int id){
	// parse userinfo
	char userInfoBuf[MAX_INFO_STRING];
	trap_GetUserinfo(id, userInfoBuf, MAX_INFO_STRING);
	string buf = userInfoBuf;
	vector<string> output;
	Tokenize(buf, output, "\\");
	for (unsigned int i = 0; i < output.size() - 1; i += 2){
		this->parseValue(output[i], output[i + 1]);
	}
}

void userinfo::setIdentity(int id, string identity){
	//format a new userinfo string then set it, replace identity value and team_identity value with identity value
	//trap_SetUserinfo(id, "");
}

void userinfo::parseValue(const string &tag, const string &value){
	try{
		if (tag == "ip")
			this->ip = value;
		else if (tag == "challenge")
			this->challenge = boost::lexical_cast<int>(value);
		else if (tag == "qport")
			this->qport = boost::lexical_cast<int>(value);
		else if (tag == "rate")
			this->rate = boost::lexical_cast<int>(value);
		else if (tag == "name")
			this->name = value;
		else if (tag == "snaps")
			this->snaps = boost::lexical_cast<int>(value);
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
		else if (tag == "cg_rpmClient")
			this->cg_rpmClient = value;
		else if (tag == "outfitting")
			this->outfitting = value;
	}
	catch (boost::bad_lexical_cast const&){
		throw "userinfo::parseValue invalid value";
	}
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