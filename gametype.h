#ifndef GAMETYPE_H
#define GAMETYPE_H

#include "g_local.h"

class Gametype {
public:
	virtual void onItemStuck(gitem_t *item) = 0;
	virtual bool onItemTouch(gitem_t *item) = 0;
};
#endif
