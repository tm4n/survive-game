#include "Menu.h"
#include "backends/b_settings.h"
#include "ResourceLoader.h"

#include <iostream>
#include <sstream>

bool clicked_options = false;

// Callback classes
class optionsCallback : public GUICallback {

	public:
		optionsCallback(Menu *am);
		virtual void callback(int obj_id);
		Menu *m;

};

optionsCallback::optionsCallback(Menu *am) : GUICallback(), m(am) {}

void optionsCallback::callback(int obj_id)
{
	clicked_options = true;

	m->snd_click();
}

//////////////////////////
// Options Callback classes
class optionsOkCallback : public GUICallback {

	public:
		optionsOkCallback(Menu *am);
		virtual void callback(int obj_id);
		Menu *m;

};

optionsOkCallback::optionsOkCallback(Menu *am) : GUICallback(), m(am) {}

void optionsOkCallback::callback(int obj_id)
{
	m->options_hide();
	
	// safe options
	b_settings *set = b_settings::instance();

	// set to options
	m->options_safe_to_settings();

	set->save_settings();
	
	m->snd_click();
}

class optionsCancelCallback : public GUICallback {

	public:
		optionsCancelCallback(Menu *am);
		virtual void callback(int obj_id);
		Menu *m;

};

optionsCancelCallback::optionsCancelCallback(Menu *am) : GUICallback(), m(am) {}

void optionsCancelCallback::callback(int obj_id)
{
	m->options_hide();
	
	m->snd_click();
}

class optionsInputCallback : public GUICallback{
public:
	optionsInputCallback(Menu *am);
	virtual void callback(int obj_id);
	Menu *m;
};
optionsInputCallback::optionsInputCallback(Menu *am) : GUICallback(), m(am) {}
void optionsInputCallback::callback(int obj_id)
{
	m->enable_inputbox(obj_id);
	m->options_update();
	m->snd_click();
}

class optionsMouseinvCallback: public GUICallback{
public:
	optionsMouseinvCallback(Menu *am);
	virtual void callback(int obj_id);
	Menu *m;
};
optionsMouseinvCallback::optionsMouseinvCallback(Menu *am) : GUICallback(), m(am) {}
void optionsMouseinvCallback::callback(int obj_id)
{
	m->mouseinv = !m->mouseinv;
	m->options_update();
	m->snd_click();
}

class optionsResLeftCallback : public GUICallback{
public:
	optionsResLeftCallback(Menu *am);
	virtual void callback(int obj_id);
	Menu *m;
};
optionsResLeftCallback::optionsResLeftCallback(Menu *am) : GUICallback(), m(am) {}
void optionsResLeftCallback::callback(int obj_id)
{
	if (m->screenres_index < 0) m->screenres_index = 0;
	else
	{
		if (m->screenres_index <= 0) m->screenres_index = OPT_NUM_SCREENRESOLUTIONS - 1;
		else m->screenres_index--;
	}
	m->options_update();
	m->snd_click();
}

class optionsResRightCallback : public GUICallback{
public:
	optionsResRightCallback(Menu *am);
	virtual void callback(int obj_id);
	Menu *m;
};
optionsResRightCallback::optionsResRightCallback(Menu *am) : GUICallback(), m(am) {}
void optionsResRightCallback::callback(int obj_id)
{
	if (m->screenres_index < 0) m->screenres_index = 0;
	else
	{
		if (m->screenres_index >= OPT_NUM_SCREENRESOLUTIONS - 1) m->screenres_index = 0;
		else m->screenres_index++;
	}
	m->options_update();
	m->snd_click();
}

class optionsFullscreenCallback : public GUICallback{
public:
	optionsFullscreenCallback(Menu *am);
	virtual void callback(int obj_id);
	Menu *m;
};
optionsFullscreenCallback::optionsFullscreenCallback(Menu *am) : GUICallback(), m(am) {}
void optionsFullscreenCallback::callback(int obj_id)
{
	m->fullscreen = !m->fullscreen;
	m->options_update();
	m->snd_click();
}

