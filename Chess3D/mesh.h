#pragma once
#include <Assimp/scene.h>
#include <vector>
#include "vector3.h"

class Face
{
public:
	explicit Face(const aiFace& face);
private:
	std::vector<unsigned int> indices;
};

class Mesh
{
public:
	Mesh() { }
	explicit Mesh(const aiMesh& mesh);
private:
	std::vector<Vector3D> vertices;
	std::vector<Face> faces;
};