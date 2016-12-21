#include "scene.h"

Scene::Scene(const aiScene& scene)
{
	for (unsigned int i = 0; i < scene.mNumMeshes; ++i)
	{
		meshes.push_back(Mesh(*scene.mMeshes[i]));
	}
}
