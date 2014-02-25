#ifndef __Q_USERINFO_H
#define __Q_USERINFO_H
#include <string>
#include <vector>
#include <boost\lexical_cast.hpp>

using namespace std;

class userinfo {
private:
	void parseValue(const string &tag, const string &value);
public:
	string ip;
	int challenge;
	unsigned short qport;
	unsigned short protocol;
	string password;
	unsigned int rate = 25000;
	unsigned char snaps = 40; // char because its never higher then 255
	string name = "Unnamed Player";
	string identity = "colrebel1";
	bool cl_anonymous;
	bool cg_predictItems;
	bool teamtask;
	short cg_thirdPerson = 2; // short because it can be another value besides true/false
	bool cg_antiLag;
	bool cg_autoReload;
	bool cg_smoothClients;
	float cg_rpmClient;
	string team_identity = "mullinsjungle";
	string outfitting = "AAAA";

	userinfo(int id);
	~userinfo();

	void setIdentity(int id, string identity);
	void writeUserInfo(int id);
};
#endif