class optionsShaderCallback : public GUICallback{
public:
	optionsShaderCallback(Menu *am);
	virtual void callback(int obj_id);
	Menu *m;
};
optionsShaderCallback::optionsShaderCallback(Menu *am) : GUICallback(), m(am) {}
void optionsShaderCallback::callback(int obj_id)
{
	m->shader = !m->shader;
	m->options_update();
	m->snd_click();
}

class optionsAntialiasCallback : public GUICallback{
public:
	optionsAntialiasCallback(Menu *am);
	virtual void callback(int obj_id);
	Menu *m;
};
optionsAntialiasCallback::optionsAntialiasCallback(Menu *am) : GUICallback(), m(am) {}
void optionsAntialiasCallback::callback(int obj_id)
{
	m->antialias = !m->antialias;
	m->options_update();
	m->snd_click();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Menu class

const int Menu::screenresolutions_x[] = { 800, 1024, 1024, 1152, 1280, 1280, 1280, 1360, 1366, 1400, 1440, 1600, 1680, 1920, 1920, 2560, 3840, 4096 };
const int Menu::screenresolutions_y[] = { 600, 600,  768,  864,  720,  800,  1024, 768,  768,  1050, 900,  1200, 1050, 1080, 1200, 1440, 2160, 2304 };

Menu::Menu(GUI *agui, ResourceLoader *aresources, GUICallback *playCb, GUICallback *quitCb)
{
	gui = agui;
	resources = aresources;
	
	current_inputbox = -1;
	current_inputstring = NULL;

	optionsCallback *optCb = new optionsCallback(this);
	optionsOkCallback *optOkCb = new optionsOkCallback(this);
	optionsCancelCallback *optCancelCb = new optionsCancelCallback(this);
	optionsInputCallback *optInputCb = new optionsInputCallback(this);

	// add menu background
	bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::MenuBackground), 1, GUIObject::Alignment::scaled, 0.0f, 0.0f);

	// add buttons
	button_ids[0] = gui->addButton(resources->getTex(ResourceLoader::texType::MenuPlay), resources->getTex(ResourceLoader::texType::MenuPlaySel), 2, GUIObject::Alignment::scaled, 0.653f, 0.20f, playCb);
	button_ids[1] = gui->addButton(resources->getTex(ResourceLoader::texType::MenuMultiplayer), resources->getTex(ResourceLoader::texType::MenuMultiplayerSel), 2, GUIObject::Alignment::scaled, 0.655f, 0.34f, NULL);
	button_ids[2] = gui->addButton(resources->getTex(ResourceLoader::texType::MenuHelp), resources->getTex(ResourceLoader::texType::MenuHelpSel), 2, GUIObject::Alignment::scaled, 0.651f, 0.49f, NULL);
	button_ids[3] = gui->addButton(resources->getTex(ResourceLoader::texType::MenuOptions), resources->getTex(ResourceLoader::texType::MenuOptionsSel), 2, GUIObject::Alignment::scaled, 0.653f, 0.645f, optCb);
	button_ids[4] = gui->addButton(resources->getTex(ResourceLoader::texType::MenuQuit), resources->getTex(ResourceLoader::texType::MenuQuitSel), 2, GUIObject::Alignment::scaled, 0.653f, 0.766f, quitCb);
	
	gui->setButtonSwitchDown(button_ids[0], button_ids[1]);
	gui->setButtonSwitchDown(button_ids[1], button_ids[2]);
	gui->setButtonSwitchUp(button_ids[1], button_ids[0]);
	gui->setButtonSwitchDown(button_ids[2], button_ids[3]);
	gui->setButtonSwitchUp(button_ids[2], button_ids[1]);
	gui->setButtonSwitchDown(button_ids[3], button_ids[4]);
	gui->setButtonSwitchUp(button_ids[3], button_ids[2]);
	gui->setButtonSwitchUp(button_ids[4], button_ids[3]); 

	// add options menu
	black_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::BlackBg), 3, GUIObject::Alignment::scaled, 0.0f, 0.0f, 5);
	gui->setAlpha(black_bg_id, 0.6f);
	gui->setScaleX(black_bg_id, 1920.f/640.f);
	gui->setScaleY(black_bg_id, 1080.f/480.f);

	st_bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::OptionsBg), 4, GUIObject::Alignment::center, -375.0f, -275.0f, 5);

	st_cancel_bt = gui->addButton(resources->getTex(ResourceLoader::texType::Button), resources->getTex(ResourceLoader::texType::ButtonSel), 5, GUIObject::Alignment::center, -375.0f + 186.0f, -275.0f + 450.f, optCancelCb, 5);
	st_cancel_txt = gui->addText("Discard", resources->getFont(ResourceLoader::fontType::fnt_mids), 6, GUIObject::Alignment::center, -375.0f + 186.0f + 28.0f, -275.0f + 450.f + 2.0f, 5);

	st_ok_bt = gui->addButton(resources->getTex(ResourceLoader::texType::Button), resources->getTex(ResourceLoader::texType::ButtonSel), 5, GUIObject::Alignment::center, -375.0f+436.0f, -275.0f+450.f, optOkCb, 5);
	st_ok_txt = gui->addText("Apply", resources->getFont(ResourceLoader::fontType::fnt_mids), 6, GUIObject::Alignment::center, -375.0f + 436.0f + 37.0f, -275.0f + 450.f + 2.0f, 5);

	st_playername_head = gui->addText("Player Name:", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -375.0f + 30.0f, -275.0f + 45.0f, 5);
	st_playername_bt = gui->addButton(resources->getTex(ResourceLoader::texType::Inputbox), resources->getTex(ResourceLoader::texType::InputBoxSel), 5, GUIObject::Alignment::center, -375.0f + 30.0f, -275.0f + 70.f, optInputCb, 5);
	st_playername_txt = gui->addText("Playa", resources->getFont(ResourceLoader::fontType::fnt_mids), 6, GUIObject::Alignment::center, -375.0f + 40.0f, -275.0f + 72.0f, 5);

	st_mousesens_head = gui->addText("Mouse Sensitivity: (default 2.0)", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -375.0f + 30.0f, -275.0f + 145.0f, 5);
	st_mousesens_bt = gui->addButton(resources->getTex(ResourceLoader::texType::Inputbox), resources->getTex(ResourceLoader::texType::InputBoxSel), 5, GUIObject::Alignment::center, -375.0f + 30.0f, -275.0f + 170.f, optInputCb, 5);
	st_mousesens_txt = gui->addText("2.0", resources->getFont(ResourceLoader::fontType::fnt_mids), 6, GUIObject::Alignment::center, -375.0f + 40.0f, -275.0f + 172.0f, 5);

	optionsMouseinvCallback *micb = new optionsMouseinvCallback(this);
	st_mouseinv_bt = gui->addButton(resources->getTex(ResourceLoader::texType::CheckboxOff), resources->getTex(ResourceLoader::texType::CheckboxOffSel), 5, GUIObject::Alignment::center, -375.0f + 30.0f, -275.0f + 230.f, micb, 5);
	st_mouseinv_txt = gui->addText("Invert Mouse", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -375.0f + 58.0f, -275.0f + 226.0f, 5);

	st_volume_head = gui->addText("Sound Volume: (0-100)", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -375.0f + 30.0f, -275.0f + 315.0f, 5);
	st_volume_bt = gui->addButton(resources->getTex(ResourceLoader::texType::Inputbox), resources->getTex(ResourceLoader::texType::InputBoxSel), 5, GUIObject::Alignment::center, -375.0f + 30.0f, -275.0f + 340.f, optInputCb, 5);
	st_volume_txt = gui->addText("100", resources->getFont(ResourceLoader::fontType::fnt_mids), 6, GUIObject::Alignment::center, -375.0f + 40.0f, -275.0f + 342.0f, 5);

	st_res_head = gui->addText("Resolution:", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -375.0f + 480.0f, -275.0f + 45.0f, 5);
	optionsResLeftCallback* rlcb = new optionsResLeftCallback(this);
	st_res_bt_left = gui->addButton(resources->getTex(ResourceLoader::texType::ButtonLeft), resources->getTex(ResourceLoader::texType::ButtonLeftSel), 5, GUIObject::Alignment::center, -375.0f + 480.0f, -275.0f + 70.f, rlcb, 5);
	optionsResRightCallback *rrcb = new optionsResRightCallback(this);
	st_res_bt_right = gui->addButton(resources->getTex(ResourceLoader::texType::ButtonRight), resources->getTex(ResourceLoader::texType::ButtonRightSel), 5, GUIObject::Alignment::center, -375.0f + 650.0f, -275.0f + 70.f, rrcb, 5);
	st_res_txt = gui->addText("1980 x 1080", resources->getFont(ResourceLoader::fontType::fnt_mids), 6, GUIObject::Alignment::center, -375.0f + 577.0f, -275.0f + 85.0f, 5);
	gui->setCentered(st_res_txt, true);

	optionsFullscreenCallback *fucb = new optionsFullscreenCallback(this);
	st_fullscreen_bt = gui->addButton(resources->getTex(ResourceLoader::texType::CheckboxOff), resources->getTex(ResourceLoader::texType::CheckboxOffSel), 5, GUIObject::Alignment::center, -375.0f + 480.0f, -275.0f + 130.f, fucb, 5);
	st_fullscreen_txt = gui->addText("Fullscreen", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -375.0f + 508.0f, -275.0f + 126.0f, 5);

	//st_shadows_bt, st_shadows_txt; //TODO: implement shadows

	optionsShaderCallback *shcb = new optionsShaderCallback(this);
	st_shader_bt = gui->addButton(resources->getTex(ResourceLoader::texType::CheckboxOff), resources->getTex(ResourceLoader::texType::CheckboxOffSel), 5, GUIObject::Alignment::center, -375.0f + 480.0f, -275.0f + 190.f, shcb, 5);
	st_shader_txt = gui->addText("Shader Effects", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -375.0f + 508.0f, -275.0f + 186.0f, 5);
	
	optionsAntialiasCallback *ancb = new optionsAntialiasCallback(this);
	st_antialias_bt = gui->addButton(resources->getTex(ResourceLoader::texType::CheckboxOff), resources->getTex(ResourceLoader::texType::CheckboxOffSel), 5, GUIObject::Alignment::center, -375.0f + 480.0f, -275.0f + 230.f, ancb, 5);
	st_antialias_txt = gui->addText("Antialiasing", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -375.0f + 508.0f, -275.0f + 226.0f, 5);

	gui->addText("Changing graphic settings\nrequires a restart", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -375.0f + 480.0f, -275.0f + 326.0f, 5);

	// hide options menu
	options_hide();

	// settings buttons

	// Add version text
	SDL_Color c = {255, 255, 255};
	Texture *tex_version = new Texture(DEF_NAME_STR, resources->getFont(ResourceLoader::fontType::fnt_small), c);
	gui->addText(tex_version, 3, GUIObject::Alignment::downleft, 40.f, -25.f);
	
	show();
}


