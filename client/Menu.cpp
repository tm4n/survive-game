#include "Menu.h"

#include "ResourceLoader.h"


Menu::Menu(GUI *agui, ResourceLoader *aresources)
{
	gui = agui;
	resources = aresources;


	// add menu background
	bg_id = gui->addPanel(resources->getTex(ResourceLoader::texType::MenuBackground), 1, 0.5f, 0.5f);
	gui->setCentered(bg_id, true);
	gui->setScaleX(bg_id, gui->screensize_x/1920.f);
	gui->setScaleY(bg_id, gui->screensize_y/1080.f);
	//gui->setScaleX(bg_id, 0.01f);
	//gui->setScaleY(bg_id, 0.01f);

	// add buttons
	button_ids[0] = gui->addButton(resources->getTex(ResourceLoader::texType::MenuPlay), resources->getTex(ResourceLoader::texType::MenuPlaySel), 2, 0.653f, 0.20f, NULL);
	button_ids[1] = gui->addButton(resources->getTex(ResourceLoader::texType::MenuMultiplayer), resources->getTex(ResourceLoader::texType::MenuMultiplayerSel), 2, 0.655f, 0.34f, NULL);
	button_ids[2] = gui->addButton(resources->getTex(ResourceLoader::texType::MenuHelp), resources->getTex(ResourceLoader::texType::MenuHelpSel), 2, 0.651f, 0.49f, NULL);
	button_ids[3] = gui->addButton(resources->getTex(ResourceLoader::texType::MenuOptions), resources->getTex(ResourceLoader::texType::MenuOptionsSel), 2, 0.653f, 0.645f, NULL);
	button_ids[4] = gui->addButton(resources->getTex(ResourceLoader::texType::MenuQuit), resources->getTex(ResourceLoader::texType::MenuQuitSel), 2, 0.653f, 0.766f, NULL);
}


void Menu::show()
{
	visible = true;

	gui->setVisible(bg_id, true);

	for (int i = 0; i < SELECTION_MAX; i++)
	{
		gui->setVisible(button_ids[i], true);
	}
}

void Menu::hide()
{
	visible = false;

	gui->setVisible(bg_id, false);

	for (int i = 0; i < SELECTION_MAX; i++)
	{
		gui->setVisible(button_ids[i], false);
	}
}


void Menu::frame()
{
}