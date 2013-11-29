#include "b_quests.h"
#include "tinyxml.h"
#include "helper.h"

#include <iostream>
#include <sstream>

#ifdef ACKNEX
#include <windows.h>
#include "adll.h"
#endif

b_quests *b_qe;


b_quests::b_quests(const char *filename)
{
	int id;
	const char *chr;
    std::string str;
	s_quest *qe;

	// clear all entries
	for (int i = 0; i < B_QUESTS_ENTRIES; i++)
	{
		quests[i].bnpc_id = 0;
		quests[i].goals[0].type = 0; quests[i].goals[1].type = 0; quests[i].goals[2].type = 0;
	}


	// load file
    std::string full_filename;
	get_workdir(&full_filename);
	full_filename.append(filename);

	TiXmlDocument doc(full_filename);
	if (!doc.LoadFile()) {err("Couldn't open file for quests backend!", -1);}

	TiXmlHandle hDoc(&doc);
	TiXmlElement *pElem, *pSubElem;
	TiXmlHandle hRoot(0);

	// document base
	pElem=hDoc.FirstChildElement().Element();
	// should always have a valid root but handle gracefully if it does
	if (!pElem) {err("item quests: Root Element not found, file empty?", -1); return;}
	if (strcmp(pElem->Value(), "backend_quests") != 0) {err("backend_quests definition not found", -1); return;}

	// save this for later
	hRoot=TiXmlHandle(pElem);

	// get all entries
	pElem=hRoot.FirstChild().Element();
	for( ; pElem; pElem=pElem->NextSiblingElement())
	{
		///////////////////////////////////////////
		// class definition
		if (strcmp(pElem->Value(), "quest") == 0)
		{

			if (pElem->QueryIntAttribute ("id", &id) != TIXML_SUCCESS) {err("id of quest missing or invalid", id); return;}
			if (id < 0 || id >= B_QUESTS_ENTRIES) {err("id of quest is invalid", id); return;}

			qe = &(quests[id]);

			if ((chr = pElem->Attribute("name")) == NULL) {err("name of quest is missing", id); return;}
			qe->bname.assign(chr);

			if ((chr = pElem->Attribute("desc")) == NULL) {err("desc of quest is missing", id); return;}
			qe->bdesc.assign(chr);

			if (pElem->QueryIntAttribute("npc_id", &(qe->bnpc_id)) != TIXML_SUCCESS) {err("npc_id of quest missing or invalid", id); return;}

			if (pElem->QueryIntAttribute("dialogue_id", &(qe->bdialogue_id)) != TIXML_SUCCESS) {err("dialogue_open_id of quest missing or invalid", id); return;}

			if (pElem->QueryIntAttribute("allow_shop", &(qe->ballow_shop)) != TIXML_SUCCESS) qe->ballow_shop = 0;

            // query subelements
			qe->goals[0].type = 0; qe->goals[1].type = 0; qe->goals[2].type = 0;
            s_req req;
			int g_id;
			s_goal *go;
			s_reward rew;

			for(pSubElem=pElem->FirstChildElement(); pSubElem; pSubElem=pSubElem->NextSiblingElement())
			{

				////////////////////////////////////////
				// REQUIREMENT

				if (strcmp(pSubElem->Value(), "requirement") == 0)
				{
					if ((chr = pSubElem->Attribute("type")) == NULL) {err("requirement type of a quest missing or invalid", id); return;}
					req.type = -1;
					if (strcmp(chr, "level") == 0) req.type = QE_REQUIREMENT_LEVEL;
					if (strcmp(chr, "quest") == 0) req.type = QE_REQUIREMENT_QUEST;
					if (strcmp(chr, "class") == 0) req.type = QE_REQUIREMENT_CLASS;
					if (strcmp(chr, "item") == 0) req.type = QE_REQUIREMENT_ITEM;
					if (req.type == -1) {err("requirement type of a quest is unknown", id); return;}

                    if (pSubElem->QueryIntAttribute("value", &(req.val)) != TIXML_SUCCESS) {err("requirement value of a quest missing or invalid", id); return;}

					qe->requirements.push_back(req);
				}


				/////////////////////////////////////////
				// GOAL (QUEST OBJECTIVE)

                if (strcmp(pSubElem->Value(), "goal") == 0)
				{
					if (pSubElem->QueryIntAttribute("id", &(g_id)) != TIXML_SUCCESS) {err("goal id of a quest missing or invalid", id); return;}
					if (g_id < 0 || g_id >= B_QUESTS_GOAL_ENTRIES) {err("goal id of a quest was invalid, probably too large", id); return;}

					go = &(qe->goals[g_id]);


					if ((chr = pSubElem->Attribute("type")) == NULL) {err("goal type of a quest missing or invalid", id); return;}

					go->type = -1;
					if (strcmp(chr, "item") == 0) go->type = QE_GOAL_ITEM;
					if (strcmp(chr, "kill") == 0) go->type = QE_GOAL_KILL;
					if (strcmp(chr, "talk") == 0) go->type = QE_GOAL_TALK;
					if (strcmp(chr, "bring") == 0) go->type = QE_GOAL_BRING;
					if (go->type == -1) {err("goal type of a quest is unknown", id); return;}

					if (pSubElem->QueryIntAttribute("target_id", &(go->target_id)) != TIXML_SUCCESS) {err("goal target_id of a quest missing or invalid", id); return;}

                    if (pSubElem->QueryIntAttribute("amount", &(go->amount)) != TIXML_SUCCESS) {err("goal value of a quest missing or invalid", id); return;}

                    if (pSubElem->QueryIntAttribute("dialogue_id", &(go->dialogue_id)) != TIXML_SUCCESS && go->type == QE_GOAL_BRING) {err("dialogue_id value of a bring-quest missing or invalid", id); return;}
				}

				//////////////////////////////////////////
				// REWARD

                if (strcmp(pSubElem->Value(), "reward") == 0)
				{
					if ((chr = pSubElem->Attribute("type")) == NULL) {err("reward type of a quest missing or invalid", id); return;}
					rew.type = -1;
					if (strcmp(chr, "item") == 0)
					{
						rew.type = QE_REWARD_ITEM;
						if (pSubElem->QueryIntAttribute("id", &(rew.id)) != TIXML_SUCCESS) {err("id for item reward missing or invalid", id); return;}
					}
					if (strcmp(chr, "gold") == 0) rew.type = QE_REWARD_GOLD;
					if (strcmp(chr, "xp") == 0) rew.type = QE_REWARD_XP;
					//if (strcmp(chr, "skill") == 0) rew.type = QE_REWARD_SKILL;
					if (rew.type == -1) {err("reward type of a quest is unknown", id); return;}

                    if (pSubElem->QueryIntAttribute("amount", &(rew.amount)) != TIXML_SUCCESS) {err("requirement value of a quest missing or invalid", id); return;}

					qe->rewards.push_back(rew);
				}

				//////////////////////////////////////////
				// QUEST SUCCESS TEXT

				if (strcmp(pSubElem->Value(), "text_success") == 0)
				{
					qe->btext_success.assign(pSubElem->GetText());
				}

                //////////////////////////////////////////
				// QUEST OPEN TEXT

				if (strcmp(pSubElem->Value(), "text_open") == 0)
				{
					qe->btext_open.assign(pSubElem->GetText());
				}
			}

		}
	}
}




s_quest *b_quests::at(uint id)
{
	if (id > B_QUESTS_ENTRIES) {err("b_quests::att was called with invalid id", -1); return NULL;}
	return &quests[id];
}


void b_quests::err(const char *msg, int id)
{
	std::ostringstream pr;
	pr << "ERROR in b_quests in id=" << id << " :" << msg;

#ifdef ACKNEX
	error((char*)pr.str().c_str());
#else
	std::cout << pr.str() << std::endl;
#endif
}
