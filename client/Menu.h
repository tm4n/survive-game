#ifndef __MENU_H__
#define __MENU_H__

#define SELECTION_MAX 5

#define GUI_GROUP_OPTIONS 5
#define GUI_GROUP_HELP 6

#include "GUI.h"
#include "ResourceLoader.h"
#include "gameClient.h"
#include "gui_serverlist.h"

class Menu
{
public:

	Menu(GUI* gui, ResourceLoader *resources, GUICallback *playMpCb, GUICallback *playCb, GUICallback *quitCb);

	void show();
	void hide();
	
	#ifndef ANDROID
	void options_show();
	void options_hide();
	void options_update();

	void options_safe_to_settings();
	void options_load_from_settings();
	#endif

	void svlist_show();
	void svlist_hide();

	void help_show();
	void help_hide();

	void snd_click();

	void enable_inputbox(int id);
	void disable_inputbox();

	void event_input_keys(SDL_Event *evt);
	void frame(double);

	bool visible;
	
	ResourceLoader *resources;
	GUI *gui;
	gui_serverlist *svlist;

#define OPT_NUM_SCREENRESOLUTIONS 18
	static const int screenresolutions_x[OPT_NUM_SCREENRESOLUTIONS];
	static const int screenresolutions_y[OPT_NUM_SCREENRESOLUTIONS];

	bool mouseinv, fullscreen, shader, antialias;
	std::string str_playername, str_mousesens, str_volume;
	int screenres_index;

	int current_inputbox;
	std::string *current_inputstring;

	static std::string str_help;

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
	
	int help_close_bt;

	int button_ids[SELECTION_MAX];
	int button_sel_ids[SELECTION_MAX];

	double caret_animation;
	bool caret_visible;
};


#endif
