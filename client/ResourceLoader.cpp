#include "ResourceLoader.h"
#include <iostream>

ResourceLoader::ResourceLoader()
{
	for (int i = 0; i < MAX_MESHES; i++) meshes[i] = NULL;
}

void ResourceLoader::load()
{
	meshes[(int)meshType::Crate] = new Mesh("assets/models/c.mdl", NULL);
	if (meshes[(int)meshType::Crate]->loaded == false) {std::cout << "ERROR loading model for Crate" << std::endl; exit(-1);}
	meshes[(int)meshType::Crate]->initShader();

	meshes[(int)meshType::Terrain] = new Mesh("assets/models/terrain.mdl", NULL);
	if (meshes[(int)meshType::Terrain]->loaded == false) {std::cout << "ERROR loading model for terrain" << std::endl; exit(-1);}
	meshes[(int)meshType::Terrain]->initShader();

	meshes[(int)meshType::Soldier] = new Mesh("assets/models/soldier.mdl", NULL);
	if (meshes[(int)meshType::Soldier]->loaded == false) {std::cout << "ERROR loading model for terrain" << std::endl; exit(-1);}
	meshes[(int)meshType::Soldier]->initShader();
}

Mesh* ResourceLoader::getMesh(meshType m)
{
	int id = (int)m;
	if (id < 1 || id > MAX_MESHES) return NULL;
	return meshes[id];
}