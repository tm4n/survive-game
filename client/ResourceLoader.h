#ifndef __RESOURCELOADER_H__
#define __RESOURCELOADER_H__

#include "Mesh.h"
#include "Texture.h"
#include "Sound.h"
#include <vector>

#define MAX_MESHES 100
#define MAX_TEXTURES 100
#define MAX_SOUNDS 100
#define MAX_FONTS 20

class ResourceLoader
{
public:
	enum class meshType : int { Crate = 1, Crate_damaged = 2, Generator = 3, Generator_glass = 4, 
		Metalcrate = 5, Metalcrate_damaged = 6, Turred = 8, Turred_MG = 9, Weaponcrate = 10, Parachute = 11, Powerup_health = 12,
		Terrain = 20, Tree = 21, Shrub = 22,
		Soldier = 30,
		Mummy = 40, Werewolf = 41, Smasher = 42, Witch = 43, Hogman = 44, Knight = 45, Bauul = 46, Harpy = 47,
		Fireball = 57, Bullet = 58, Muzzleflash = 59, 
		Colt = 60, Colt_hand = 61, Chainsaw = 62, Chainsaw_hand = 63, Wesson = 64, Wesson_hand = 65, HKSL8 = 66, HKSL8_hand = 67, Shotgun = 68, Shotgun_hand = 69, USAS12 = 70, USAS12_hand = 71,
		
		};

	enum class texType : int { Pointer = 1, MenuBackground = 2, MenuPlay = 3, MenuMultiplayer = 4, MenuHelp = 5, MenuOptions = 6, MenuQuit = 7,
		 MenuPlaySel = 8, MenuMultiplayerSel = 9, MenuHelpSel = 10, MenuOptionsSel = 11, MenuQuitSel = 12,
		 Button = 15, ButtonSel = 16, CheckboxOn = 17, CheckboxOff = 18, CheckboxOnSel = 19, CheckboxOffSel = 20,
		 Inputbox = 21, InputBoxSel = 22,
		 ButtonLeft = 23, ButtonLeftSel = 24, ButtonRight = 25, ButtonRightSel = 26,
		 GuiAmmo = 31, GuiHealth = 32, GuiCrosshair = 33, GuiScoreboard = 34,
		 IngameMenuBg = 35, BlackBg = 36,
		 OptionsBg = 37, HelpBg = 38,
		 FlashRed = 40, FlashGreen = 41, Sky = 42,
		 pBlood1 = 60, pBlood2 = 61, pSpark = 62
	};

	enum class sndType : int {
	None = 0, Click = 1, Chat = 2, NextWave = 3, Step = 4,
	Colt_shot = 10, Colt_reload = 11, 
	Chainsaw_shot = 12, Chainsaw_loop = 13,
	Wesson_shoot = 14,
	HKSL8_shoot = 16,
	Shotgun_shoot = 18, Shotgun_reload = 19,
	USAS_shoot = 20,
	Turret = 25,
	Flesh_hit = 30, Metal_hit1 = 31, Wood_hit1 = 32, Wood_hit2 = 33, Wood_hit3 = 34, Fireball = 35, Fireball_hit = 36,
	Generator = 40, Zombie1= 41, Zombie2 = 42, Monster = 43, Harpy1 = 44, Harpy2 = 45}; // TODO: finish

	enum class fontType : int { fnt_small = 1, fnt_norm = 2, fnt_normp = 3, fnt_large = 4, fnt_mid = 5, fnt_mids = 6, fnt_menu = 7};

	ResourceLoader();

	void loadMenu();
	void loadIngame();

	Mesh* getMesh(meshType m);
	Texture* getTex(texType t);
	TTF_Font *getFont(fontType f);
	Sound* getSnd(sndType s);

private:
	Mesh *meshes[MAX_MESHES];
	Texture *textures[MAX_MESHES];
	TTF_Font *fonts[MAX_FONTS];
	Sound *sounds[MAX_SOUNDS];

	void openFont(fontType f, const char *filename, int size);
	
	bool menu_done, ingame_done;

};


#endif
