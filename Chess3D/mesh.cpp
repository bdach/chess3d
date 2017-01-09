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

Eigen::Matrix4f Mesh::GetWorldMatrix() const
{
	return world_matrix;
}

Face::Face(const aiFace& face)
{
	for (auto i = 0; i < face.mNumIndices; ++i)
	{
		indices.push_back(face.mIndices[i]);
	}
}
