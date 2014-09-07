#include "Menu.h"

#include "ResourceLoader.h"


Menu::Menu(GUI *agui, ResourceLoader *aresources, GUICallback *playCb, GUICallback *quitCb)
{
	gui = agui;
	resources = aresources;


	// add menu background
	bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::MenuBackground), 1, GUIObject::Alignment::scaled, 0.0f, 0.0f);

	// add buttons
	button_ids[0] = gui->addButton(resources->getTex(ResourceLoader::texType::MenuPlay), resources->getTex(ResourceLoader::texType::MenuPlaySel), 2, GUIObject::Alignment::scaled, 0.653f, 0.20f, playCb);
	button_ids[1] = gui->addButton(resources->getTex(ResourceLoader::texType::MenuMultiplayer), resources->getTex(ResourceLoader::texType::MenuMultiplayerSel), 2, GUIObject::Alignment::scaled, 0.655f, 0.34f, NULL);
	button_ids[2] = gui->addButton(resources->getTex(ResourceLoader::texType::MenuHelp), resources->getTex(ResourceLoader::texType::MenuHelpSel), 2, GUIObject::Alignment::scaled, 0.651f, 0.49f, NULL);
	button_ids[3] = gui->addButton(resources->getTex(ResourceLoader::texType::MenuOptions), resources->getTex(ResourceLoader::texType::MenuOptionsSel), 2, GUIObject::Alignment::scaled, 0.653f, 0.645f, NULL);
	button_ids[4] = gui->addButton(resources->getTex(ResourceLoader::texType::MenuQuit), resources->getTex(ResourceLoader::texType::MenuQuitSel), 2, GUIObject::Alignment::scaled, 0.653f, 0.766f, quitCb);
	
	gui->setButtonSwitchDown(button_ids[0], button_ids[1]);
	gui->setButtonSwitchDown(button_ids[1], button_ids[2]);
	gui->setButtonSwitchUp(button_ids[1], button_ids[0]);
	gui->setButtonSwitchDown(button_ids[2], button_ids[3]);
	gui->setButtonSwitchUp(button_ids[2], button_ids[1]);
	gui->setButtonSwitchDown(button_ids[3], button_ids[4]);
	gui->setButtonSwitchUp(button_ids[3], button_ids[2]);
	gui->setButtonSwitchUp(button_ids[4], button_ids[3]); 

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
}
