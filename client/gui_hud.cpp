#include "gui_hud.h"

#include <sstream>

gui_hud::gui_hud(GUI *gui, ResourceLoader *resources)
{
	this->gui = gui;
	this->resources = resources;

	// create debug info
	debug_id = gui->addText("TEST!", resources->getFont(ResourceLoader::fontType::fnt_small), 1, GUIObject::Alignment::upleft, 0.1f, 0.1f);

	// create health info
	health_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiHealth), 1, GUIObject::Alignment::downleft, 20.f, -148.f);
	health_txt_id = gui->addText("100", resources->getFont(ResourceLoader::fontType::fnt_mid), 1, GUIObject::Alignment::downleft, 60.f, -93.f);

	// create ammo info
	ammo_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiAmmo), 1, GUIObject::Alignment::downright, -148.f, -148.f);
	ammo_txt_id = gui->addText("50", resources->getFont(ResourceLoader::fontType::fnt_mid), 1, GUIObject::Alignment::downright, -103.f, -93.f);
	ammo_mag_txt_id = gui->addText("10", resources->getFont(ResourceLoader::fontType::fnt_mids), 1, GUIObject::Alignment::downright, -71.f, -86.f);

	// create crosshair
	crosshair_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiCrosshair), 1, GUIObject::Alignment::center, 0.0f, 0.0f);
	gui->setCentered(crosshair_id, true);

	// create game status message indicator
	status_id = gui->addText("Status goes here!", resources->getFont(ResourceLoader::fontType::fnt_mids), 1, GUIObject::Alignment::upcenter, 0.f, 30.f);
	gui->setCentered(status_id, true);

	message_id = gui->addText("Animated message goes here!", resources->getFont(ResourceLoader::fontType::fnt_mid), 1, GUIObject::Alignment::downcenter, 0.f, -100.f);
	gui->setCentered(message_id, true);

	wave_points_id = gui->addText("Wave: 0 \nPoints: 2", resources->getFont(ResourceLoader::fontType::fnt_mid), 1, GUIObject::Alignment::upright, -150.f, 10.f);
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
	// Debug::
	std::ostringstream s;

	s << "Health: " << health << ", Ammo: " << ", Magazin: " << magazin << ", Wave: " << wave
		<< ", Wave: " << ", Points: " << points;

	gui->updateText(debug_id, s.str().c_str());
	
	// Update Fight GUI
	s.str(""); s << health;
	gui->updateText(health_txt_id, s.str().c_str());
	
	s.str(""); s << ammo;
	gui->updateText(ammo_txt_id, s.str().c_str());
	
	s.str(""); s << magazin;
	gui->updateText(ammo_mag_txt_id, s.str().c_str());
	
	s.str(""); s << "Wave: " << wave << "\nPoints: " << points;
	gui->updateText(wave_points_id, s.str().c_str());
}
