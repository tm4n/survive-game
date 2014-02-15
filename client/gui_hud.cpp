#include "gui_hud.h"

#include <sstream>

gui_hud::gui_hud(GUI *gui, ResourceLoader *resources)
{
	this->gui = gui;
	this->resources = resources;

	//create
	SDL_Color c = {255, 255, 255};
	Texture *t = new Texture("TEST!", resources->getFont(), c);
	debug_id = gui->addText(t, 1, 0.1f, 0.1f);
}

gui_hud::~gui_hud()
{
}

void gui_hud::show()
{
	gui->setVisible(debug_id, true);
}

void gui_hud::hide()
{
	gui->setVisible(debug_id, false);
}

void gui_hud::frame(double time_frame, float health, int ammo, int magazin, int wave, int points)
{
	std::ostringstream s;

	s << "Health: " << health << "\nAmmo: " << "\nMagazin: " << magazin << "\nWave: " << wave
		<< "\nWave: " << "\nPoints: " << points;

	SDL_Color c = {255, 255, 255};
	Texture *t = new Texture(s.str().c_str(), resources->getFont(), c);
	gui->updateTexture(debug_id, t);
}