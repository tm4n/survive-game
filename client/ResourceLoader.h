#ifndef __RESOURCELOADER_H__
#define __RESOURCELOADER_H__

#include "Mesh.h"
#include <vector>

#define MAX_MESHES 100

class ResourceLoader
{
public:
	enum class meshType : int { Crate = 1, Terrain = 2, Tree = 3, Shrub = 4, Soldier = 10 };

	ResourceLoader();

	void load();

	Mesh* getMesh(meshType m);

private:
	Mesh *meshes[MAX_MESHES];

};


#endif