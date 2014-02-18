#include "gametype_inf.h"
#define ITEM_BRIEFCASE 100
#define TRIGGER_EXTRACTION 200

Gametype_inf::Gametype_inf() {
	Com_Printf("Initializing gametype...\n");
	int start = level.time;
	// Register the items
	// Boe!Man 11/29/12: Register items per gametype.
		gitem_t *item = BG_FindItem("briefcase");
		if (item){
			item->quantity = ITEM_BRIEFCASE;
		}
	// Boe!Man 11/29/12: Register triggers per gametype.
	gentity_t *find = NULL;
	while (NULL != (find = G_Find(find, FOFS(classname), "gametype_trigger")))
	{
		if (strcmp(find->targetname, (const char*) "briefcase_destination"))
		{
			continue;
		}

		// Assign the id to it.
		find->health = TRIGGER_EXTRACTION;
		//find->touch = gametype_trigger_touch;
		trap_LinkEntity(find);
	}
	Com_Printf("Gametype INF initialized in %dms\n", level.time - start);
}

void Gametype_inf::onItemStuck(gitem_t *item){
	G_ResetGametypeItem(item);
	trap_SendServerCommand(-1, "print \"Gametype item reset.\n\"");
}

bool Gametype_inf::onItemTouch(gitem_t *item){
	return true;
}