#ifndef __MENU_H__
#define __MENU_H__

#include "GUI.h"


class Menu
{
public:

	Menu(GUI* gui);

	void show();
	void hide();

	void frame();

private:
	GUI* gui;
};


#endif