#ifndef __MENU_H__
#define __MENU_H__

#define SELECTION_MAX 5


#include "GUI.h"
#include "ResourceLoader.h"
#include "gameClient.h"

class Menu
{
public:

	Menu(GUI* gui, ResourceLoader *resources, GUICallback *playCb, GUICallback *quitCb);

	void show();
	void hide();
	
	void options_show();
	void options_hide();

	void snd_click();

	void frame();

	bool visible;
	
	ResourceLoader *resources;
	GUI *gui;

private:

	int bg_id, black_bg_id;

	int options_bg_id, options_cancel_bt, options_cancel_txt, options_ok_bt, options_ok_txt;

	int button_ids[SELECTION_MAX];
	int button_sel_ids[SELECTION_MAX];
};


#endif
