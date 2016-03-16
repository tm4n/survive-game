#include <gui_serverlist.h>

#define GUI_GROUP_SERVERLIST 12

gui_serverlist::gui_serverlist(GUI* gui, ResourceLoader *resources, GUICallback *playMpCb)
{
	this->gui = gui;
	this->resources = resources;

	black_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::BlackBg), 3, GUIObject::Alignment::scaled, 0.0f, 0.0f, GUI_GROUP_SERVERLIST);
	gui->setAlpha(black_bg_id, 0.6f);
	gui->setScaleX(black_bg_id, 1920.f / 640.f);
	gui->setScaleY(black_bg_id, 1080.f / 480.f);

	txt_placeholder = gui->addText("Placeholdaa!", resources->getFont(ResourceLoader::fontType::fnt_mid), 1, GUIObject::Alignment::center, 60.f, -93.f, GUI_GROUP_SERVERLIST);

	hide();
}

gui_serverlist::~gui_serverlist()
{
	gui->removeObject(bg_id);
	gui->removeObject(black_bg_id);
	gui->removeObject(txt_placeholder);
	gui->removeObject(but_ok);
}

void gui_serverlist::show()
{
	gui->setGroupVisible(GUI_GROUP_SERVERLIST, true);
}

void gui_serverlist::hide()
{
	gui->setGroupVisible(GUI_GROUP_SERVERLIST, false);
	gui->setVisible(black_bg_id, false);
}

void gui_serverlist::frame(double time_delta)
{

}