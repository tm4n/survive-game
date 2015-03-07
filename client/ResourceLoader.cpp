#include "ResourceLoader.h"
#include <iostream>
#include "MeshTerrain.h"
#include "MeshGUI.h"
#include "MeshBlend.h"
#include "helper.h"

ResourceLoader::ResourceLoader()
{
	for (int i = 0; i < MAX_MESHES; i++) meshes[i] = NULL;
	for (int i = 0; i < MAX_TEXTURES; i++) textures[i] = NULL;
	for (int i = 0; i < MAX_SOUNDS; i++) sounds[i] = NULL;
	
	menu_done = false;
	ingame_done = false;
}

void ResourceLoader::loadIngame()
{
	if (ingame_done == true) return;
	
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

	m = new Mesh("assets/models/wc.mdl", NULL);
	meshes[(int)meshType::Weaponcrate] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();

	m = new Mesh("assets/models/parachute.mdl", NULL);
	meshes[(int)meshType::Parachute] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();
	
	m = new Mesh("assets/models/powerup_hp.mdl", NULL);
	meshes[(int)meshType::Powerup_health] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();
	


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
	m = new Mesh("assets/models/bullet.mdl", NULL);
	meshes[(int)meshType::Bullet] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();

	m = new Mesh("assets/models/weapons/colt.mdl", NULL);
	meshes[(int)meshType::Colt] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();
	
	m = new MeshGUI("assets/models/weapons/colt_hand.mdl", NULL);
	meshes[(int)meshType::Colt_hand] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();
	
	m = new Mesh("assets/models/weapons/chainsaw.mdl", NULL);
	meshes[(int)meshType::Chainsaw] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();
	
	m = new MeshGUI("assets/models/weapons/chainsaw_hand.mdl", NULL);
	meshes[(int)meshType::Chainsaw_hand] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();
	
	m = new Mesh("assets/models/weapons/wesson.mdl", NULL);
	meshes[(int)meshType::Wesson] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();
	
	m = new MeshGUI("assets/models/weapons/wesson_hand.mdl", NULL);
	meshes[(int)meshType::Wesson_hand] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();
	
	m = new Mesh("assets/models/weapons/hksl8.mdl", NULL);
	meshes[(int)meshType::HKSL8] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();
	
	m = new MeshGUI("assets/models/weapons/hksl8_hand.mdl", NULL);
	meshes[(int)meshType::HKSL8_hand] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();
	
	m = new Mesh("assets/models/weapons/shotgun.mdl", NULL);
	meshes[(int)meshType::Shotgun] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();
	
	m = new MeshGUI("assets/models/weapons/shotgun_hand.mdl", NULL);
	meshes[(int)meshType::Shotgun_hand] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();
	
	m = new Mesh("assets/models/weapons/usas12.mdl", NULL);
	meshes[(int)meshType::USAS12] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();
	
	m = new MeshGUI("assets/models/weapons/usas12_hand.mdl", NULL);
	meshes[(int)meshType::USAS12_hand] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();
	
	m = new MeshBlend("assets/models/weapons/muzzle.mdl", NULL);
	meshes[(int)meshType::Muzzleflash] = m;
	if (m == false) {std::cout << "ERROR loading model '" << m->filename << "'" << std::endl; exit(-1);}
	m->initShader();
	
	
	// ingame textures
	
	textures[(int)texType::GuiAmmo] = new Texture("assets/gui/gui_ammo.tga");
	textures[(int)texType::GuiHealth] = new Texture("assets/gui/gui_health.tga");
	textures[(int)texType::GuiCrosshair] = new Texture("assets/gui/crosshair.tga");
	textures[(int)texType::GuiScoreboard] = new Texture("assets/gui/score_bg.tga");

	textures[(int)texType::IngameMenuBg] = new Texture("assets/gui/ingame_menu_bg.tga");
	textures[(int)texType::FlashRed] = new Texture("assets/gui/flash_red.tga");
	textures[(int)texType::FlashGreen] = new Texture("assets/gui/flash_green.tga");

	// particles
	textures[(int)texType::pBlood1] = new Texture("assets/textures/blood1.tga");
	textures[(int)texType::pBlood2] = new Texture("assets/textures/blood2.tga");
	textures[(int)texType::pSpark] = new Texture("assets/textures/spark.tga");

	// check if all textures have been successfully loaded
	for (int i = 0; i < MAX_TEXTURES; i++)
	{
		if (textures[i] != NULL) if (textures[i]->loaded == false) {std::cout << "ERROR loading Texture: " << textures[i]->file << std::endl; exit(-1);}
	}


	// Sounds
	//sounds[(int)]


	ingame_done = true;
}

void ResourceLoader::loadMenu()
{
	if (menu_done == true) return;

	log(LOG_DEBUG_VERBOSE, "loading menu resources");



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

	textures[(int)texType::BlackBg] = new Texture("assets/gui/black.tga");


	// check if all textures have been successfully loaded
	for (int i = 0; i < MAX_TEXTURES; i++)
	{
		if (textures[i] != NULL) if (textures[i]->loaded == false) {std::cout << "ERROR loading Texture: " << textures[i]->file << std::endl; exit(-1);}
	}

	// Sounds
	sounds[(int)sndType::Click] = new Sound("assets/sounds/click.wav");
	if (sounds[(int)sndType::Click]->loaded == false) { std::cout << "ERROR loading Sound: " << sounds[(int)sndType::Click]->file << std::endl; exit(-1); }
	

	// Fonts

	openFont(fontType::fnt_small, "assets/OpenSans-Semibold.ttf", 10);
	openFont(fontType::fnt_norm, "assets/OpenSans-Semibold.ttf", 16);
	openFont(fontType::fnt_normp, "assets/OpenSans-Bold.ttf", 16);
	openFont(fontType::fnt_large, "assets/OpenSans-Bold.ttf", 36);
	openFont(fontType::fnt_mid, "assets/OpenSans-Bold.ttf", 24);
	openFont(fontType::fnt_mids, "assets/OpenSans-Bold.ttf", 18);
	openFont(fontType::fnt_menu, "assets/OpenSans-Bold.ttf", 20);

	
	menu_done = true;
}

void ResourceLoader::openFont(fontType f, const char *filename, int size)
{
	SDL_RWops *file = SDL_RWFromFile(filename, "rb");
	if (file == NULL) {std::cout << "ERROR on SDL_RWFromFile while opening font file: " << filename << std::endl;  exit( (-2));}

	fonts[(int)f] = TTF_OpenFontRW(file, 1, size);
	if (fonts[(int)f] == NULL) {std::cout << "ERROR loading Font " << filename << " size:" << size << std::endl; exit (-1);}
	
	TTF_SetFontHinting(fonts[(int)f], TTF_HINTING_LIGHT); 
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

Sound* ResourceLoader::getSnd(sndType s)
{
	int id = (int)s;
	if (id < 1 || id > MAX_SOUNDS) return NULL;
	return sounds[id];
}

TTF_Font *ResourceLoader::getFont(fontType f)
{
	int id = (int)f;
	if (id < 1 || id > MAX_FONTS) return NULL;
	return fonts[id];
}
