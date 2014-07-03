#ifndef __GUI_HUD_H__
#define __GUI_HUD_H__

#include "GUI.h"
#include "ResourceLoader.h"

class gui_hud
{
public:
	enum hud_state {hidden, spectating, playing, game_end};
	
	gui_hud(GUI *gui, ResourceLoader *resources);
	~gui_hud();

	void set_state(hud_state new_state);

	void frame(double time_frame, float health, int ammo, int magazin, int wave, int points);

protected:
	GUI *gui;
	ResourceLoader *resources;
	
	hud_state state;

	int debug_id, ammo_txt_id, ammo_mag_txt_id, ammo_bg_id, health_txt_id, health_bg_id, crosshair_id;
	int status_id, message_id, wave_points_id;
};


#endif
