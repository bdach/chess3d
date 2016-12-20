#include <iostream>
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>
#include "mesh.h"
#include "window.h"

Mesh LoadObject(const std::string& path);

int main(int argc, char **argv)
{
	const std::string path = "../Chess3D/Models/wt_teapot.obj";
	Window window("SDL Test");
	auto mesh = LoadObject(path);
	window.Show();
	return 0;
}

Mesh LoadObject(const std::string& path)
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
	if (scene->mNumMeshes >= 1)
	{
		mesh = Mesh(*scene->mMeshes[0]);
	}
	return mesh;
}