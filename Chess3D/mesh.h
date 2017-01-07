#pragma once
#include <Assimp/scene.h>
#include <vector>
#include "vertex.h"

class Face
{
public:
	explicit Face(const aiFace& face);
	std::vector<unsigned int> indices;
};

class Mesh
{
public:
	Mesh();
	explicit Mesh(const aiMesh& mesh);

	std::vector<Vertex> vertices;
	std::vector<Face> faces;
	unsigned char r, g, b;

	Eigen::Matrix4f GetWorldMatrix() const;
private:
	Eigen::Matrix4f world_matrix;
};