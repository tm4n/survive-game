#include "b_dropsets.h"
#include "tinyxml.h"
#include "b_items.h"
#include "helper.h"

#include <iostream>
#include <sstream>

#ifdef ACKNEX
#include <windows.h>
#include "adll.h"
#endif

b_dropsets *b_drp;


b_dropsets::b_dropsets(const char *filename)
{
	int id;
	s_dropset *dps;
    std::string str;

    std::string full_filename;
	get_workdir(&full_filename);
	full_filename.append(filename);

	TiXmlDocument doc(full_filename);
	if (!doc.LoadFile()) {err("Couldn't open file for dropsets backend!", -1);}

	TiXmlHandle hDoc(&doc);
	TiXmlElement *pElem, *pSubElem;;
	TiXmlHandle hRoot(0);

	// document base
	pElem=hDoc.FirstChildElement().Element();
	// should always have a valid root but handle gracefully if it does
	if (!pElem) {err("item dropsets: Root Element not found, file empty?", -1); return;}
	if (strcmp(pElem->Value(), "backend_dropsets") != 0) {err("backend_items definition not found", -1); return;}

	// save this for later
	hRoot=TiXmlHandle(pElem);

	// get all entries
	pElem=hRoot.FirstChild().Element();
	for( ; pElem; pElem=pElem->NextSiblingElement())
	{
		///////////////////////////////////////////
		// class definition
		if (strcmp(pElem->Value(), "dropset") == 0)
		{

			if (pElem->QueryIntAttribute ("id", &id) != TIXML_SUCCESS) {err("id of dropset missing or invalid", id); return;}
			if (id < 0 || id >= B_DROPSETS_ENTRIES) {err("id of dropset is invalid", id); return;}

			dps = &(dropsets[id]);


            // Drops

            s_drop d;

			for(pSubElem=pElem->FirstChildElement(); pSubElem; pSubElem=pSubElem->NextSiblingElement())
			{

				if (strcmp(pSubElem->Value(), "drop") == 0)
				{
					uint iid;
                    if (pSubElem->QueryUnsignedAttribute("item_id", &iid) != TIXML_SUCCESS) {err("item_id value of dropset drop missing or invalid", id); return;}
					d.item_id = (ushort)iid;
					if (pSubElem->QueryFloatAttribute("chance", &(d.chance)) != TIXML_SUCCESS) {err("chance value of dropset drop missing or invalid", id); return;}

                    dps->drops.push_back(d);
				}
			}

		}
	}
}


s_dropset *b_dropsets::at(uint id)
{
	if (id > B_DROPSETS_ENTRIES) {err(" b_dropsets::att was called with invalid id", -1); return NULL;}
	return &dropsets[id];
}


// fill given inventory with loot
bool b_dropsets::fill_loot(item_cont *ct, uint dropset_id)
{
    s_dropset *dps = at(dropset_id);
    if (dps == NULL) {err("b_dropsets::fill_loot was called with invalid dropset_id", dropset_id); return false;}

    for (std::list<s_drop>::iterator it = dps->drops.begin(); it != dps->drops.end(); it++)
    {

        if (rand() % 100 <= it->chance)
        {
                // create item
                item *s = b_it->create_item(it->item_id);
                ct->add(s);
                log (LOG_DEBUG, "added item");
        }

    }


    return true;
}



void b_dropsets::err(const char *msg, int id)
{
	std::ostringstream pr;
	pr << "ERROR in b_dropsets in id=" << id << " :" << msg;

#ifdef ACKNEX
	error(pr.str().c_str());
#else
	std::cout << pr.str() << std::endl;
#endif
}
