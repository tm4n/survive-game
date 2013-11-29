#include "b_dialogues.h"
#include "tinyxml.h"
#include "helper.h"

#include <iostream>
#include <sstream>

#ifdef ACKNEX
#include <windows.h>
#include "adll.h"
#endif

b_dialogues *b_di;


b_dialogues::b_dialogues(const char *filename)
{
	int id;
	const char *chr;
	s_dialogue *di;

	str_err.assign("Text missing!");

    std::string full_filename;
	get_workdir(&full_filename);
	full_filename.append(filename);

	TiXmlDocument doc(full_filename);
	if (!doc.LoadFile()) {err("Couldn't open file for dialogues backend!", -1);}

	TiXmlHandle hDoc(&doc);
	TiXmlElement *pElem, *pSubElem, *pSubSubElem;
	TiXmlHandle hRoot(0);

	// document base
	pElem=hDoc.FirstChildElement().Element();
	// should always have a valid root but handle gracefully if it does
	if (!pElem) {err("dialogues backend: Root Element not found, file empty?", -1); return;}
	if (strcmp(pElem->Value(), "backend_dialogues") != 0) {err("backend_dialogues definition not found", -1); return;}

	// save this for later
	hRoot=TiXmlHandle(pElem);

	// get all entries
	pElem=hRoot.FirstChild().Element();
	for( ; pElem; pElem=pElem->NextSiblingElement())
	{
		///////////////////////////////////////////
		// class definition
		if (strcmp(pElem->Value(), "dialogue") == 0)
		{

			if (pElem->QueryIntAttribute ("id", &id) != TIXML_SUCCESS) {err("id of dialogue missing or invalid", id); return;}
			if (id < 0 || id >= B_DIALOGUES_ENTRIES) {err("id of dialogue is invalid", id); return;}

			di = &(dialogues[id]);


            // query subelements
			s_part p;
			s_part_answer p_a;

			for(pSubElem=pElem->FirstChildElement(); pSubElem; pSubElem=pSubElem->NextSiblingElement())
			{

				/////////////////////////////////////////
				// dialogue PART

                if (strcmp(pSubElem->Value(), "part") == 0)
				{
					p.text.clear();
					p.answers.clear();

					if (pSubElem->QueryIntAttribute("id", &(p.id)) != TIXML_SUCCESS) {err("part id of a dialogue missing or invalid", id); return;}

					// get text and answers
					for(pSubSubElem=pSubElem->FirstChildElement(); pSubSubElem; pSubSubElem=pSubSubElem->NextSiblingElement())
					{
						// dialogue text
						if (strcmp(pSubSubElem->Value(), "text") == 0)
						{
							p.text.assign(pSubSubElem->GetText());
						}

							
						// dialogue answer
						if (strcmp(pSubSubElem->Value(), "answer") == 0)
						{
							if ((chr = pSubSubElem->Attribute("action")) == NULL) {err("answer action of a dialogue part is missing or invalid", id); return;}

							p_a.action = -1;
							p_a.val = 0;
							if (strcmp(chr, "quest_accept") == 0)
							{
								p_a.action = DI_ANSWER_QUEST_ACCEPT;
								if (pSubSubElem->QueryIntAttribute("value", &(p_a.val)) != TIXML_SUCCESS) {err("answer value for (quest_accept) of a dialogue part missing or invalid", id); return;}
							}
							if (strcmp(chr, "next_part") == 0)
							{
								p_a.action = DI_ANSWER_NEXT_PART;
								if (pSubSubElem->QueryIntAttribute("value", &(p_a.val)) != TIXML_SUCCESS) {err("answer value for (next_part) of a dialogue part missing or invalid", id); return;}
							}
							if (strcmp(chr, "close") == 0) p_a.action = DI_ANSWER_CLOSE;
							if (strcmp(chr, "shop") == 0) p_a.action = DI_ANSWER_SHOP;
							if (strcmp(chr, "quest_bring") == 0) p_a.action = DI_ANSWER_QUEST_BRING;
							if (p_a.action == -1) {err("answer action of a dialogue part is unknown", id); return;}

							p_a.text.assign(pSubSubElem->GetText());

							p.answers.push_back(p_a);
						}

					}

					di->parts.push_back(p);

				}

			}

		}
	}
}


std::string b_dialogues::get_text(uint dialogue_id, int part_id, std::string* plname)
{
	if (dialogue_id >= B_DIALOGUES_ENTRIES) return str_err;

	for (std::list<s_part>::iterator it = dialogues[dialogue_id].parts.begin(); it != dialogues[dialogue_id].parts.end(); it++)
	{
		if (it->id == part_id)
		{
			std::string s(it->text);
			const std::string rp("%name");

			str_replace(s, rp, *plname);

			return s;
		}
	}

	return str_err;
}

s_part_answer *b_dialogues::get_answer(uint dialogue_id, uint part_id, int answer_id)
{
	if (dialogue_id >= B_DIALOGUES_ENTRIES) return NULL;

	for (std::list<s_part>::iterator it = dialogues[dialogue_id].parts.begin(); it != dialogues[dialogue_id].parts.end(); it++)
	{
		if (it->id == part_id)
		{
			if ((uint) answer_id < it->answers.size())
			{
				return &it->answers.at(answer_id);
			}
			else return NULL;
		}
	}

	return NULL;
}



s_dialogue *b_dialogues::at(uint id)
{
	if (id > B_DIALOGUES_ENTRIES) {err("b_dialogues::at was called with invalid id", -1); return NULL;}
	return &dialogues[id];
}


void b_dialogues::err(const char *msg, int id)
{
	std::ostringstream pr;
	pr << "ERROR in b_dialogues in id=" << id << " :" << msg;

#ifdef ACKNEX
	error((char*)pr.str().c_str());
#else
	std::cout << pr.str() << std::endl;
#endif
}
