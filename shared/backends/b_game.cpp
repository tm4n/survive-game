#include "b_game.h"
#include "tinyxml.h"
#include "helper.h"

#include <iostream>
#include <sstream>

#ifdef ACKNEX
#include <windows.h>
#include "adll.h"
#endif

b_game *b_ga;


b_game::b_game(const char *filename)
{
	const char* chr;

    std::string full_filename;
	get_workdir(&full_filename);
	full_filename.append(filename);

	TiXmlDocument doc(full_filename);
	if (!doc.LoadFile()) {err("Couldn't open file for game backend!");}

	TiXmlHandle hDoc(&doc);
	TiXmlElement *pElem;

	// document base
	pElem=hDoc.FirstChildElement().Element();
	// should always have a valid root but handle gracefully if it does
	if (!pElem) {err("backend game: Root Element not found, file empty?"); return;}
	if (strcmp(pElem->Value(), "backend_game") != 0) {err("backend_game definition not found"); return;}

	if ((chr = pElem->Attribute("name")) == NULL) {err("game name is missing"); return;}
	game_name.assign(chr);

	if ((chr = pElem->Attribute("login_ip_address")) == NULL) {err("login_ip_address is missing"); return;}
	login_ip_address.assign(chr);

	if (pElem->QueryIntAttribute ("login_port", &login_port) != TIXML_SUCCESS) {err("login_port is missing or invalid"); return;}
}



void b_game::err(const char *msg)
{
	std::ostringstream pr;
	pr << "ERROR in b_game:" << msg;

#ifdef ACKNEX
	error((char*)pr.str().c_str());
#else
	std::cout << pr.str() << std::endl;
#endif
}
