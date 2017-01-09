#include "scene.h"

Scene::Scene(const aiScene& scene)
{
	for (unsigned int i = 0; i < scene.mNumMeshes; ++i)
	{
		auto mesh = scene.mMeshes[i];
		meshes.push_back(Mesh(*mesh, *scene.mMaterials[mesh->mMaterialIndex]));
	}
	for (unsigned int i = 0; i < scene.mNumLights; ++i)
	{
		auto light_ptr = scene.mLights[i];
		lights.push_back(Light(*scene.mLights[i], *scene.mRootNode->FindNode(light_ptr->mName)));
	}
}
