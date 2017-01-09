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

struct Material
{
	Eigen::Vector3f ambient;
	Eigen::Vector3f diffuse;
	Eigen::Vector3f specular;
	float shininess;
};

class Mesh
{
public:
	Mesh();
	Mesh(const aiMesh& mesh, const aiMaterial& material);

	std::vector<Vertex> vertices;
	std::vector<Face> faces;
	Material material;

	Eigen::Matrix4f GetWorldMatrix() const;
private:
	Eigen::Matrix4f world_matrix;
};