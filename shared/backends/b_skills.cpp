#include "b_skills.h"
#include "tinyxml.h"
#include "helper.h"

#include <iostream>
#include <sstream>
#include <cmath>

#ifdef ACKNEX
#include <windows.h>
#include "adll.h"
#endif

b_skills *b_sk;


b_skills::b_skills(const char *filename)
{
	int id;
	const char *chr;
    std::string str;
	s_skill *sk;

	// clear all entries
	for (int i = 0; i < B_SKILLS_ENTRIES; i++)
	{
		skills[i].valid = false;
		skills[i].bclass_id = 0;
	}


	// load file
    std::string full_filename;
	get_workdir(&full_filename);
	full_filename.append(filename);

	TiXmlDocument doc(full_filename);
	if (!doc.LoadFile()) {err("Couldn't open file for skills backend!", -1);}

	TiXmlHandle hDoc(&doc);
	TiXmlElement *pElem, *pSubElem;
	TiXmlHandle hRoot(0);

	// document base
	pElem=hDoc.FirstChildElement().Element();
	// should always have a valid root but handle gracefully if it does
	if (!pElem) {err("backend skills: Root Element not found, file empty?", -1); return;}
	if (strcmp(pElem->Value(), "backend_skills") != 0) {err("backend_skills definition not found", -1); return;}

	// save this for later
	hRoot=TiXmlHandle(pElem);

	// get all entries
	pElem=hRoot.FirstChild().Element();
	for( ; pElem; pElem=pElem->NextSiblingElement())
	{
		///////////////////////////////////////////
		// class definition
		if (strcmp(pElem->Value(), "skill") == 0)
		{

			if (pElem->QueryIntAttribute ("id", &id) != TIXML_SUCCESS) {err("id of skill missing or invalid", id); return;}
			if (id < 0 || id >= B_SKILLS_ENTRIES) {err("id of skill is invalid", id); return;}

			sk = &(skills[id]);

			if (pElem->QueryIntAttribute("class_id", &(sk->bclass_id)) != TIXML_SUCCESS) {err("class_id of skill missing or invalid", id); return;}

			if (pElem->QueryIntAttribute("skilltree_cat", &(sk->bskilltree_cat)) != TIXML_SUCCESS) sk->bskilltree_cat = 0;

			if (pElem->QueryIntAttribute("skilltree_num", &(sk->bskilltree_num)) != TIXML_SUCCESS) sk->bskilltree_num = 0;


			if ((chr = pElem->Attribute("name")) == NULL) {err("name of skill is missing", id); return;}
			sk->bname.assign(chr);

            #ifdef ACKNEX
			if ((chr = pElem->Attribute("icon")) == NULL) {err("icon of skill is missing", id); return;}
			sk->icon = bmap_create((char*)chr);
			if (sk->icon == NULL) {err("icon of skill could not be loaded", id); return;}
			#endif

			if ((chr = pElem->Attribute("desc")) == NULL) {err("desc of skill is missing", id); return;}
			sk->bdesc.assign(chr);

			if (pElem->QueryFloatAttribute("manacost_start", &(sk->bmanacost_start)) != TIXML_SUCCESS) {err("manacost_start of skill missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute("manacost_add", &(sk->bmanacost_add)) != TIXML_SUCCESS) {err("manacost_add of skill missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute("cooldown_start", &(sk->bcooldown_start)) != TIXML_SUCCESS) {err("cooldown_start of skill missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute("cooldown_add", &(sk->bcooldown_add)) != TIXML_SUCCESS) {err("cooldown_add of skill missing or invalid", id); return;}

			if ((chr = pElem->Attribute("target_type")) == NULL) {err("target_type of a skill missing or invalid", id); return;}
			sk->btarget_type = -1;
			if (strcmp(chr, "none") == 0) sk->btarget_type = SK_TARGET_TYPE_NONE;
			if (strcmp(chr, "actor_friendly") == 0) sk->btarget_type = SK_TARGET_TYPE_ACTOR_FRIENDLY;
			if (strcmp(chr, "actor_enemy") == 0) sk->btarget_type = SK_TARGET_TYPE_ACTOR_ENEMY;
			if (strcmp(chr, "actor_any") == 0) sk->btarget_type = SK_TARGET_TYPE_ACTOR_ANY;
			if (strcmp(chr, "ground") == 0) sk->btarget_type = SK_TARGET_TYPE_GROUND;
			if (sk->btarget_type == -1) {err("target_type of a skill is unknown", id); return;}

			if (pElem->QueryIntAttribute("range", &(sk->brange)) != TIXML_SUCCESS) sk->brange = 0;

			sk->valid = true;

            // query subelements
            s_sreq req;
			s_action act;

			for(pSubElem=pElem->FirstChildElement(); pSubElem; pSubElem=pSubElem->NextSiblingElement())
			{

				////////////////////////////////////////
				// REQUIREMENT

				if (strcmp(pSubElem->Value(), "requirement") == 0)
				{
					if ((chr = pSubElem->Attribute("type")) == NULL) {err("requirement type of a skill missing or invalid", id); return;}
					req.type = -1;
					if (strcmp(chr, "level") == 0) req.type = SK_REQUIREMENT_LEVEL;
					if (strcmp(chr, "strength") == 0) req.type = SK_REQUIREMENT_STRE;
					if (strcmp(chr, "agility") == 0) req.type = SK_REQUIREMENT_AGIL;
					if (strcmp(chr, "intelligence") == 0) req.type = SK_REQUIREMENT_INTEL;
					if (strcmp(chr, "skill") == 0) req.type = SK_REQUIREMENT_SKILL;
					if (req.type == -1) {err("requirement type of a skill is unknown", id); return;}

                    if (pSubElem->QueryIntAttribute("value", &(req.val)) != TIXML_SUCCESS) {err("requirement value of a skill missing or invalid", id); return;}

					sk->requirements.push_back(req);
				}


				/////////////////////////////////////////
				// ACTION

                if (strcmp(pSubElem->Value(), "action") == 0)
				{
					if ((chr = pSubElem->Attribute("type")) == NULL) {err("action type of a skill missing or invalid", id); return;}
					act.type = -1;
					if (strcmp(chr, "instant") == 0) act.type = SK_ACTION_TYPE_INSTANT;
					if (strcmp(chr, "wave") == 0) act.type = SK_ACTION_TYPE_WAVE;
					if (strcmp(chr, "projectile") == 0) act.type = SK_ACTION_TYPE_PROJECTILE;
					if (act.type == -1) {err("action type of a skill is unknown", id); return;}

					if ((chr = pSubElem->Attribute("hit")) == NULL) {err("action hit of a skill missing or invalid", id); return;}
					act.hit_type = -1;
					if (strcmp(chr, "buff_heal") == 0) act.hit_type = BUFF_HEAL;
					if (strcmp(chr, "buff_damage") == 0) act.hit_type = BUFF_DAMAGE;
					if (strcmp(chr, "buff_manaregen") == 0) act.hit_type = BUFF_MANAREGEN;
					if (strcmp(chr, "buff_manadrain") == 0) act.hit_type = BUFF_MANADRAIN;
					if (strcmp(chr, "buff_slowdown") == 0) act.hit_type = BUFF_SLOWDOWN;
					if (strcmp(chr, "buff_speed") == 0) act.hit_type = BUFF_SPEED;
					if (strcmp(chr, "change_health") == 0) act.hit_type = SK_ACTION_CHANGE_HEALTH;
					if (strcmp(chr, "change_mana") == 0) act.hit_type = SK_ACTION_CHANGE_MANA;
					if (act.hit_type == -1) {err("hit type of a skill is unknown", id); return;}

					if ((chr = pSubElem->Attribute("affected")) == NULL) {err("action affected of a skill missing or invalid", id); return;}
					act.affected = -1;
					if (strcmp(chr, "actor_all") == 0) act.affected = SK_ACTION_AFFECTED_ALL;
					if (strcmp(chr, "actor_enemy") == 0) act.affected = SK_ACTION_AFFECTED_ENEMY;
					if (strcmp(chr, "actor_friendly") == 0) act.affected = SK_ACTION_AFFECTED_FRIENDLY;
					if (strcmp(chr, "actor_self") == 0) act.affected = SK_ACTION_AFFECTED_SELF;
					if (act.affected == -1) {err("affected type of a skill is unknown", id); return;}

                    if (pSubElem->QueryFloatAttribute("speed", &(act.speed)) != TIXML_SUCCESS) act.speed = 0;

                    if (pSubElem->QueryFloatAttribute("splash_range", &(act.splash_range)) != TIXML_SUCCESS) act.splash_range = 0;

					if (pSubElem->QueryFloatAttribute("val1_start", &(act.val1_start)) != TIXML_SUCCESS) {err("val1_start value of a skill action missing or invalid", id); return;}
					if (pSubElem->QueryFloatAttribute("val1_add", &(act.val1_add)) != TIXML_SUCCESS) {err("val1_add value of a skill action missing or invalid", id); return;}

					if (pSubElem->QueryFloatAttribute("val2_start", &(act.val2_start)) != TIXML_SUCCESS) act.val2_start = 0;
					if (pSubElem->QueryFloatAttribute("val2_add", &(act.val2_add)) != TIXML_SUCCESS) act.val2_add = 0;

					if ((chr = pSubElem->Attribute("file")) != NULL) act.file.assign(chr);

					if (pSubElem->QueryIntAttribute("effect_start", &(act.effect_start)) != TIXML_SUCCESS) act.effect_start = 0;

					if (pSubElem->QueryIntAttribute("effect_projectile", &(act.effect_projectile)) != TIXML_SUCCESS) act.effect_projectile = 0;

					if (pSubElem->QueryIntAttribute("effect_hit", &(act.effect_hit)) != TIXML_SUCCESS) act.effect_hit = 0;


					sk->actions.push_back(act);
				}

			}

		}
	}
}




s_skill *b_skills::at(uint id)
{
	if (id > B_SKILLS_ENTRIES) {err("b_skills::att was called with invalid id", -1); return NULL;}
	if (skills[id].valid == false) return NULL;
	return &skills[id];
}


float b_skills::get_cooldown(uint id, ushort level)
{
	if (id > B_SKILLS_ENTRIES) {err("b_skills::get_cooldown was called with invalid id", -1); return (float)0.01;}

	return skills[id].bcooldown_start + (skills[id].bcooldown_add*level);
}

float b_skills::get_manacost(uint id, ushort level)
{
	if (id > B_SKILLS_ENTRIES) {err("b_skills::get_manacost was called with invalid id", -1); return (float)0.01;}

	return skills[id].bmanacost_start + (skills[id].bmanacost_add*level);
}



std::string b_skills::get_skill_desc(uint id, ushort level)
{
	std::ostringstream s;
	if (id > B_SKILLS_ENTRIES) {err("b_skills::get_skill_desc was called with invalid id", -1); return s.str();}

	// parse string
	std::string strtemp(skills[id].bdesc);
	if (skills[id].actions.size() > 0)
	{
		std::vector<s_action>::iterator i = skills[id].actions.begin();
		str_assemble(&strtemp, 0, 0, fabs(i->val1_start + i->val1_add*(float)level), fabs(i->val2_start + i->val2_add*(float)level), &skills[id].bname, NULL, NULL);
	}


	s << strtemp;

	return s.str();
}

std::string b_skills::get_skill_info(uint id, ushort level)
{
	std::ostringstream s;
	if (id > B_SKILLS_ENTRIES) {err("b_skills::get_skill_info was called with invalid id", -1); return s.str();}

	s << skills[id].bname << std::endl
		<< "Level: " << level << std::endl;

	float manacost = get_manacost(id, level);
	if (manacost > 0) s << "Mana: " << manacost << std::endl << std::endl;

	s << get_skill_desc(id, level);

	return s.str();
}


void b_skills::err(const char *msg, int id)
{
	std::ostringstream pr;
	pr << "ERROR in b_skills in id=" << id << " :" << msg;

#ifdef ACKNEX
	error((char*)pr.str().c_str());
#else
	std::cout << pr.str() << std::endl;
#endif
}
