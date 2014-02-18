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
	bool cg_thirdPerson;
	bool cg_antiLag;
	bool cg_autoReload;
	bool cg_smoothClients;
	string cg_rpmClient;
	string team_identity = "mullinsjungle";
	string outfitting = "AAAA";

	userinfo(int id);
	~userinfo();

	void setIdentity(int id, string identity);
};
#endif
