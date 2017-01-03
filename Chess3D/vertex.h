#pragma once
#include <Assimp/vector3.h>
#include <Eigen/Core>

class Vertex
{
public:
	Vertex();
	explicit Vertex(const aiVector3D& _vector, const aiVector3D& _normal);

	Eigen::Vector4f vector;
	Eigen::Vector3f normal;
};
