#include "ResourceLoader.h"
#include <iostream>
#include "MeshTerrain.h"

ResourceLoader::ResourceLoader()
{
	for (int i = 0; i < MAX_MESHES; i++) meshes[i] = NULL;
	for (int i = 0; i < MAX_TEXTURES; i++) textures[i] = NULL;
}

void ResourceLoader::load()
{
	// crates

	meshes[(int)meshType::Crate] = new Mesh("assets/models/c.mdl", NULL);
	if (meshes[(int)meshType::Crate]->loaded == false) {std::cout << "ERROR loading model for Crate" << std::endl; exit(-1);}
	meshes[(int)meshType::Crate]->initShader();



	// Environment

	meshes[(int)meshType::Terrain] = new MeshTerrain("assets/models/terrain.mdl", NULL);
	if (meshes[(int)meshType::Terrain]->loaded == false) {std::cout << "ERROR loading model for terrain" << std::endl; exit(-1);}
	meshes[(int)meshType::Terrain]->initShader();

	meshes[(int)meshType::Tree] = new Mesh("assets/models/tree1.mdl", NULL);
	if (meshes[(int)meshType::Tree]->loaded == false) {std::cout << "ERROR loading model for tree1" << std::endl; exit(-1);}
	meshes[(int)meshType::Tree]->initShader();

	meshes[(int)meshType::Shrub] = new Mesh("assets/models/shrub1.mdl", NULL);
	if (meshes[(int)meshType::Shrub]->loaded == false) {std::cout << "ERROR loading model for shrub" << std::endl; exit(-1);}
	meshes[(int)meshType::Shrub]->initShader();



	// characters

	meshes[(int)meshType::Soldier] = new Mesh("assets/models/soldier.mdl", NULL);
	if (meshes[(int)meshType::Soldier]->loaded == false) {std::cout << "ERROR loading model for soldier" << std::endl; exit(-1);}
	meshes[(int)meshType::Soldier]->initShader();

	// weapons








	////////////////////////////////////////////////////
	// Textures

	textures[(int)texType::MenuBackground] = new Texture("assets/gui/menu_bg.tga");

	textures[(int)texType::MenuPlay] = new Texture("assets/gui/menu_play.tga");
	textures[(int)texType::MenuPlaySel] = new Texture("assets/gui/menu_play_sel.tga");
	textures[(int)texType::MenuMultiplayer] = new Texture("assets/gui/menu_multiplayer.tga");
	textures[(int)texType::MenuMultiplayerSel] = new Texture("assets/gui/menu_multiplayer_sel.tga");
	textures[(int)texType::MenuOptions] = new Texture("assets/gui/menu_options.tga");
	textures[(int)texType::MenuOptionsSel] = new Texture("assets/gui/menu_options_sel.tga");
	textures[(int)texType::MenuHelp] = new Texture("assets/gui/menu_help.tga");
	textures[(int)texType::MenuHelpSel] = new Texture("assets/gui/menu_help_sel.tga");
	textures[(int)texType::MenuQuit] = new Texture("assets/gui/menu_quit.tga");
	textures[(int)texType::MenuQuitSel] = new Texture("assets/gui/menu_quit_sel.tga");

	// check if all textures have been successfully loaded
	for (int i = 0; i < MAX_TEXTURES; i++)
	{
		if (textures[i] != NULL) if (textures[i]->loaded == false) {std::cout << "ERROR loading Texture: " << textures[i]->file << std::endl;}
	}
}


Mesh* ResourceLoader::getMesh(meshType m)
{
	int id = (int)m;
	if (id < 1 || id > MAX_MESHES) return NULL;
	return meshes[id];
}

Texture* ResourceLoader::getTex(texType m)
{
	int id = (int)m;
	if (id < 1 || id > MAX_TEXTURES) return NULL;
	return textures[id];
}