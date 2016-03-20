#include <gui_serverlist.h>
#include <string>

#define GUI_GROUP_SERVERLIST 12

bool clicked_connect = false;

class connectCallback : public GUICallback {

public:
	virtual void callback(int obj_id);
};

void connectCallback::callback(int obj_id)
{
	clicked_connect = true;
}




gui_serverlist::gui_serverlist(GUI* gui, ResourceLoader *resources, GUICallback *playMpCb, GUICallback *closeSvlistCb)
{
	this->gui = gui;
	this->resources = resources;
	this->playMpCb = playMpCb;

	connectCallback *cb_con = new connectCallback();

	black_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::BlackBg), 3, GUIObject::Alignment::scaled, 0.0f, 0.0f, GUI_GROUP_SERVERLIST);
	gui->setAlpha(black_bg_id, 0.6f);
	gui->setScaleX(black_bg_id, 1920.f / 640.f);
	gui->setScaleY(black_bg_id, 1080.f / 480.f);

	bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::SvlistBg), 4, GUIObject::Alignment::center, -400.0f, -300.0f, GUI_GROUP_SERVERLIST);

	txt_placeholder_head = gui->addText("Work in progress!", resources->getFont(ResourceLoader::fontType::fnt_mid), 5, GUIObject::Alignment::center, -110.f, -250.f, GUI_GROUP_SERVERLIST);
	std::string txt = { "A server list will be displayed here in a later version.\n\n"
		"For now, you can connect to an official server, located in Europe." };
	txt_placeholder = gui->addText(txt, resources->getFont(ResourceLoader::fontType::fnt_mids), 5, GUIObject::Alignment::center, -285.f, -100.f, GUI_GROUP_SERVERLIST);

	but_connect = gui->addButton(resources->getTex(ResourceLoader::texType::Button), resources->getTex(ResourceLoader::texType::ButtonSel), 5, GUIObject::Alignment::center, -64.0f, 100.f, cb_con, GUI_GROUP_SERVERLIST);
	gui->addText("Connect", resources->getFont(ResourceLoader::fontType::fnt_mids), 6, GUIObject::Alignment::center, -64.0f + 28.0f, 100.f + 2.0f, GUI_GROUP_SERVERLIST);

	but_close = gui->addButton(resources->getTex(ResourceLoader::texType::Button), resources->getTex(ResourceLoader::texType::ButtonSel), 5, GUIObject::Alignment::center, -64.0f, -300.0f + 540.f, closeSvlistCb, GUI_GROUP_SERVERLIST);
	gui->addText("Close", resources->getFont(ResourceLoader::fontType::fnt_mids), 6, GUIObject::Alignment::center, -64.0f + 38.0f, -300.0f + 540.f + 2.0f, GUI_GROUP_SERVERLIST);

	gui->setButtonSwitchDown(but_connect, but_close);
	gui->setButtonSwitchUp(but_close, but_connect);

	hide();
}

gui_serverlist::~gui_serverlist()
{
	/* not used currently
	gui->removeObject(bg_id);
	gui->removeObject(black_bg_id);
	gui->removeObject(txt_placeholder);
	gui->removeObject(but_connect);*/
}

void gui_serverlist::show()
{
	gui->setGroupVisible(GUI_GROUP_SERVERLIST, true);

	gui->setButtonDefault(but_connect);
}

void gui_serverlist::hide()
{
	gui->setGroupVisible(GUI_GROUP_SERVERLIST, false);
	gui->setVisible(black_bg_id, false);
}

void gui_serverlist::frame(double time_delta)
{
	if (clicked_connect)
	{
		clicked_connect = false;
		hide();

		// TODO: set here what you want
		conn_ip.assign("192.168.0.36");
		conn_port = 1201;

		playMpCb->callback(but_connect);
	}
}