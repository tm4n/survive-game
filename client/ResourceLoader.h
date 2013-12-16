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
	enum class meshType : int { Crate = 1, Terrain = 2, Tree = 3, Shrub = 4, Soldier = 10 };

	enum class texType : int { Pointer = 1, MenuBackground = 2, MenuPlay = 3, MenuMultiplayer = 4, MenuHelp = 5, MenuOptions = 6, MenuQuit = 7,
		 MenuPlaySel = 8, MenuMultiplayerSel = 9, MenuHelpSel = 10, MenuOptionsSel = 11, MenuQuitSel = 12
	};

	ResourceLoader();

	void load();

	Mesh* getMesh(meshType m);
	Texture* getTex(texType t);

private:
	Mesh *meshes[MAX_MESHES];
	Texture *textures[MAX_MESHES];

};


#endif