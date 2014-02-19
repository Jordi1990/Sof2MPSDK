#ifndef GAMETYPE_INF_H
#define GAMETYPE_INF_H

#include "g_local.h"
class Gametype_inf : public Gametype {
public:
	void onItemStuck(gitem_t *item);
	bool onItemTouch(gitem_t *item, gclient_s *other);
	Gametype_inf();
	void update();
	void onTeamEliminated(team_t team);
	void onGametimeExpired();
};
#endif
