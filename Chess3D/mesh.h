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

enum MeshType
{
	UNKNOWN = 0,
	PIECE,
	FIELD
};

class Mesh
{
public:
	Mesh();
	Mesh(const aiMesh& mesh, const aiMaterial& material);
	Mesh(const aiNode& node, const aiMesh& mesh, const aiMaterial& material);

	std::vector<Vertex> vertices;
	std::vector<Face> faces;
	Material material;
	MeshType mesh_type = UNKNOWN;
	std::string name;

	Eigen::Matrix4f GetWorldMatrix() const;
	Eigen::Vector3f GetPosition() const;

	void Move(Eigen::Vector3f vec);
private:
	Eigen::Matrix4f world_matrix;
};