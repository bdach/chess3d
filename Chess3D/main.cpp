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
	const std::string path = "../Chess3D/Models/wt_teapot.obj";
	Window window("SDL Test");
	auto scene = LoadObject(path);
	scene.cameras.push_back(Camera(Eigen::Vector3f(0, 2, 2)));
	window.Show(scene);
	return 0;
}

Scene LoadObject(const std::string& path)
{
	Mesh mesh;
	Assimp::Importer importer;
	auto scene = importer.ReadFile(path,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);
	if (nullptr == scene)
	{
		std::cerr << "[Error] Loading model failed" << std::endl;
	}
	return Scene(*scene);
}