void Menu::show()
{
	visible = true;

	gui->setVisible(bg_id, true);

	for (int i = 0; i < SELECTION_MAX; i++)
	{
		gui->setVisible(button_ids[i], true);
	}

	// untrap mouse
	SDL_SetRelativeMouseMode(SDL_FALSE);

	gui->setButtonDefault(button_ids[0]);
}

void Menu::hide()
{
	visible = false;

	gui->setVisible(bg_id, false);

	for (int i = 0; i < SELECTION_MAX; i++)
	{
		gui->setVisible(button_ids[i], false);
	}
			
	// trap mouse
	SDL_SetRelativeMouseMode(SDL_TRUE);

	gui->setButtonDefault(-1);
}


void Menu::frame(double time_frame)
{
	if (clicked_options == true)
	{
		clicked_options = false;

		options_show();
	}

	if (current_inputbox >= 0)
	{
		// caret navigation (text cursor)
		caret_animation += time_frame;
		if (caret_animation > 8.f)
		{
			caret_animation -= 8.f;

			if (caret_visible) current_inputstring->pop_back();
			else current_inputstring->push_back('|');
			caret_visible = !caret_visible;

			options_update();
		}
	}
}

void Menu::enable_inputbox(int id)
{
	if (current_inputbox >= 0) disable_inputbox();
	SDL_StartTextInput();

	if (id == st_playername_bt)
	{
		current_inputbox = id;
		current_inputstring = &str_playername;
	}
	if (id == st_mousesens_bt)
	{
		current_inputbox = id;
		current_inputstring = &str_mousesens;
	}
	if (id == st_volume_bt)
	{
		current_inputbox = id;
		current_inputstring = &str_volume;
	}

	if (current_inputbox >= 0)
	{
		current_inputstring->push_back('|');
		caret_visible = true;
	}
}

