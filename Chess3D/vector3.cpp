#include "vector3.h"

Vector3D::Vector3D()
{
	x = y = z = 0;
}

Vector3D::Vector3D(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

Vector3D::Vector3D(const aiVector3D& _vector)
{
	x = _vector.x;
	y = _vector.y;
	z = _vector.z;
}
