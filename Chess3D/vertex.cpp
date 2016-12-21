#include "vertex.h"

Vertex::Vertex(const aiVector3D& _vector) : Vertex(_vector.x, _vector.y, _vector.z)
{
}

Vertex::Vertex() : Vertex(0, 0, 0)
{
}

Vertex::Vertex(float _x, float _y, float _z) : vector(_x, _y, _z, 1)
{
}
