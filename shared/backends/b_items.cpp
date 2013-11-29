#include "b_items.h"
#include "tinyxml.h"
#include "helper.h"

#include <iostream>
#include <sstream>

#ifdef ACKNEX
#include <windows.h>
#include "adll.h"
#endif

#include "b_classes.h" // needed for item description

b_items *b_it;


b_items::b_items(const char *filename)
{
	int id;
	s_item *it;
	const char *chr;
    std::string str;

    std::string full_filename;
	get_workdir(&full_filename);
	full_filename.append(filename);

	TiXmlDocument doc(full_filename);
	if (!doc.LoadFile()) {err("Couldn't open file for items backend!", -1);}

	TiXmlHandle hDoc(&doc);
	TiXmlElement *pElem, *pSubElem;;
	TiXmlHandle hRoot(0);

	// document base
	pElem=hDoc.FirstChildElement().Element();
	// should always have a valid root but handle gracefully if it does
	if (!pElem) {err("item backend: Root Element not found, file empty?", -1); return;}
	if (strcmp(pElem->Value(), "backend_items") != 0) {err("backend_items definition not found", -1); return;}

	// save this for later
	hRoot=TiXmlHandle(pElem);

	// get all entries
	pElem=hRoot.FirstChild().Element();
	for( ; pElem; pElem=pElem->NextSiblingElement())
	{
		///////////////////////////////////////////
		// class definition
		if (strcmp(pElem->Value(), "item") == 0)
		{

			if (pElem->QueryIntAttribute ("id", &id) != TIXML_SUCCESS) {err("id of item missing or invalid", id); return;}
			if (id < 0 || id >= B_ITEMS_ENTRIES) {err("id of item is invalid", id); return;}

			it = &(items[id]);

			if ((chr = pElem->Attribute("file")) == NULL) {err("file of item is missing", id); return;}
			it->bfile.assign(chr);

			#ifdef ACKNEX
            str = it->bfile.substr(0, it->bfile.length() - 3);
            str.append("tga");
            it->icon = (void*)bmap_create((char*)str.c_str());
			#endif

			if ((chr = pElem->Attribute("name")) == NULL) {err("name of item is missing", id); return;}
			it->bname.assign(chr);

			if ((chr = pElem->Attribute("category")) == NULL) {err("category of item is missing", id); return;}
			it->bcategory = 0;
			if (strcmp(chr, "other") == 0) it->bcategory = IT_CAT_OTHER;
			if (strcmp(chr, "weapon_rhand") == 0) it->bcategory = IT_CAT_WEAPON_RHAND;
			if (strcmp(chr, "ring") == 0) it->bcategory = IT_CAT_RING;
			if (strcmp(chr, "potion_health") == 0) it->bcategory = IT_CAT_POTION_HEALTH;
			if (strcmp(chr, "potion_mana") == 0) it->bcategory = IT_CAT_POTION_MANA;
			if (it->bcategory == 0) err("category of item is unkown", id);

			if (pElem->QueryIntAttribute("effect", &(it->beffect)) != TIXML_SUCCESS) it->beffect = 0;

			if (pElem->QueryIntAttribute("val1_min", &(it->bval_min[0])) != TIXML_SUCCESS) it->bval_min[0] = 0;

			if (pElem->QueryIntAttribute("val1_max", &(it->bval_max[0])) != TIXML_SUCCESS) it->bval_min[0] = 0;

            if (pElem->QueryIntAttribute("val2_min", &(it->bval_min[1])) != TIXML_SUCCESS) it->bval_min[1] = 0;

			if (pElem->QueryIntAttribute("val2_max", &(it->bval_max[1])) != TIXML_SUCCESS) it->bval_max[1] = 0;

			if (pElem->QueryIntAttribute("gold_value", &(it->bgold_value)) != TIXML_SUCCESS) {err("gold_value of item missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute("speed", &(it->bspeed)) != TIXML_SUCCESS) it->bspeed = 0;

			if (pElem->QueryFloatAttribute("range", &(it->brange)) != TIXML_SUCCESS) it->brange = 0;

            // Restrictions

            s_restr r;

			for(pSubElem=pElem->FirstChildElement(); pSubElem; pSubElem=pSubElem->NextSiblingElement())
			{

				if (strcmp(pSubElem->Value(), "restrict_level") == 0)
				{
                    r.type = IT_RESTR_LEVEL;
                    if (pSubElem->QueryIntAttribute("value", &(r.val)) != TIXML_SUCCESS) {err("restriction_level value of item missing or invalid", id); return;}
                    it->restrictions.push_back(r);
				}

                if (strcmp(pSubElem->Value(), "restrict_class") == 0)
				{
                    r.type = IT_RESTR_CLASS;
                    if (pSubElem->QueryIntAttribute("value", &(r.val)) != TIXML_SUCCESS) {err("restriction_class value of item missing or invalid", id); return;}
                    it->restrictions.push_back(r);
				}

                if (strcmp(pSubElem->Value(), "restrict_stre") == 0)
				{
                    r.type = IT_RESTR_STRE;
                    if (pSubElem->QueryIntAttribute("value", &(r.val)) != TIXML_SUCCESS) {err("restriction_stre value of item missing or invalid", id); return;}
                    it->restrictions.push_back(r);
				}

                if (strcmp(pSubElem->Value(), "restrict_agil") == 0)
				{
                    r.type = IT_RESTR_AGIL;
                    if (pSubElem->QueryIntAttribute("value", &(r.val)) != TIXML_SUCCESS) {err("restriction_agil value of item missing or invalid", id); return;}
                    it->restrictions.push_back(r);
				}

				if (strcmp(pSubElem->Value(), "restrict_intel") == 0)
				{
                    r.type = IT_RESTR_INTEL;
                    if (pSubElem->QueryIntAttribute("value", &(r.val)) != TIXML_SUCCESS) {err("restriction_intel value of item missing or invalid", id); return;}
                    it->restrictions.push_back(r);
				}
			}

		}
	}
}

s_item *b_items::at(ushort id)
{
	if (id > B_ITEMS_ENTRIES) {err(" b_items::att was called with invalid id", -1); return NULL;}
	return &items[id];
}

// get if item is usable
bool b_items::useable(ushort it_id)
{
	uint cat = at(it_id)->bcategory;
	return (cat == IT_CAT_POTION_HEALTH || cat == IT_CAT_POTION_MANA);
}


item *b_items::create_item(ushort it_id)
{

    s_item *itv = at(it_id);
    if (itv == NULL) {err(" b_items::create_item was called with invalid id", -1); return NULL;}

    item *it = new item(it_id);

    for (int i = 0; i < ITEM_NUM_VALUES; i++)
    {
        if (itv->bval_max[i] != 0)
            it->val[i] = (char)(itv->bval_min[i] + (rand() % (itv->bval_max[i]- itv->bval_min[i]+1)));
        else it->val[i] = 0;
    }

    // TODO: special properties
    for (int i = 0; i < ITEM_NUM_SPECIAL; i++) it->sp[i] = 0;

    return it;

}


// get item description
void b_items::get_desc(std::string *str_out, item* it, int show_value)
{

	std::ostringstream str;
	str << at(it->id)->bname << std::endl;
	// make decision by type
	switch (at(it->id)->bcategory)
	{

		case IT_CAT_WEAPON_RHAND:
			str << std::endl << "Damage: " << (int)(it->val)[0];
			str << " - " << (int)(it->val)[1];
		break;

		case IT_CAT_POTION_HEALTH:
			str << std::endl << "Heals " << (int)(it->val)[0] << " health points";
		break;

		case IT_CAT_POTION_MANA:
			str << std::endl << "Refreshs " << (int)(it->val)[0] << " mana points";
		break;

		default:
			str << std::endl << "Value: " << (int)(it->val)[0];
			str << " - " << (int)(it->val)[1];
	}

	// description for restrictions
	for (std::list<s_restr>::iterator itr = at(it->id)->restrictions.begin(); itr != at(it->id)->restrictions.end(); itr++)
	{
		if (itr->type != 0)
		{
			if (itr->type == IT_RESTR_CLASS)
				str << std::endl << "Only for " << b_cl->at(itr->val)->bname;

			if (itr->type == IT_RESTR_LEVEL)
				str << std::endl << "Needs Level " << itr->val;

			if (itr->type == IT_RESTR_STRE)
				str << std::endl << "Needs " << itr->val << " Strength";

			if (itr->type == IT_RESTR_AGIL)
				str << std::endl << "Needs " << itr->val << " Agility";

			if (itr->type == IT_RESTR_INTEL)
				str << std::endl << "Needs " << itr->val << " Intelligence";

		}
	}

	//TODO: add description of special type

	if (useable(it->id)) str << std::endl << "Useable";

	if (show_value == 1 || show_value == 0)
	{
		str << std::endl << "Price: " << at(it->id)->bgold_value;
		if (show_value == 1) str << std::endl << "Buy with right-click.";
	}
	if (show_value == 2)
	{
		str << std::endl << "Selling price: " << at(it->id)->bgold_value/(int)2;
		str << std::endl << "Sell with right-click.";

	}

	if (show_value == 0 && useable(it->id)) str << std::endl << "Use with right-click.";

	// save into output str
	str_out->assign(str.str());
}



void b_items::err(const char *msg, int id)
{
	std::ostringstream pr;
	pr << "ERROR in b_items in id=" << id << " :" << msg;

#ifdef ACKNEX
	error((char*)pr.str().c_str());
#else
	std::cout << pr.str() << std::endl;
#endif
}
