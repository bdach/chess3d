#pragma once
#include <Assimp/vector3.h>
#include <Eigen/Core>

class Vertex
{
public:
	Vertex();
	Vertex(float _x, float _y, float _z);
	explicit Vertex(const aiVector3D& _vector);

	Eigen::Vector4f vector;
};
