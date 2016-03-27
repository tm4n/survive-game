#include "gui_hud.h"

#include "net_cl.h"
#include "helper.h"

#include <sstream>

// callbacks classes
class closeCallback : public GUICallback {
	public:
		closeCallback(gui_hud *hud);
		virtual void callback(int obj_id);
		gui_hud *hud;
};

closeCallback::closeCallback(gui_hud *ahud) : GUICallback() {hud = ahud;}

void closeCallback::callback(int obj_id)
{
	hud->hide_ingame_menu();
}

class disconCallback : public GUICallback {
	public:
		disconCallback(bool *quit);
		virtual void callback(int obj_id);
		bool *quit;
};

disconCallback::disconCallback(bool *aquit) : GUICallback() {quit = aquit;}

void disconCallback::callback(int obj_id)
{
	*quit = true;
}

///////////////////////////////////////////////////////////////////////////

gui_hud::gui_hud(GUI *gui, ResourceLoader *resources, net_cl *net_client, bool *quit)
{
	this->gui = gui;
	this->resources = resources;
	this->net_client = net_client;
	this->quit = quit;
	this->scoreboard_visible = false;
	this->ingame_menu_visible = false;
	this->chat_active = false;
	this->chat_carvis = false;
	this->scoreboard_timer = 0.f;
	this->msg_timer = 1200.f;
	this->chat_caranim = 0.;

	// create debug info
	//debug_id = gui->addText("TEST!", resources->getFont(ResourceLoader::fontType::fnt_small), 1, GUIObject::Alignment::upleft, 0.1f, 0.1f);

	// create health info
#ifndef ANDROID
	health_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiHealth), 1, GUIObject::Alignment::downleft, 20.f, -148.f);
	health_txt_id = gui->addText("100", resources->getFont(ResourceLoader::fontType::fnt_mid), 1, GUIObject::Alignment::downleft, 60.f, -93.f);
#else
	health_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiHealth), 1, GUIObject::Alignment::downleft, 120.f, -210.f);
	gui->setScaleX(health_bg_id, 1.2f); gui->setScaleY(health_bg_id, 1.2f);
	health_txt_id = gui->addText("100", resources->getFont(ResourceLoader::fontType::fnt_large), 1, GUIObject::Alignment::downleft, 170.f, -150.f);
#endif

	// create ammo info
#ifndef ANDROID
	ammo_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiAmmo), 1, GUIObject::Alignment::downright, -148.f, -148.f);
	ammo_mag_txt_id = gui->addText("50", resources->getFont(ResourceLoader::fontType::fnt_mid), 1, GUIObject::Alignment::downright, -103.f, -93.f);
	ammo_txt_id = gui->addText("10", resources->getFont(ResourceLoader::fontType::fnt_mids), 1, GUIObject::Alignment::downright, -71.f, -86.f);
#else
	ammo_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiAmmo), 1, GUIObject::Alignment::downright, -260.f, -210.f);
	gui->setScaleX(ammo_bg_id, 1.2f); gui->setScaleY(ammo_bg_id, 1.2f);
	ammo_mag_txt_id = gui->addText("50", resources->getFont(ResourceLoader::fontType::fnt_large), 1, GUIObject::Alignment::downright, -220.f, -160.f);
	ammo_txt_id = gui->addText("10", resources->getFont(ResourceLoader::fontType::fnt_mid), 1, GUIObject::Alignment::downright, -170.f, -148.f);
#endif
	// create crosshair
	crosshair_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiCrosshair), 1, GUIObject::Alignment::center, 0.0f, 0.0f);
	gui->setCentered(crosshair_id, true);

	// create game status message indicator
#ifndef ANDROID
	status_id = gui->addText("Status goes here!", resources->getFont(ResourceLoader::fontType::fnt_mids), 1, GUIObject::Alignment::upcenter, 0.f, 80.f);
#else
	status_id = gui->addText("Status goes here!", resources->getFont(ResourceLoader::fontType::fnt_larges), 1, GUIObject::Alignment::upcenter, 0.f, 140.f);
#endif
	gui->setCentered(status_id, true);

#ifndef ANDROID
	message_id = gui->addText("Animated message goes here!", resources->getFont(ResourceLoader::fontType::fnt_mid), 1, GUIObject::Alignment::downcenter, 0.f, -100.f);
#else
	message_id = gui->addText("Animated message goes here!", resources->getFont(ResourceLoader::fontType::fnt_large), 1, GUIObject::Alignment::downcenter, 0.f, -130.f);
#endif
	gui->setCentered(message_id, true);
	gui->setVisible(message_id, false);

