#include <iostream>
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>
#include "window.h"
#include "scene.h"

#ifdef main
#undef main
#endif

Scene LoadObject(const std::string& path);

int main(int argc, char **argv)
{
	const std::string path = "../Chess3D/Models/chess2.dae";
	Window window("Chess3D");
	auto scene = LoadObject(path);
	Camera camera(Eigen::Vector3f(16, 26, 15));
	camera.LookAt(Eigen::Vector3f(6, 7, 0));
	scene.cameras.push_back(camera);
	window.Show(scene);
	return 0;
}

Scene LoadObject(const std::string& path)
{
	Assimp::Importer importer;
	auto scene = importer.ReadFile(path,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);
	if (NULL == scene)
	{
		std::cerr << "[Error] Loading model failed" << std::endl;
		return Scene();
	}
	std::cerr << "Meshes: " << scene->mNumMeshes << std::endl;
	std::cerr << "Cameras: " << scene->mNumCameras << std::endl;
	std::cerr << "Lights: " << scene->mNumLights << std::endl;
	return Scene(*scene);
}