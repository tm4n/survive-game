#include "Menu.h"
#include "backends\b_settings.h"
#include "ResourceLoader.h"

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

// Callback classes
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


Menu::Menu(GUI *agui, ResourceLoader *aresources, GUICallback *playCb, GUICallback *quitCb)
{
	gui = agui;
	resources = aresources;

	optionsCallback *optCb = new optionsCallback(this);
	optionsOkCallback *optOkCb = new optionsOkCallback(this);
	optionsCancelCallback *optCancelCb = new optionsCancelCallback(this);

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
	st_playername_bt = gui->addButton(resources->getTex(ResourceLoader::texType::Inputbox), resources->getTex(ResourceLoader::texType::InputBoxSel), 5, GUIObject::Alignment::center, -375.0f + 30.0f, -275.0f + 70.f, NULL, 5);
	st_playername_txt = gui->addText("Playa", resources->getFont(ResourceLoader::fontType::fnt_mids), 6, GUIObject::Alignment::center, -375.0f + 40.0f, -275.0f + 72.0f, 5);

	st_mousesens_head = gui->addText("Mouse Sensitivity: (default 2.0)", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -375.0f + 30.0f, -275.0f + 145.0f, 5);
	st_mousesens_bt = gui->addButton(resources->getTex(ResourceLoader::texType::Inputbox), resources->getTex(ResourceLoader::texType::InputBoxSel), 5, GUIObject::Alignment::center, -375.0f + 30.0f, -275.0f + 170.f, NULL, 5);
	st_mousesens_txt = gui->addText("2.0", resources->getFont(ResourceLoader::fontType::fnt_mids), 6, GUIObject::Alignment::center, -375.0f + 40.0f, -275.0f + 172.0f, 5);

	st_mouseinv_bt = gui->addButton(resources->getTex(ResourceLoader::texType::CheckboxOff), resources->getTex(ResourceLoader::texType::CheckboxOffSel), 5, GUIObject::Alignment::center, -375.0f + 30.0f, -275.0f + 230.f, NULL, 5); 
	st_mouseinv_txt = gui->addText("Invert Mouse", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -375.0f + 58.0f, -275.0f + 226.0f, 5);

	st_volume_head = gui->addText("Sound Volume: (0-100)", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -375.0f + 30.0f, -275.0f + 315.0f, 5);
	st_volume_bt = gui->addButton(resources->getTex(ResourceLoader::texType::Inputbox), resources->getTex(ResourceLoader::texType::InputBoxSel), 5, GUIObject::Alignment::center, -375.0f + 30.0f, -275.0f + 340.f, NULL, 5);
	st_volume_txt = gui->addText("100", resources->getFont(ResourceLoader::fontType::fnt_mids), 6, GUIObject::Alignment::center, -375.0f + 40.0f, -275.0f + 342.0f, 5);

	st_res_head = gui->addText("Resolution:", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -375.0f + 480.0f, -275.0f + 45.0f, 5);
	st_res_bt_left = gui->addButton(resources->getTex(ResourceLoader::texType::ButtonLeft), resources->getTex(ResourceLoader::texType::ButtonLeftSel), 5, GUIObject::Alignment::center, -375.0f + 480.0f, -275.0f + 70.f, NULL, 5);
	st_res_bt_right = gui->addButton(resources->getTex(ResourceLoader::texType::ButtonRight), resources->getTex(ResourceLoader::texType::ButtonRightSel), 5, GUIObject::Alignment::center, -375.0f + 650.0f, -275.0f + 70.f, NULL, 5);
	st_res_txt = gui->addText("1980 x 1080", resources->getFont(ResourceLoader::fontType::fnt_mids), 6, GUIObject::Alignment::center, -375.0f + 577.0f, -275.0f + 85.0f, 5);
	gui->setCentered(st_res_txt, true);

	st_fullscreen_bt = gui->addButton(resources->getTex(ResourceLoader::texType::CheckboxOff), resources->getTex(ResourceLoader::texType::CheckboxOffSel), 5, GUIObject::Alignment::center, -375.0f + 480.0f, -275.0f + 130.f, NULL, 5);
	st_fullscreen_txt = gui->addText("Fullscreen", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -375.0f + 508.0f, -275.0f + 126.0f, 5);

	//st_shadows_bt, st_shadows_txt; //TODO: implement shadows

	st_shader_bt = gui->addButton(resources->getTex(ResourceLoader::texType::CheckboxOff), resources->getTex(ResourceLoader::texType::CheckboxOffSel), 5, GUIObject::Alignment::center, -375.0f + 480.0f, -275.0f + 190.f, NULL, 5);
	st_shader_txt = gui->addText("Shader Effects", resources->getFont(ResourceLoader::fontType::fnt_normp), 6, GUIObject::Alignment::center, -375.0f + 508.0f, -275.0f + 186.0f, 5);
	
	st_antialias_bt = gui->addButton(resources->getTex(ResourceLoader::texType::CheckboxOff), resources->getTex(ResourceLoader::texType::CheckboxOffSel), 5, GUIObject::Alignment::center, -375.0f + 480.0f, -275.0f + 230.f, NULL, 5);
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


void Menu::frame()
{
	if (clicked_options == true)
	{
		clicked_options = false;

		options_show();
	}
}

void Menu::snd_click()
{
	resources->getSnd(ResourceLoader::sndType::Click)->play(1, 100.f);
}

void Menu::options_show()
{
	// show options menu 
	gui->setGroupVisible(5, true);
}

void Menu::options_hide()
{
	// hide options menu 
	gui->setGroupVisible(5, false);
}