#ifndef ANDROID
	wave_points_id = gui->addText("Wave: 0 \nPoints: 2", resources->getFont(ResourceLoader::fontType::fnt_mid), 1, GUIObject::Alignment::upright, -150.f, 10.f);
#else
	wave_points_id = gui->addText("Wave: 0 \nPoints: 2", resources->getFont(ResourceLoader::fontType::fnt_larges), 1, GUIObject::Alignment::upright, -285.f, 70.f);
#endif
	
	// wave timer
#ifndef ANDROID
	wave_timer_txt_id =  gui->addText("seconds until next wave!", resources->getFont(ResourceLoader::fontType::fnt_mids), 1, GUIObject::Alignment::upcenter, 0.f, 30.f);
	wave_timer_id =  gui->addText("999", resources->getFont(ResourceLoader::fontType::fnt_mids), 1, GUIObject::Alignment::upcenter, -135.f, 30.f, 200, 0, 0);
#else
	wave_timer_txt_id = gui->addText("seconds until next wave!", resources->getFont(ResourceLoader::fontType::fnt_larges), 1, GUIObject::Alignment::upcenter, 0.f, 80.f);
	wave_timer_id = gui->addText("999", resources->getFont(ResourceLoader::fontType::fnt_larges), 1, GUIObject::Alignment::upcenter, -220.f, 80.f, 200, 0, 0);
#endif
	gui->setCentered(wave_timer_id, true);
	gui->setCentered(wave_timer_txt_id, true);

	// highscore:
	score_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiScoreboard), 5, GUIObject::Alignment::center, -225.f, -150.f);
	gui->setAlpha(score_bg_id, 0.8f);

	score_names_txt_id = gui->addText(" ", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -195.f, -140.f);
	score_points_txt_id = gui->addText(" ", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -35.f, -140.f);
	score_pings_txt_id = gui->addText(" ", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, 85.f, -140.f);
	highscore_txt_id = gui->addText(" ", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, 0.f, 125.f);
	gui->setCentered(highscore_txt_id, true);

	// controller help
#ifdef ANDROID
	controller_help_id = gui->addPanel(resources->getTex(ResourceLoader::texType::ControllerHelp), 3, GUIObject::Alignment::center, 0.f, 0.f);
	gui->setCentered(controller_help_id, true);
	gui->setAlpha(controller_help_id, 0.8f);
#endif

	// chat
	chat_txt_id = gui->addText("", resources->getFont(ResourceLoader::fontType::fnt_smallb), 6, GUIObject::Alignment::upleft, 8.f, 8.f);
	chat_input_txt_id = gui->addText("", resources->getFont(ResourceLoader::fontType::fnt_smallb), 6, GUIObject::Alignment::upleft, 8.f, 110.f);

	// ingame menu
#ifdef ANDROID
	float pos_offset = 550.f;
#else
	float pos_offset = 0.f;
#endif
	ingame_menu_bg = gui->addPanel(resources->getTex(ResourceLoader::texType::IngameMenuBg), 3, GUIObject::Alignment::center, -150.f+pos_offset, -225.f);
	gui->setAlpha(ingame_menu_bg, 0.8f);

	SDL_Color c = {255, 255, 255};
	SDL_Color sel = {255, 128, 128};
	disconCallback *dc = new disconCallback(quit);
	Texture *tex = new Texture("Disconnect", resources->getFont(ResourceLoader::fontType::fnt_mid), c);
	Texture *tex_sel = new Texture("Disconnect", resources->getFont(ResourceLoader::fontType::fnt_mid), sel);
	ingame_but_disconnect = gui->addButton(tex, tex_sel, 4, GUIObject::Alignment::center, -125.f+pos_offset, -200.f, dc);
	callbacks.push_back(dc);
	textures.push_back(tex); textures.push_back(tex_sel);

	/*tex = new Texture("Options", resources->getFont(ResourceLoader::fontType::fnt_mid), c);
	tex_sel = new Texture("Options", resources->getFont(ResourceLoader::fontType::fnt_mid), sel);
	ingame_but_options = gui->addButton(tex, tex_sel, 4, GUIObject::Alignment::center, -125.f, -100.f, NULL);
	//callbacks.push_back(dc);
	textures.push_back(tex); textures.push_back(tex_sel);*/

	closeCallback *cc = new closeCallback(this);
	tex = new Texture("Back to Game", resources->getFont(ResourceLoader::fontType::fnt_mid), c);
	tex_sel = new Texture("Back to Game", resources->getFont(ResourceLoader::fontType::fnt_mid), sel);
	ingame_but_close = gui->addButton(tex, tex_sel, 4, GUIObject::Alignment::center, -125.f+pos_offset, 95.f, cc);
	callbacks.push_back(cc);
	textures.push_back(tex); textures.push_back(tex_sel);

	/*gui->setButtonSwitchDown(ingame_but_disconnect, ingame_but_options);
	gui->setButtonSwitchDown(ingame_but_options, ingame_but_close);
	gui->setButtonSwitchUp(ingame_but_options, ingame_but_disconnect);
	gui->setButtonSwitchUp(ingame_but_close, ingame_but_options);*/

	gui->setButtonSwitchDown(ingame_but_disconnect, ingame_but_close);
	gui->setButtonSwitchUp(ingame_but_close, ingame_but_disconnect);


	// hide everything default
	hide_scoreboard();
	set_state(hud_state::hidden);
	show_status_connecting();
	hide_wave_timer();
	hide_ingame_menu();

