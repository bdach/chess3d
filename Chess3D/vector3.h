#pragma once
#include <Assimp/vector3.h>

class Vector3D
{
public:
	Vector3D();
	Vector3D(float _x, float _y, float _z);
	explicit Vector3D(const aiVector3D& _vector);
private:
	float x, y, z;
};
