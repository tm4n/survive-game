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
	Mesh *m;

	// crates

	meshes[(int)meshType::Crate] = new Mesh("assets/models/c.mdl", NULL);
	if (meshes[(int)meshType::Crate]->loaded == false) {std::cout << "ERROR loading model for Crate" << std::endl; exit(-1);}
	meshes[(int)meshType::Crate]->initShader();

	meshes[(int)meshType::Crate_damaged] = new Mesh("assets/models/c_destroyed.mdl", NULL);
	if (meshes[(int)meshType::Crate_damaged]->loaded == false) {std::cout << "ERROR loading model for Crate_damaged" << std::endl; exit(-1);}
	meshes[(int)meshType::Crate_damaged]->initShader();

	meshes[(int)meshType::Generator] = new Mesh("assets/models/genera.mdl", NULL);
	if (meshes[(int)meshType::Generator]->loaded == false) {std::cout << "ERROR loading model for Generator" << std::endl; exit(-1);}
	meshes[(int)meshType::Generator]->initShader();

	meshes[(int)meshType::Generator_glass] = new Mesh("assets/models/genera_glass.mdl", NULL);
	if (meshes[(int)meshType::Generator_glass]->loaded == false) {std::cout << "ERROR loading model for Generator_glass" << std::endl; exit(-1);}
	meshes[(int)meshType::Generator_glass]->initShader();

	meshes[(int)meshType::Metalcrate] = new Mesh("assets/models/mc.mdl", NULL);
	if (meshes[(int)meshType::Metalcrate]->loaded == false) {std::cout << "ERROR loading model for Metalcrate" << std::endl; exit(-1);}
	meshes[(int)meshType::Metalcrate]->initShader();

	meshes[(int)meshType::Metalcrate_damaged] = new Mesh("assets/models/mc_destroyed.mdl", NULL);
	if (meshes[(int)meshType::Metalcrate_damaged]->loaded == false) {std::cout << "ERROR loading model for Metalcrate_damaged" << std::endl; exit(-1);}
	meshes[(int)meshType::Metalcrate_damaged]->initShader();

	meshes[(int)meshType::Turred] = new Mesh("assets/models/turret_stand.mdl", NULL);
	if (meshes[(int)meshType::Turred]->loaded == false) {std::cout << "ERROR loading model for Turred" << std::endl; exit(-1);}
	meshes[(int)meshType::Turred]->initShader();

	meshes[(int)meshType::Turred_MG] = new Mesh("assets/models/turret_mg.mdl", NULL);
	if (meshes[(int)meshType::Turred_MG]->loaded == false) {std::cout << "ERROR loading model for Turred_MG" << std::endl; exit(-1);}
	meshes[(int)meshType::Turred_MG]->initShader();


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

	m = new Mesh("assets/models/soldier.mdl", NULL);
	meshes[(int)meshType::Soldier] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();

	// npcs
	m = new Mesh("assets/models/mummy.mdl", NULL);
	meshes[(int)meshType::Mummy] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();

	m = new Mesh("assets/models/werewolf.mdl", NULL);
	meshes[(int)meshType::Werewolf] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();

	m = new Mesh("assets/models/smasher.mdl", NULL);
	meshes[(int)meshType::Smasher] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();

	m = new Mesh("assets/models/witch.mdl", NULL);
	meshes[(int)meshType::Witch] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();

	m = new Mesh("assets/models/hogman.mdl", NULL);
	meshes[(int)meshType::Hogman] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();

	m = new Mesh("assets/models/knight.mdl", NULL);
	meshes[(int)meshType::Knight] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();

	m = new Mesh("assets/models/bauul.mdl", NULL);
	meshes[(int)meshType::Bauul] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();

	m = new Mesh("assets/models/harpy.mdl", NULL);
	meshes[(int)meshType::Harpy] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();

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
		if (textures[i] != NULL) if (textures[i]->loaded == false) {std::cout << "ERROR loading Texture: " << textures[i]->file << std::endl; exit(-1);}
	}
	
	// Fonts
	const char *filename = "assets/OpenSans-Semibold.ttf";
	SDL_RWops *file = SDL_RWFromFile(filename, "rb");
	if (file == NULL) {std::cout << "ERROR on SDL_RWFromFile while opening font file: " << filename << std::endl;  return;}
	fnt = TTF_OpenFontRW(file, 1, 18);
	if (fnt == NULL) {std::cout << "ERROR loading Font: " << filename << std::endl; exit (-1);}
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

TTF_Font *ResourceLoader::getFont()
{
	return fnt;
}
