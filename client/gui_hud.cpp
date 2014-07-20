#include "gui_hud.h"

#include "net_cl.h"
#include "helper.h"

#include <sstream>

gui_hud::gui_hud(GUI *gui, ResourceLoader *resources)
{
	this->gui = gui;
	this->resources = resources;
	this->scoreboard_visible = false;
	this->scoreboard_timer = 0.f;
	this->msg_timer = 1200.f;

	// create debug info
	debug_id = gui->addText("TEST!", resources->getFont(ResourceLoader::fontType::fnt_small), 1, GUIObject::Alignment::upleft, 0.1f, 0.1f);

	// create health info
	health_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiHealth), 1, GUIObject::Alignment::downleft, 20.f, -148.f);
	health_txt_id = gui->addText("100", resources->getFont(ResourceLoader::fontType::fnt_mid), 1, GUIObject::Alignment::downleft, 60.f, -93.f);

	// create ammo info
	ammo_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiAmmo), 1, GUIObject::Alignment::downright, -148.f, -148.f);
	ammo_mag_txt_id = gui->addText("50", resources->getFont(ResourceLoader::fontType::fnt_mid), 1, GUIObject::Alignment::downright, -103.f, -93.f);
	ammo_txt_id = gui->addText("10", resources->getFont(ResourceLoader::fontType::fnt_mids), 1, GUIObject::Alignment::downright, -71.f, -86.f);

	// create crosshair
	crosshair_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiCrosshair), 1, GUIObject::Alignment::center, 0.0f, 0.0f);
	gui->setCentered(crosshair_id, true);

	// create game status message indicator
	status_id = gui->addText("Status goes here!", resources->getFont(ResourceLoader::fontType::fnt_mids), 1, GUIObject::Alignment::upcenter, 0.f, 30.f);
	gui->setCentered(status_id, true);

	message_id = gui->addText("Animated message goes here!", resources->getFont(ResourceLoader::fontType::fnt_mid), 1, GUIObject::Alignment::downcenter, 0.f, -100.f);
	gui->setCentered(message_id, true);
	gui->setVisible(message_id, false);

	wave_points_id = gui->addText("Wave: 0 \nPoints: 2", resources->getFont(ResourceLoader::fontType::fnt_mid), 1, GUIObject::Alignment::upright, -150.f, 10.f);
	
	// wave timer
	wave_timer_txt_id =  gui->addText("seconds until next wave!", resources->getFont(ResourceLoader::fontType::fnt_mids), 1, GUIObject::Alignment::upcenter, 0.f, 30.f);
	gui->setCentered(wave_timer_txt_id, true);
	wave_timer_id =  gui->addText("999", resources->getFont(ResourceLoader::fontType::fnt_mids), 1, GUIObject::Alignment::upcenter, -135.f, 30.f, 200, 0, 0);
	gui->setCentered(wave_timer_id, true);

	// highscore:
	score_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::GuiScoreboard), 5, GUIObject::Alignment::center, -225.f, -150.f);
	gui->setAlpha(score_bg_id, 0.8f);

	score_names_txt_id = gui->addText(" ", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -195.f, -140.f);
	score_points_txt_id = gui->addText(" ", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -35.f, -140.f);
	score_pings_txt_id = gui->addText(" ", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, 85.f, -140.f);
	highscore_txt_id = gui->addText(" ", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, 0.f, 125.f);
	gui->setCentered(highscore_txt_id, true);

	// hide everything default
	hide_scoreboard();
	set_state(hud_state::hidden);
	show_status_connecting();
	hide_wave_timer();
}

gui_hud::~gui_hud()
{
}

void gui_hud::set_state(hud_state new_state)
{
	
	if (new_state == state) return;
	
	// deactivate old state
	// hide everything
	gui->setVisible(debug_id, false);
	gui->setVisible(health_bg_id, false);
	gui->setVisible(health_txt_id, false);
	gui->setVisible(ammo_bg_id, false);
	gui->setVisible(ammo_txt_id, false);
	gui->setVisible(ammo_mag_txt_id, false);
	gui->setVisible(crosshair_id, false);
	gui->setVisible(message_id, false);
	gui->setVisible(wave_points_id, false);
	
	
	if (new_state == hud_state::spectating)
	{
		gui->setVisible(debug_id, true);
		gui->setVisible(message_id, true);
		gui->setVisible(wave_points_id, true);
	}
	if (new_state == hud_state::playing)
	{
		gui->setVisible(debug_id, true);
		gui->setVisible(health_bg_id, true);
		gui->setVisible(health_txt_id, true);
		gui->setVisible(ammo_bg_id, true);
		gui->setVisible(ammo_txt_id, true);
		gui->setVisible(ammo_mag_txt_id, true);
		gui->setVisible(crosshair_id, true);
		gui->setVisible(message_id, true);
		gui->setVisible(wave_points_id, true);
	}
	
	state = new_state;
}

void gui_hud::set_debug(std::string s)
{
	gui->updateText(debug_id, s);
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
}

/////////////////////////////////////////////////////////////////////////////
// status
void gui_hud::show_status_end()
{
	gui->updateText(status_id, "The Generator has been destroyed. Please wait for a new round to start!");
	gui->setVisible(status_id, true);
}

void gui_hud::show_status_join(bool respawn)
{
	if (!respawn) gui->updateText(status_id, "You are spectating. Leftclick to join the game.");
	else gui->updateText(status_id, "Leftclick to rejoin the game. You will loose a third of your points.");
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
	gui->updateText(status_id, "Could not connect to server.");
	gui->setVisible(status_id, true);
}

void gui_hud::show_status_wrongver()
{
	gui->updateText(status_id, "Your game version is outdated.\nGet the newest version from the official homepage!");
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
	std::string str_timer = std::to_string(time);

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