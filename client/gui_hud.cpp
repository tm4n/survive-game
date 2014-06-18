#include "gui_hud.h"

#include <sstream>

gui_hud::gui_hud(GUI *gui, ResourceLoader *resources)
{
	this->gui = gui;
	this->resources = resources;

	// create debug info
	SDL_Color c = {255, 255, 255};
	Texture *t = new Texture("TEST!", resources->getFont(ResourceLoader::fontType::fnt_mid), c);
	debug_id = gui->addText(t, 1, 0.1f, 0.1f);

	// create health info
	health_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiHealth), 1, 0.1f, 0.8f);
	gui->setCentered(health_bg_id, true);
	t = new Texture("100", resources->getFont(ResourceLoader::fontType::fnt_mid), c);
	health_txt_id = gui->addText(t, 1, 0.1f, 0.8f);
	gui->setCentered(health_txt_id, true);

	// create ammo info
	ammo_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiAmmo), 1, 0.8f, 0.8f);
	gui->setCentered(ammo_bg_id, true);
	t = new Texture("50", resources->getFont(ResourceLoader::fontType::fnt_mid), c);
	ammo_txt_id = gui->addText(t, 1, 0.85f, 0.85f);
	gui->setCentered(ammo_txt_id, true);
	t = new Texture("50", resources->getFont(ResourceLoader::fontType::fnt_mids), c);
	ammo_mag_txt_id = gui->addText(t, 1, 0.85f, 0.85f);
	gui->setCentered(ammo_txt_id, true);

	// create crosshair
	crosshair_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiCrosshair), 1, 0.5f, 0.5f);
	gui->setCentered(crosshair_id, true);
}

gui_hud::~gui_hud()
{
}

// ´TODO: replace hide/show with set_state
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

	s << "Health: " << health << ", Ammo: " << ", Magazin: " << magazin << ", Wave: " << wave
		<< ", Wave: " << ", Points: " << points;

	SDL_Color c = {255, 255, 255};
	Texture *t = new Texture(s.str().c_str(), resources->getFont(ResourceLoader::fontType::fnt_mid), c);
	gui->updateTexture(debug_id, t);
}