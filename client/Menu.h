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

	int st_bg_id, st_cancel_bt, st_cancel_txt, st_ok_bt, st_ok_txt;
	int st_playername_head, st_playername_bt, st_playername_txt;
	int st_mousesens_head, st_mousesens_bt, st_mousesens_txt;
	int st_mouseinv_bt, st_mouseinv_txt;
	int st_volume_head, st_volume_bt, st_volume_txt;
	int st_res_head, st_res_bt_left, st_res_bt_right, st_res_txt;
	int st_fullscreen_bt, st_fullscreen_txt;
	//int st_shadows_bt, st_shadows_txt; //TODO: implement shadows
	int st_shader_bt, st_shader_txt;
	int st_antialias_bt, st_antialias_txt;

	int button_ids[SELECTION_MAX];
	int button_sel_ids[SELECTION_MAX];
};


#endif
