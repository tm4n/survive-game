#include "b_classes.h"
#include "tinyxml.h"
#include "helper.h"

#ifdef ACKNEX
#include <windows.h>
#include "adll.h"
#endif

b_classes *b_cl;

b_classes::b_classes(const char *filename)
{
	int id;
	s_class *cl;
	const char *chr;

	std::string full_filename;
	get_workdir(&full_filename);
	full_filename.append(filename);

	TiXmlDocument doc(full_filename);
	if (!doc.LoadFile()) {err("Couldn't open file for classes gui!", -1); return;}

	TiXmlHandle hDoc(&doc);
	TiXmlElement *pElem;
	TiXmlHandle hRoot(0);

	// document base
	pElem=hDoc.FirstChildElement().Element();
	// should always have a valid root but handle gracefully if it does
	if (!pElem) {err("Root Element not found, file empty?", -1); return;}
	if (strcmp(pElem->Value(), "backend_classes") != 0) {err("backend_classes definition not found", -1); return;}

	// save this for later
	hRoot=TiXmlHandle(pElem);

	// get all entries
	pElem=hRoot.FirstChild().Element();
	for( ; pElem; pElem=pElem->NextSiblingElement())
	{
		///////////////////////////////////////////
		// class definition
		if (strcmp(pElem->Value(), "class") == 0)
		{

			if (pElem->QueryIntAttribute ("id", &id) != TIXML_SUCCESS) {err("id of class missing or invalid", id); return;}
			if (id < 0 || id >= B_CLASSES_ENTRIES) {err("id of class is invalid", id); return;}

			cl = &(classes[id]);

			if ((chr = pElem->Attribute("file")) == NULL) {err("file of class is missing", id); return;}
			cl->bfile.assign(chr);

			if ((chr = pElem->Attribute("name")) == NULL) {err("name of class is missing", id); return;}
			cl->bname.assign(chr);

			if ((chr = pElem->Attribute("info")) == NULL) {err("info of class is missing", id); return;}
			cl->binfo.assign(chr);

			if (pElem->QueryFloatAttribute("start_health", &(cl->bstart_health)) != TIXML_SUCCESS) {err("start_health of class missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute("add_health", &(cl->badd_health)) != TIXML_SUCCESS) {err("add_health of class missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute("start_stren", &(cl->bstart_stren)) != TIXML_SUCCESS) {err("start_stren of class missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute("add_stren", &(cl->badd_stren)) != TIXML_SUCCESS) {err("add_stren of class missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute("start_agil", &(cl->bstart_agil)) != TIXML_SUCCESS) {err("start_agil of class missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute("add_agil", &(cl->badd_agil)) != TIXML_SUCCESS) {err("add_agil of class missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute("start_intel", &(cl->bstart_intel)) != TIXML_SUCCESS) {err("start_intel of class missing or invalid", id); return;}

			if (pElem->QueryFloatAttribute("add_intel", &(cl->badd_intel)) != TIXML_SUCCESS) {err("add_intel of class missing or invalid", id); return;}

			if (pElem->QueryIntAttribute("attach_rhand", &(cl->battach_rhand)) != TIXML_SUCCESS) {err("attach_rhand of class missing or invalid", id); return;}

			if (pElem->QueryIntAttribute("attach_rhand_align", &(cl->battach_rhand_align)) != TIXML_SUCCESS) {err("attach_rhand_align of class missing or invalid", id); return;}


			//TODO: finish!
		}
	}
}

s_class *b_classes::at(uint id)
{
	if (id > B_CLASSES_ENTRIES) {err("at was called with invalid id", -1); return NULL;}
	return &classes[id];
}


void b_classes::err(const char *msg, int id)
{
	std::ostringstream pr;
	pr << "ERROR in b_classes in id=" << id << " :" << msg;

#ifdef ACKNEX
	error((char*)pr.str().c_str());
#else
	std::cout << pr.str() << std::endl;
#endif
}
