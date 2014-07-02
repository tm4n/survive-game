#include "gui_hud.h"

#include <sstream>

gui_hud::gui_hud(GUI *gui, ResourceLoader *resources)
{
	this->gui = gui;
	this->resources = resources;

	// create debug info
	SDL_Color c = {255, 255, 255};
	Texture *t = new Texture("TEST!", resources->getFont(ResourceLoader::fontType::fnt_small), c);
	debug_id = gui->addText(t, 1, GUIObject::Alignment::upleft, 0.1f, 0.1f);

	// create health info
	health_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiHealth), 1, GUIObject::Alignment::downleft, 20.f, -148.f);
	t = new Texture("100", resources->getFont(ResourceLoader::fontType::fnt_mid), c);
	health_txt_id = gui->addText(t, 1, GUIObject::Alignment::downleft, 60.f, -93.f);

	// create ammo info
	ammo_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiAmmo), 1, GUIObject::Alignment::downright, -148.f, -148.f);
	t = new Texture("50", resources->getFont(ResourceLoader::fontType::fnt_mid), c);
	ammo_txt_id = gui->addText(t, 1, GUIObject::Alignment::downright, -103.f, -93.f);
	t = new Texture("50", resources->getFont(ResourceLoader::fontType::fnt_mids), c);
	ammo_mag_txt_id = gui->addText(t, 1, GUIObject::Alignment::downright, -71.f, -86.f);

	// create crosshair
	crosshair_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiCrosshair), 1, GUIObject::Alignment::center, 0.0f, 0.0f);
	gui->setCentered(crosshair_id, true);

	// create game status message indicator
	t = new Texture("Status goes here!", resources->getFont(ResourceLoader::fontType::fnt_mids), c);
	status_id = gui->addText(t, 1, GUIObject::Alignment::upcenter, 0.f, 30.f);
	gui->setCentered(status_id, true);

	t = new Texture("Animated message goes here!", resources->getFont(ResourceLoader::fontType::fnt_mid), c);
	message_id = gui->addText(t, 1, GUIObject::Alignment::downcenter, 0.f, -100.f);
	gui->setCentered(message_id, true);

	t = new Texture("Wave: 0 \nPoints: 2", resources->getFont(ResourceLoader::fontType::fnt_mid), c);
	wave_points_id = gui->addText(t, 1, GUIObject::Alignment::upright, -150.f, 10.f);
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