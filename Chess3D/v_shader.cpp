#include "v_shader.h"

ShadedVertex::ShadedVertex(Eigen::Vector4f original, Eigen::Vector4f screen, Eigen::Vector3f normal)
{
	screen_coords = screen;
	original_coords = Eigen::Vector3f(original.x(), original.y(), original.z());
	this->normal = normal;
}

float ShadedVertex::ScreenX() const
{
	return screen_coords.x();
}

float ShadedVertex::ScreenY() const
{
	return screen_coords.y();
}

float ShadedVertex::ScreenZ() const
{
	return screen_coords.z();
}

void VertexShader::TransformCoords(const Mesh& mesh, const Camera& camera, std::back_insert_iterator<std::vector<ShadedVertex>>& iterator)
{
	Eigen::Matrix4f matrix = camera.GetProjectionMatrix() * camera.GetViewMatrix() * mesh.GetWorldMatrix();
	for (auto it = mesh.vertices.begin(); it != mesh.vertices.end(); ++it)
	{
		Eigen::Vector4f coords = matrix * it->vector;
		coords /= coords.w();
		iterator = ShadedVertex(mesh.GetWorldMatrix() * it->vector, coords, it->normal);
	}
}
