#ifndef __GUI_SERVERLIST_H__
#define __GUI_SERVERLIST_H__

#include "GUI.h"
#include "ResourceLoader.h"
#include "defs.h"

class gui_serverlist
{
public:
	gui_serverlist(GUI* gui, ResourceLoader *resources, GUICallback *playMpCb, GUICallback *closeSvlistCb);
	virtual ~gui_serverlist();
	
	void show();
	void hide();

	void frame(double time_delta);

	std::string conn_ip;
	int conn_port;

protected:
	GUI *gui;
	ResourceLoader *resources;
	GUICallback * playMpCb;

	int bg_id, black_bg_id;
	int txt_placeholder, txt_placeholder_head;
	int but_connect, but_close;
};

#endif