#ifdef ANDROID
	gui->setVisible(controller_help_id, true);
#endif
}

gui_hud::~gui_hud()
{
	/*set_state(hud_state::hidden);
	hide_ingame_menu();
	hide_scoreboard();
	hide_status();
	hide_wave_timer();*/

	// delete added stuff
	//gui->removeObject(debug_id);
	// delete health info
	gui->removeObject(health_bg_id);
	gui->removeObject(health_txt_id);
	// delete ammo info
	gui->removeObject(ammo_bg_id);
	gui->removeObject(ammo_mag_txt_id);
	gui->removeObject(ammo_txt_id);
	// delete crosshair
	gui->removeObject(crosshair_id);
	// delete game status message indicator
	gui->removeObject(status_id);
	gui->removeObject(message_id);
	gui->removeObject(wave_points_id);
	// delete wave timer
	gui->removeObject(wave_timer_txt_id);
	gui->removeObject(wave_timer_id);
	// delete chat
	gui->removeObject(chat_txt_id);
	gui->removeObject(chat_input_txt_id);

	// delete highscore:
	gui->removeObject(score_bg_id);
	gui->removeObject(score_names_txt_id);
	gui->removeObject(score_points_txt_id);
	gui->removeObject(score_pings_txt_id);
	gui->removeObject(highscore_txt_id);

	// delete ingame menu
	gui->removeObject(ingame_menu_bg);
	gui->removeObject(ingame_but_disconnect);
	//gui->removeObject(ingame_but_options);
	gui->removeObject(ingame_but_close);

#ifdef ANDROID
	gui->removeObject(controller_help_id);
#endif

	for (GUICallback *c : callbacks) delete c;
	for (Texture *t : textures) delete t;
}

void gui_hud::set_state(hud_state new_state)
{
	
	if (new_state == state) return;
	
	// deactivate old state
	// hide everything
	//gui->setVisible(debug_id, false);
	gui->setVisible(health_bg_id, false);
	gui->setVisible(health_txt_id, false);
	gui->setVisible(ammo_bg_id, false);
	gui->setVisible(ammo_txt_id, false);
	gui->setVisible(ammo_mag_txt_id, false);
	gui->setVisible(crosshair_id, false);
	gui->setVisible(wave_points_id, false);
	
	
	if (new_state == hud_state::spectating)
	{
		//gui->setVisible(debug_id, true);
		gui->setVisible(wave_points_id, true);
	}
	if (new_state == hud_state::playing)
	{
		//gui->setVisible(debug_id, true);
		gui->setVisible(health_bg_id, true);
		gui->setVisible(health_txt_id, true);
		gui->setVisible(ammo_bg_id, true);
		gui->setVisible(ammo_txt_id, true);
		gui->setVisible(ammo_mag_txt_id, true);
		gui->setVisible(crosshair_id, true);
		gui->setVisible(wave_points_id, true);
#ifdef ANDROID
		gui->setVisible(controller_help_id, false);
#endif
	}
	
	state = new_state;
}

void gui_hud::set_debug(std::string s)
{
	//gui->updateText(debug_id, s);
}

void gui_hud::scroll_chat(const char *msg)
{
	// move all strings up by one
	chatlines[0].assign(chatlines[1]);
	chatlines[1].assign(chatlines[2]);
	chatlines[2].assign(chatlines[3]);
	chatlines[3].assign(chatlines[4]);
	chatlines[4].assign(msg);
	// display
	std::ostringstream s;
	for (int i = 0; i < 5; i++)
	{
		s << chatlines[i] << '\n';
	}
	gui->updateText(chat_txt_id, s.str());
}

