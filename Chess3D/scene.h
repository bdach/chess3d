#pragma once
#include <Assimp/scene.h>
#include <vector>
#include "mesh.h"
#include "camera.h"

class Scene
{
public: 
	Scene() {}
	explicit Scene(const aiScene& scene);

	std::vector<Mesh> meshes;
	std::vector<Camera> cameras;
};
