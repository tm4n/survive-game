#include "b_npcs.h"
#include "tinyxml.h"
#include "helper.h"

#ifdef ACKNEX
#include <windows.h>
#include "adll.h"
#endif

b_npcs *b_np;


b_npcs::b_npcs(const char *filename)
{
	int id;
	s_npcs *np;
	const char *chr;

	std::string full_filename;
	get_workdir(&full_filename);
	full_filename.append(filename);

	TiXmlDocument doc(full_filename);
	if (!doc.LoadFile()) {err("Couldn't open file for npcs gui!", -1); return;}

	TiXmlHandle hDoc(&doc);
	TiXmlElement *pElem;
	TiXmlHandle hRoot(0);

	// document base
	pElem=hDoc.FirstChildElement().Element();
	// should always have a valid root but handle gracefully if it does
	if (!pElem) {err("Root Element not found, file empty?", -1); return;}
	if (strcmp(pElem->Value(), "backend_npcs") != 0) {err("backend_npcs definition not found", -1); return;}

	// save this for later
	hRoot=TiXmlHandle(pElem);

	// get all entries
	pElem=hRoot.FirstChild().Element();
	for( ; pElem; pElem=pElem->NextSiblingElement())
	{
		///////////////////////////////////////////
		// class definition
		if (strcmp(pElem->Value(), "npc") == 0)
		{

			if (pElem->QueryIntAttribute ("id", &id) != TIXML_SUCCESS) {err("id of npc missing or invalid", id); return;}
			if (id < 0 || id >= B_NPCS_ENTRIES) {err("id of npc is invalid", id); return;}

			np = &(npcs[id]);

			if ((chr = pElem->Attribute("file")) == NULL) {err("file of npc is missing", id); return;}
			np->bfile.assign(chr);

			if ((chr = pElem->Attribute("name")) == NULL) {err("name of npc is missing", id); return;}
			np->bname.assign(chr);

			if ((chr = pElem->Attribute("info")) == NULL) {err("info of npc is missing", id); return;}
			np->binfo.assign(chr);

			if (pElem->QueryIntAttribute ("faction", &(np->bfaction)) != TIXML_SUCCESS) {err("faction of npc missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute ("max_health", &(np->bmax_health)) != TIXML_SUCCESS) {err("max_health of npc missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute ("move_speed", &(np->bmove_speed)) != TIXML_SUCCESS) {err("move_speed of npc missing or invalid", id); return;}

			if (pElem->QueryIntAttribute ("level", &(np->blevel)) != TIXML_SUCCESS) {err("level of npc missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute ("att_range", &(np->batt_range)) != TIXML_SUCCESS) {err("att_range of npc missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute ("att_speed", &(np->batt_speed)) != TIXML_SUCCESS) {err("att_speed of npc missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute ("att_dmg_min", &(np->batt_dmg_min)) != TIXML_SUCCESS) {err("att_dmg_min of npc missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute ("att_dmg_max", &(np->batt_dmg_max)) != TIXML_SUCCESS) {err("att_dmg_max of npc missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute ("att_cooldown", &(np->batt_cooldown)) != TIXML_SUCCESS) {err("att_cooldown of npc missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute ("bounty_xp", &(np->bbounty_xp)) != TIXML_SUCCESS) np->bbounty_xp = 0;

			if (pElem->QueryFloatAttribute ("bounty_gold_probability", &(np->bbounty_gold_probability)) != TIXML_SUCCESS) np->bbounty_gold_probability = 0;

			if (pElem->QueryIntAttribute ("bounty_gold_min", &(np->bbounty_gold_min)) != TIXML_SUCCESS) np->bbounty_gold_min = 0;
			if (pElem->QueryIntAttribute ("bounty_gold_max", &(np->bbounty_gold_max)) != TIXML_SUCCESS) np->bbounty_gold_max = 0;

			if (pElem->QueryIntAttribute ("dropset_id", &(np->bdropset_id)) != TIXML_SUCCESS) np->bdropset_id = 0;

			if (pElem->QueryIntAttribute ("dialogue_casual_id", &(np->bdialogue_casual_id)) != TIXML_SUCCESS) np->bdialogue_casual_id = 0;

			/*np->binteract_type = B_NPCS_INTERACT_TYPE_NONE;
			if ((chr = pElem->Attribute ("interact_type")) != NULL)
			{
                if (strcmp(chr, "quest") == 0) np->binteract_type = B_NPCS_INTERACT_TYPE_QUEST;
                if (strcmp(chr, "merchant") == 0) np->binteract_type = B_NPCS_INTERACT_TYPE_MERCHANT;
                if (strcmp(chr, "quest_merchant") == 0) np->binteract_type = B_NPCS_INTERACT_TYPE_QUEST_MERCHANT;
			}*/

			// TODO: finish!

		}
	}
}

s_npcs *b_npcs::at(uint id)
{
	if (id > B_NPCS_ENTRIES) {err(" b_npcs::at was called with invalid id", -1); return NULL;}
	return &npcs[id];
}

void b_npcs::err(const char *msg, int id)
{
	std::ostringstream pr;
	pr << "ERROR in b_npcs in id=" << id << " :" << msg;

#ifdef ACKNEX
	error((char*)pr.str().c_str());
#else
	std::cout << pr.str() << std::endl;
#endif
}
