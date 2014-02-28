//
// This file handles all the async fetching of client information
// such as country, admin, aliases

#include "g_local.h"
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <mutex>
#include "sqlite++\SQLiteC++.h"
std::mutex mtx;

void fetchAdmin(int number){
	try{
		g_entities[number].client->pers.adminLevel = NONE;
		SQLite::Database db("users.db");
		SQLite::Statement query(db, "SELECT level from admins WHERE IP=? AND name=? LIMIT 1");

		query.bind(0, g_entities[number].client->pers.ip);
		query.bind(1, g_entities[number].client->pers.cleanName);

		while (query.executeStep())
		{
			g_entities[number].client->pers.adminLevel = (adm_t)query.getColumn(0).getInt();
		}
	}
	catch (const std::exception &e){
		Com_Printf("fetchAdmin exception: %s\n", e.what());
	}
}

void fetchCountry(int number, bool firstTime){
	string data;
	string temp;
	start:
	boost::asio::ip::tcp::iostream stream;
	stream.expires_from_now(boost::posix_time::seconds(5));
	stream.connect("ip2country.sourceforge.net", "http");
	stream << "GET //ip2c.php?ip=" << g_entities[number].client->pers.ip << " HTTP / 1.0\r\n";
	stream << "Host: ip2country.sourceforge.net\r\n";
	stream << "Accept: */*\r\n";
	stream << "Connection: close\r\n\r\n";

	while (std::getline(stream, temp))
		data += temp;

	string country = "Unknown";
	string countryCode = "??";
	if (data.find("country_name") != string::npos){
		std::vector<std::string> strs;
		boost::split(strs, data, boost::is_any_of("\"")); // [5] = country code [7] = country
		country = strs[7];
		countryCode = strs[5];
	}
	else
		goto start;

	g_entities[number].client->pers.country = country;
	g_entities[number].client->pers.countryCode = countryCode;
	//Com_Printf("Country: %s\n", country.c_str());
	if (firstTime)
	{
		infoMsgToClients(-1, va("%s ^5[%s]^7 is connecting...", g_entities[number].client->pers.netname.c_str(), g_entities[number].client->pers.country.c_str()));
	}
}

void fetchInfo(int number, bool firstTime){
	mtx.lock();
	fetchCountry(number, firstTime);
	fetchAdmin(number);
	mtx.unlock();
}