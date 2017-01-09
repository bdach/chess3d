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
	Mesh(const aiMesh& mesh, const aiMaterial& material);

	std::vector<Vertex> vertices;
	std::vector<Face> faces;
	Eigen::Vector3f color;

	Eigen::Matrix4f GetWorldMatrix() const;
private:
	Eigen::Matrix4f world_matrix;
};