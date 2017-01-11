#include "mesh.h"

Mesh::Mesh()
{
	world_matrix = Eigen::Matrix4f::Identity();
}

Mesh::Mesh(const aiMesh& mesh, const aiMaterial& material) : Mesh()
{
	for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
	{
		vertices.push_back(Vertex(mesh.mVertices[i], mesh.mNormals[i]));
	}
	for (unsigned int i = 0; i < mesh.mNumFaces; ++i)
	{
		faces.push_back(Face(mesh.mFaces[i]));
	}
	aiColor4D color;
	if (AI_SUCCESS == material.Get(AI_MATKEY_COLOR_AMBIENT, color))
	{
		this->material.ambient = Eigen::Vector3f(color.r, color.g, color.b);
	}
	if (AI_SUCCESS == material.Get(AI_MATKEY_COLOR_DIFFUSE, color))
	{
		this->material.diffuse = Eigen::Vector3f(color.r, color.g, color.b);
	}
	if (AI_SUCCESS == material.Get(AI_MATKEY_COLOR_SPECULAR, color))
	{
		this->material.specular = Eigen::Vector3f(color.r, color.g, color.b);
	}
	material.Get(AI_MATKEY_SHININESS, this->material.shininess);
}

Mesh::Mesh(const aiNode& node, const aiMesh& mesh, const aiMaterial& material) : Mesh(mesh, material)
{
	auto transform = node.mTransformation;
	world_matrix <<
		transform.a1, transform.a2, transform.a3, transform.a4,
		transform.b1, transform.b2, transform.b3, transform.b4,
		transform.c1, transform.c2, transform.c3, transform.c4,
		transform.d1, transform.d2, transform.d3, transform.d4;
}

Eigen::Matrix4f Mesh::GetWorldMatrix() const
{
	return world_matrix;
}

void Mesh::Translate(Eigen::Vector3f vec)
{
	Eigen::Matrix4f translation;
	translation <<
		1, 0, 0, vec.x(),
		0, 1, 0, vec.y(),
		0, 0, 1, vec.z(),
		0, 0, 0, 1;
	world_matrix = world_matrix * translation;
}

Face::Face(const aiFace& face)
{
	for (auto i = 0; i < face.mNumIndices; ++i)
	{
		indices.push_back(face.mIndices[i]);
	}
}