void Menu::disable_inputbox()
{
	if (current_inputbox < 0) return;
	// remove blinking cursor
	if (caret_visible) current_inputstring->pop_back();
	caret_visible = false;

	current_inputbox = -1;
	current_inputstring = NULL;
	SDL_StopTextInput();
}

void Menu::snd_click()
{
	resources->getSnd(ResourceLoader::sndType::Click)->play(1, 100.f);
}

void Menu::event_input_keys(SDL_Event *evt)
{
	if (evt->type == SDL_TEXTINPUT)
	{
		if (current_inputbox >= 0)
		{
			if (caret_visible) current_inputstring->pop_back();

			if (current_inputbox == st_playername_bt) current_inputstring->append(evt->text.text);
			else
			{
				// sanatize input
				char first = evt->text.text[0];
				std::string str_first(1, first);
				if (first >= '0' && first <= '9') current_inputstring->append(str_first);
				if (first == '.' && current_inputstring->find('.') == std::string::npos) current_inputstring->append(str_first);
			}
			if (caret_visible) current_inputstring->push_back('|');
			options_update();
		}
	}
	if (evt->type == SDL_TEXTEDITING)
	{
		if (current_inputbox >= 0)
		{
			// TODO: does this really work? no, it deletes the text when out of focus
			//current_inputstring->assign(evt->edit.text);
			options_update();
		}
		/*composition = event.edit.text;
		cursor = event.edit.start;
		selection_len = event.edit.length;*/
	}
	if (evt->type == SDL_KEYDOWN)
	{
		if (evt->key.keysym.sym == SDLK_BACKSPACE)
		{
			if (current_inputbox >= 0)
			{
				if (caret_visible) current_inputstring->pop_back();

				if (current_inputstring->length() > 0)
				{
					current_inputstring->pop_back();
					options_update();
				}

				if (caret_visible) current_inputstring->push_back('|');
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////
// options menu


void Menu::options_update()
{
	gui->updateText(st_playername_txt, str_playername);
	gui->updateText(st_mousesens_txt, str_mousesens);

	if (mouseinv) gui->setButtonTextures(st_mouseinv_bt, resources->getTex(ResourceLoader::texType::CheckboxOn), resources->getTex(ResourceLoader::texType::CheckboxOnSel));
	else gui->setButtonTextures(st_mouseinv_bt, resources->getTex(ResourceLoader::texType::CheckboxOff), resources->getTex(ResourceLoader::texType::CheckboxOffSel));

	gui->updateText(st_volume_txt, str_volume);

	if (screenres_index >= 0)
	{
		std::ostringstream bf;
		bf << screenresolutions_x[screenres_index] << " x " << screenresolutions_y[screenres_index];
		gui->updateText(st_res_txt, bf.str());
	}
	else gui->updateText(st_res_txt, "Custom");

	if (fullscreen) gui->setButtonTextures(st_fullscreen_bt, resources->getTex(ResourceLoader::texType::CheckboxOn), resources->getTex(ResourceLoader::texType::CheckboxOnSel));
	else gui->setButtonTextures(st_fullscreen_bt, resources->getTex(ResourceLoader::texType::CheckboxOff), resources->getTex(ResourceLoader::texType::CheckboxOffSel));
	
	//TODO: shadow

	if (shader) gui->setButtonTextures(st_shader_bt, resources->getTex(ResourceLoader::texType::CheckboxOn), resources->getTex(ResourceLoader::texType::CheckboxOnSel));
	else gui->setButtonTextures(st_shader_bt, resources->getTex(ResourceLoader::texType::CheckboxOff), resources->getTex(ResourceLoader::texType::CheckboxOffSel));

	if (antialias) gui->setButtonTextures(st_antialias_bt, resources->getTex(ResourceLoader::texType::CheckboxOn), resources->getTex(ResourceLoader::texType::CheckboxOnSel));
	else gui->setButtonTextures(st_antialias_bt, resources->getTex(ResourceLoader::texType::CheckboxOff), resources->getTex(ResourceLoader::texType::CheckboxOffSel));

}

void Menu::options_safe_to_settings()
{
	b_settings *set = b_settings::instance();

	set->playername.assign(str_playername);
	try
	{
		set->mouse_sensitivity = std::stof(str_mousesens);
	}
	catch (std::exception e) {}

	set->mouse_invert = mouseinv;

	try
	{
		set->sound_volume = std::stof(str_volume);
	}
	catch (std::exception e) {}

	if (screenres_index >= 0)
	{
		set->screenres_x = screenresolutions_x[screenres_index];
		set->screenres_y = screenresolutions_y[screenres_index];
		set->screenaspect = ((float)set->screenres_x) / ((float)set->screenres_y);
	}

	set->fullscreen = fullscreen;
	set->shader = shader;
	//set->shadow
	if (antialias) set->antialias = 1; else set->antialias = 0;

}

void Menu::options_load_from_settings()
{
	b_settings *set = b_settings::instance();

	str_playername.assign(set->playername);
	str_mousesens.assign(std::to_string(set->mouse_sensitivity));
	mouseinv = set->mouse_invert;
	str_volume.assign(std::to_string((int)set->sound_volume));

	// load screenres
	screenres_index = -1;
	for (int i = 0; i < OPT_NUM_SCREENRESOLUTIONS; i++)
	{
		if (set->screenres_x == screenresolutions_x[i] && set->screenres_y == screenresolutions_y[i])
		{
			screenres_index = i;
			break;
		}
	}

	fullscreen = set->fullscreen;
	shader = set->shader;
	antialias = (set->antialias == 1);

}

void Menu::options_show()
{
	// hide buttons below
	for (int i = 0; i < SELECTION_MAX; i++)
	{
		gui->setVisible(button_ids[i], false);
	}

	// update menu entries from settings backend
	options_load_from_settings();
	options_update();

	// show options menu 
	gui->setGroupVisible(5, true);
}

void Menu::options_hide()
{
	// disable still active input boxes
	disable_inputbox();

	// hide options menu 
	gui->setGroupVisible(5, false);

	// show buttons below
	for (int i = 0; i < SELECTION_MAX; i++)
	{
		gui->setVisible(button_ids[i], true);
	}
}
