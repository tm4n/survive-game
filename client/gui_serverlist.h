#ifndef __GUI_SERVERLIST_H__
#define __GUI_SERVERLIST_H__

#include "GUI.h"
#include "ResourceLoader.h"
#include "defs.h"

class gui_serverlist
{
public:
	gui_serverlist(GUI* gui, ResourceLoader *resources, GUICallback *playMpCb);
	virtual ~gui_serverlist();
	
	void show();
	void hide();

	void frame(double time_delta);

protected:
	GUI *gui;
	ResourceLoader *resources;

	int bg_id, black_bg_id;
	int txt_placeholder, but_ok;
};

#endif