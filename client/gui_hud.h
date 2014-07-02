#ifndef __GUI_HUD_H__
#define __GUI_HUD_H__

#include "GUI.h"
#include "ResourceLoader.h"

class gui_hud
{
public:
	gui_hud(GUI *gui, ResourceLoader *resources);
	~gui_hud();

	void show();
	void hide();

	void frame(double time_frame, float health, int ammo, int magazin, int wave, int points);

protected:
	GUI *gui;
	ResourceLoader *resources;

	int debug_id, ammo_txt_id, ammo_mag_txt_id, ammo_bg_id, health_txt_id, health_bg_id, crosshair_id;
	int status_id, message_id, wave_points_id;
};


#endif