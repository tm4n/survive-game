#ifndef __GUI_HUD_H__
#define __GUI_HUD_H__

#include "GUI.h"
#include "ResourceLoader.h"
#include "defs.h"
#include "net_cl.h"

class gui_hud
{
public:
	enum hud_state {hidden, spectating, playing, game_end};
	
	gui_hud(GUI *gui, ResourceLoader *resources, net_cl *net_client, bool *quit);
	~gui_hud();

	void set_state(hud_state new_state);

	void set_debug(std::string s);

	void frame(double time_frame, float health, int ammo, int magazin, int wave, uint points);

	void show_status_end();
	void show_status_join(bool respawn);
	void show_status_respawn_timer(int respawn_timer);
	void show_status_connecting();
	void show_status_noserver();
	void show_status_wrongver();
	void hide_status();

	void show_message(const std::string &str);
	bool message_visible();

	void show_wave_timer(int time);
	void hide_wave_timer();

	void scroll_chat(const char *msg);
	void chat_enable();
	void chat_cancel();
	void chat_enter();
	void chat_input_update();
	bool chat_carvis;
	bool chat_active;
	std::string chat_inputstring;

	void show_scoreboard();
	void update_scoreboard(const char *sdata);
	void hide_scoreboard();
	bool scoreboard_visible;

	void toggle_ingame_menu();
	void show_ingame_menu();
	void hide_ingame_menu();
	bool ingame_menu_visible;

protected:
	GUI *gui;
	ResourceLoader *resources;
	net_cl *net_client;
	bool *quit;

	float scoreboard_timer;
	float msg_timer;
	double chat_caranim;

	std::string chatlines[5];
	
	hud_state state;

	int debug_id, ammo_txt_id, ammo_mag_txt_id, ammo_bg_id, health_txt_id, health_bg_id, crosshair_id;
	int status_id, message_id, wave_points_id, wave_timer_txt_id, wave_timer_id;
	int chat_txt_id, chat_input_txt_id;

	int score_bg_id, score_names_txt_id, score_points_txt_id, score_pings_txt_id, highscore_txt_id;

	int ingame_menu_bg, ingame_but_disconnect, ingame_but_close; // ingame_but_options,

	int controller_help_id;

	std::list<GUICallback*> callbacks;
	std::list<Texture*> textures;
};


#endif
