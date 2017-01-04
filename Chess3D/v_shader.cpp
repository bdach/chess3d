#include "v_shader.h"

ShadedVertex::ShadedVertex(Eigen::Vector4f _coords)
{
	coords = _coords;
}

float ShadedVertex::x() const
{
	return coords.x();
}

float ShadedVertex::y() const
{
	return coords.y();
}

float ShadedVertex::z() const
{
	return coords.z();
}

void VertexShader::TransformCoords(const Mesh& mesh, const Camera& camera, std::back_insert_iterator<std::vector<ShadedVertex>>& iterator)
{
	Eigen::Matrix4f matrix = camera.GetProjectionMatrix() * camera.GetViewMatrix() * mesh.GetWorldMatrix();
	for (auto it = mesh.vertices.begin(); it != mesh.vertices.end(); ++it)
	{
		Eigen::Vector4f coords = matrix * it->vector;
		coords /= coords.w();
		iterator = ShadedVertex(coords);
	}
}
