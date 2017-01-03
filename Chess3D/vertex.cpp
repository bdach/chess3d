#include "vertex.h"

Vertex::Vertex(const aiVector3D& _vector, const aiVector3D& _normal)
{
	vector = Eigen::Vector4f(_vector.x, _vector.y, _vector.z, 1);
	normal = Eigen::Vector3f(_normal.x, _normal.y, _normal.z);
}

Vertex::Vertex()
{
}
