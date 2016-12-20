#include "mesh.h"

Mesh::Mesh(const aiMesh& mesh)
{
	for (auto i = 0; i < mesh.mNumVertices; ++i)
	{
		vertices.push_back(Vector3D(mesh.mVertices[i]));
	}
	for (auto i = 0; i < mesh.mNumFaces; ++i)
	{
		faces.push_back(Face(mesh.mFaces[i]));
	}
}

Face::Face(const aiFace& face)
{
	for (auto i = 0; i < face.mNumIndices; ++i)
	{
		indices.push_back(face.mIndices[i]);
	}
}