void gui_hud::chat_enable()
{
	chat_inputstring.clear();
	// activate chat
	chat_active = true;
	gui->setVisible(chat_input_txt_id, true);
	chat_input_update();
	// deactivate input
	SDL_StartTextInput();
}
void gui_hud::chat_cancel()
{
	// deactivate input
	SDL_StopTextInput();
	// deactive chat
	chat_active = false;
	gui->setVisible(chat_input_txt_id, false);
	// delete msg
	chat_inputstring.clear();
	chat_carvis = false;
}
void gui_hud::chat_enter()
{
	// deactivate input
	SDL_StopTextInput();
	// deactive chat
	chat_active = false;
	gui->setVisible(chat_input_txt_id, false);
	// send message
	if (chat_carvis) chat_inputstring.pop_back();
	if (chat_inputstring.length() > 0)
	{
		net_client->send_chat(chat_inputstring.c_str(), chat_inputstring.length() + 1, net_client->serverpeer);
	}
	// delete msg
	chat_inputstring.clear();
	chat_carvis = false;
}
void gui_hud::chat_input_update()
{
	gui->updateText(chat_input_txt_id, chat_inputstring);
}

void gui_hud::frame(double time_frame, float health, int ammo, int magazin, int wave, uint points)
{
	std::ostringstream s;
	
	// Update Fight GUI
	if (state == hud_state::playing)
	{
		s.str(""); s << health;
		gui->updateText(health_txt_id, s.str().c_str());
		
		s.str("");
		if (ammo >= 0) s << ammo; else s << " ";
		gui->updateText(ammo_txt_id, s.str().c_str());
		
		s.str(""); s << magazin;
		gui->updateText(ammo_mag_txt_id, s.str().c_str());
	}
	
	s.str(""); s << "Wave: " << wave << "\nPoints: " << points;
	gui->updateText(wave_points_id, s.str().c_str());

	// message management
	if (msg_timer < 1200.f)
	{
		msg_timer += (float)(10.*time_frame);
		gui->setAlpha(message_id, 0.6f + sin(toRadians(msg_timer))*0.4f);
		if (msg_timer >= 1200.f) gui->setVisible(message_id, false);
	}

	// scoreboard management
	if (scoreboard_visible)
	{
		if (scoreboard_timer > 32.f)
		{
			scoreboard_timer = 0.f;
			net_client->send_scoreboard(net_client->serverpeer);
		}
		scoreboard_timer += (float)(time_frame);
	}

	if (chat_active == true)
	{
		// caret navigation (text cursor)
		chat_caranim += time_frame;
		if (chat_caranim > 8.)
		{
			chat_caranim -= 8.;

			if (chat_carvis) chat_inputstring.pop_back();
			else chat_inputstring.push_back('|');
			chat_carvis = !chat_carvis;
			chat_input_update();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// status

void gui_hud::show_status_end()
{
	#ifndef ANDROID
		gui->updateText(status_id, "The Generator has been destroyed. Game over!");
	#else
		gui->updateText(status_id, "The Generator has been destroyed. Game over! Press (A) for menu.");
	#endif // ANDROID
	gui->setVisible(status_id, true);
}

void gui_hud::show_status_join(bool respawn)
{
#ifndef ANDROID
	if (!respawn) gui->updateText(status_id, "You are spectating. Leftclick to join the game.");
	else gui->updateText(status_id, "Leftclick to rejoin the game. You will loose a third of your points.");
#else
	if (!respawn) gui->updateText(status_id, "Press right trigger to start.");
	else gui->updateText(status_id, "Press right trigger to respawn. You will loose a third of your points.");
#endif
	gui->setVisible(status_id, true);
}
void gui_hud::show_status_respawn_timer(int respawn_timer)
{
	std::ostringstream ss;
	ss << "Respawn in " << respawn_timer << " seconds";
	gui->updateText(status_id, ss.str().c_str());
	gui->setVisible(status_id, true);
}
void gui_hud::show_status_connecting()
{
	gui->updateText(status_id, "Connecting to server...");
	gui->setVisible(status_id, true);
}

void gui_hud::show_status_noserver()
{
#ifdef ANDROID
	gui->updateText(status_id, "Could not connect to server or got disconnected.\nPress (A) to exit.");
#else
	gui->updateText(status_id, "Could not connect to server or got disconnected.\nPress Esc to exit.");
#endif
	gui->setVisible(status_id, true);
}

void gui_hud::show_status_wrongver()
{
#ifdef ANDROID
	gui->updateText(status_id, "Your game version is outdated.\nGet the newest version!\nPress (A) to exit.");
#else
	gui->updateText(status_id, "Your game version is outdated.\nGet the newest version!\nPress Esc to exit.");
#endif
	gui->setVisible(status_id, true);
}

void gui_hud::hide_status()
{
	gui->setVisible(status_id, false);
}

/////////////////////////////////////////////////////////////////////////////
// message display
void gui_hud::show_message(const std::string &str)
{
	gui->updateText(message_id, str);
	gui->setVisible(message_id, true);
	msg_timer = 0.f;
}

bool gui_hud::message_visible()
{
	return (msg_timer < 1200.f);
}

/////////////////////////////////////////////////////////////////////////////
// wave timer

void gui_hud::show_wave_timer(int time)
{
	std::ostringstream os ;
    os << time ;;
	std::string str_timer(os.str());

	std::string s;
	if (time < 100) s.assign("  ");
	if (time < 10) s.assign("    ");
	s.append(str_timer);

	hide_status();
	gui->updateText(wave_timer_id, s, 200, 0, 0);
	gui->setVisible(wave_timer_id, true);
	gui->setVisible(wave_timer_txt_id, true);
}

void gui_hud::hide_wave_timer()
{
	gui->setVisible(wave_timer_id, false);
	gui->setVisible(wave_timer_txt_id, false);
}

/////////////////////////////////////////////////////////////////////////////
// scoreboard

void gui_hud::show_scoreboard()
{
	if (scoreboard_visible == true) return;

	scoreboard_visible = true;

	gui->updateText(score_names_txt_id, "Player name");
	gui->updateText(score_points_txt_id, "Points");
	gui->updateText(score_pings_txt_id, "Latency (Ping)");
	gui->updateText(highscore_txt_id, " ");

	gui->setVisible(score_bg_id, true);
	gui->setVisible(score_names_txt_id, true);
	gui->setVisible(score_points_txt_id, true);
	gui->setVisible(score_pings_txt_id, true);
	gui->setVisible(highscore_txt_id, true);

	scoreboard_timer = 9999.f;
}

void gui_hud::update_scoreboard(const char *sdata)
{
	if (scoreboard_visible == true)
	{
		std::vector<std::string> v = split(sdata, '§');

		if ((v.size()-1) % 3 != 0) {log(LOG_ERROR, "ERROR on decoding scoreboard: not multiple of 3 + 1"); return;}

		std::string names("Player name:\n\n");
		std::string points("Points\n\n");
		std::string pings("Latency (Ping)\n\n");

		for (uint i = 0; i < (v.size()-1)/3; i++)
		{
			names.append(v.at(i*3));
			names.append("\n");

			points.append(v.at(i*3+1));
			points.append("\n");

			pings.append(v.at(i*3+2));
			pings.append("\n");
		}

		gui->updateText(score_names_txt_id, names);
		gui->updateText(score_points_txt_id, points);
		gui->updateText(score_pings_txt_id, pings);

		gui->updateText(highscore_txt_id, v.at(v.size()-1));

	}
}

void gui_hud::hide_scoreboard()
{
	scoreboard_visible = false;

	gui->setVisible(score_bg_id, false);
	gui->setVisible(score_names_txt_id, false);
	gui->setVisible(score_points_txt_id, false);
	gui->setVisible(score_pings_txt_id, false);
	gui->setVisible(highscore_txt_id, false);
}


// ingame menu

void gui_hud::toggle_ingame_menu()
{
	if (ingame_menu_visible) hide_ingame_menu(); else show_ingame_menu();
}

void gui_hud::show_ingame_menu()
{
	ingame_menu_visible = true;
	gui->setVisible(ingame_menu_bg, true);
	gui->setVisible(ingame_but_disconnect, true);
	//gui->setVisible(ingame_but_options, true);
	gui->setVisible(ingame_but_close, true);

#ifdef ANDROID
	gui->setX(controller_help_id, -300.f);
	gui->setVisible(controller_help_id, true);
#endif

	// untrap mouse
	SDL_SetRelativeMouseMode(SDL_FALSE);

	gui->setButtonDefault(ingame_but_close);
}

void gui_hud::hide_ingame_menu()
{
	ingame_menu_visible = false;
	gui->setVisible(ingame_menu_bg, false);
	gui->setVisible(ingame_but_disconnect, false);
	//gui->setVisible(ingame_but_options, false);
	gui->setVisible(ingame_but_close, false);

#ifdef ANDROID
	gui->setVisible(controller_help_id, false);
#endif

	// trap mouse
	SDL_SetRelativeMouseMode(SDL_TRUE);

	gui->setButtonDefault(-1);
}
