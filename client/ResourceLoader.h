#ifndef __RESOURCELOADER_H__
#define __RESOURCELOADER_H__

#include "Mesh.h"
#include "Texture.h"
#include <vector>

#define MAX_MESHES 100
#define MAX_TEXTURES 100

class ResourceLoader
{
public:
	enum class meshType : int { Crate = 1, Crate_damaged = 2, Generator = 3, Generator_glass = 4, 
		Metalcrate = 5, Metalcrate_damaged = 6, Turred = 8, Turred_MG = 9, Weaponcrate = 10, Parachute = 11, Powerup_health = 12,
		Terrain = 20, Tree = 21, Shrub = 22,
		Soldier = 30,
		Mummy = 40, Werewolf = 41, Smasher = 42, Witch = 43, Hogman = 44, Knight = 45, Bauul = 46, Harpy = 47,
		Colt = 60, Colt_hand = 61, Chainsaw = 62, Chainsaw_hand = 63, Wesson = 64, Wesson_hand = 65, HKSL8 = 66, HKSL8_hand = 67, Shotgun = 68, Shotgun_hand = 69, USAS12 = 70, USAS12_hand = 71
		};

	enum class texType : int { Pointer = 1, MenuBackground = 2, MenuPlay = 3, MenuMultiplayer = 4, MenuHelp = 5, MenuOptions = 6, MenuQuit = 7,
		 MenuPlaySel = 8, MenuMultiplayerSel = 9, MenuHelpSel = 10, MenuOptionsSel = 11, MenuQuitSel = 12
	};

	ResourceLoader();

	void load();

	Mesh* getMesh(meshType m);
	Texture* getTex(texType t);
	TTF_Font *getFont();

private:
	Mesh *meshes[MAX_MESHES];
	Texture *textures[MAX_MESHES];
	TTF_Font *fnt;

};


#endif
