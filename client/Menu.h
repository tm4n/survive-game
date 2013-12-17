#ifndef __MENU_H__
#define __MENU_H__

#define SELECTION_MAX 5


#include "GUI.h"
#include "ResourceLoader.h"

class Menu
{
public:

	Menu(GUI* gui, ResourceLoader *resources);

	void show();
	void hide();

	void frame();

	bool visible;

private:
	GUI *gui;
	ResourceLoader *resources;

	int bg_id;

	int button_ids[SELECTION_MAX];
	int button_sel_ids[SELECTION_MAX];
};


#endif