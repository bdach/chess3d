#include "scene.h"

Scene::Scene(const aiScene& scene)
{
	auto root_node = scene.mRootNode;
	for (unsigned int i = 0; i < root_node->mNumChildren; ++i)
	{
		auto node = root_node->mChildren[i];
		if (node->mNumMeshes == 0) continue;
		auto mesh = scene.mMeshes[node->mMeshes[0]];
		auto material = scene.mMaterials[mesh->mMaterialIndex];
		meshes.push_back(Mesh(*node, *mesh, *material));
	}
	for (unsigned int i = 0; i < scene.mNumLights; ++i)
	{
		auto light_ptr = scene.mLights[i];
		lights.push_back(Light(*scene.mLights[i], *scene.mRootNode->FindNode(light_ptr->mName)));
	}
}
