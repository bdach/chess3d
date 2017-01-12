#include <iostream>
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>
#include "window.h"
#include "scene.h"


Scene LoadObject(const std::string& path);

int main(int argc, char **argv)
{
	const std::string path = "chess.dae";
	auto scene = LoadObject(path);
	Window window("Chess3D", scene);
	window.Show();
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