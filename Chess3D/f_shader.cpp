#include <algorithm>
#include <SDL_pixels.h>
#include "f_shader.h"
#include "v_shader.h"
#include <ctime>
#include <Eigen/Dense>

#define DEBUG

const CwiseClampOp<float> LightlessFragmentShader::clamp = CwiseClampOp<float>(0, 1);

LightingModel::LightingModel(const std::vector<Light>& lights, const Camera& camera) : lights(lights)
{
	eye = camera.eye;
}

Eigen::Vector3f PhongLightingModel::GetColor(const Material& material, Eigen::Vector3f pos, Eigen::Vector3f n) const
{
	Eigen::Array3f result = material.ambient.array() * ambient;
	for (auto light : lights)
	{
		auto l = (light.position - pos).normalized();
		auto nl = n.dot(l);
		if (nl < 0) continue;
		auto diffuse = light.diffuse.array() * material.diffuse.array() * nl * light.GetIntensity(pos);
		result += diffuse;
		auto v = (eye - pos).normalized();
		auto r = (2 * nl * n - l).normalized();
		auto specular = light.specular.array() * material.specular.array() * pow(r.dot(v), material.shininess) * light.GetIntensity(pos);
		result += specular;
	}
	return result.unaryExpr(clamp);
}

Eigen::Vector3f BlinnPhongLightingModel::GetColor(const Material& material, Eigen::Vector3f pos, Eigen::Vector3f n) const
{
	Eigen::Array3f result = material.ambient.array() * ambient;
	for (auto light : lights)
	{
		auto l = (light.position - pos).normalized();
		auto nl = n.dot(l);
		if (nl < 0) continue;
		result += light.diffuse.array() * material.diffuse.array() * nl * light.GetIntensity(pos);
		auto v = (eye - pos).normalized();
		auto h = (l + v).normalized();
		result += light.specular.array() * material.specular.array() * pow(n.dot(h), material.shininess) * light.GetIntensity(pos);
	}
	return result.unaryExpr(clamp);
}

LightlessFragmentShader::LightlessFragmentShader(int _width, int _height, std::vector<unsigned char>& _pixel_data) : pixel_data(_pixel_data), depth_buffer(_width * _height)
{
	width = _width;
	height = _height;
	for (auto i = 0; i < _width * _height; ++i)
	{
		depth_buffer[i] = INT_MAX - 1;
	}
}

void LightlessFragmentShader::Paint(const Mesh& mesh, const std::vector<ShadedVertex>& vertices)
{
	color = mesh.material.diffuse;
	for (auto face : mesh.faces)
	{
		Paint(face, vertices);
	}
}

void LightlessFragmentShader::Paint(const Face& face, const std::vector<ShadedVertex>& vertices)
{
	std::vector<Eigen::Vector3i> screen;
	for (auto index : face.indices)
	{
		screen.push_back(TransformCoords(vertices[index]));
	}
	std::sort(screen.begin(), screen.end(), [](const Eigen::Vector3i& v1, const Eigen::Vector3i& v2) -> bool { return v1.y() < v2.y(); });
	Eigen::Vector3f z_coords(screen[0].z(), screen[1].z(), screen[2].z());

	FillBottomFlatTriangle(screen[0], screen[1], screen[2], z_coords);
	FillTopFlatTriangle(screen[0], screen[1], screen[2], z_coords);
}

Eigen::Vector3i LightlessFragmentShader::TransformCoords(const ShadedVertex& vertex) const
{
	int z = (vertex.ScreenZ() >= -1 && vertex.ScreenZ() <= 1) ? (vertex.ScreenZ() + 1) / 2 * INT_MAX - 1 : INT_MAX;
	return Eigen::Vector3i((vertex.ScreenX() + 1) / 2 * width, (vertex.ScreenY() - 1) / 2 * -height, z);
}

void LightlessFragmentShader::FillBottomFlatTriangle(Eigen::Vector3i v1, Eigen::Vector3i v2, Eigen::Vector3i v3, const Eigen::Vector3f& z_coords)
{
	int y_max = v2.y();
	float m1 = static_cast<float>(v2.x() - v1.x()) / (v2.y() - v1.y());
	float m2 = static_cast<float>(v3.x() - v1.x()) / (v3.y() - v1.y());
	if (m1 > m2) std::swap(m1, m2);

	float x1 = v1.x(), x2 = v1.x();

	for (int y = v1.y(); y <= y_max; ++y)
	{
		DrawScanline(v1, v2, v3, z_coords, x1, x2, y);
		x1 += m1;
		x2 += m2;
	}
}

void LightlessFragmentShader::FillTopFlatTriangle(Eigen::Vector3i v1, Eigen::Vector3i v2, Eigen::Vector3i v3, const Eigen::Vector3f& z_coords)
{
	int y_min = v2.y();
	float m1 = static_cast<float>(v1.x() - v3.x()) / (v1.y() - v3.y());
	float m2 = static_cast<float>(v2.x() - v3.x()) / (v2.y() - v3.y());
	if (m1 < m2) std::swap(m1, m2);

	float x1 = v3.x(), x2 = v3.x();

	for (int y = v3.y(); y > y_min; --y)
	{
		DrawScanline(v1, v2, v3, z_coords, x1, x2, y);
		x1 -= m1;
		x2 -= m2;
	}      
}

void LightlessFragmentShader::DrawScanline(Eigen::Vector3i v1, Eigen::Vector3i v2, Eigen::Vector3i v3, const Eigen::Vector3f& z_coords, float x1, float x2, int y)
{
	for (int x = fmax(0, x1); x < fmin(x2, width); ++x)
	{
		if (y < 0 || y >= height) break;
		int offset = width * y + x;
		int z = GetBarycentricCoordinates(v1, v2, v3, x, y) * z_coords;
		if (z >= depth_buffer[offset]) continue;
		depth_buffer[offset] = z;
		pixel_data[4 * offset + 0] = color.x() * 255;
		pixel_data[4 * offset + 1] = color.y() * 255;
		pixel_data[4 * offset + 2] = color.z() * 255;
		pixel_data[4 * offset + 3] = SDL_ALPHA_OPAQUE;
	}
}

Eigen::RowVector3f LightlessFragmentShader::GetBarycentricCoordinates(const Eigen::Vector3i& v1, const Eigen::Vector3i& v2, const Eigen::Vector3i& v3, int x, int y)
{
	Eigen::Vector2f p(x, y), a(v1.x(), v1.y()), b(v2.x(), v2.y()), c(v3.x(), v3.y());
	Eigen::Vector2f t0 = b - a, t1 = c - a, t2 = p - a;
	auto d00 = t0.dot(t0);
	auto d01 = t0.dot(t1);
	auto d11 = t1.dot(t1);
	auto d20 = t2.dot(t0);
	auto d21 = t2.dot(t1);
	auto denom = d00 * d11 - d01 * d01;
	auto v = (d11 * d20 - d01 * d21) / denom;
	auto w = (d00 * d21 - d01 * d20) / denom;
	return Eigen::Vector3f(1.0f - v - w, v, w);
}

FlatFragmentShader::FlatFragmentShader(int width, int height, std::vector<unsigned char>& pixel_data, const LightingModel& lighting_model) : LightlessFragmentShader(width, height, pixel_data), lighting_model(lighting_model)
{
}

void FlatFragmentShader::Paint(const Mesh& mesh, const std::vector<ShadedVertex>& vertices)
{
	material = mesh.material;
	LightlessFragmentShader::Paint(mesh, vertices);
}

void FlatFragmentShader::Paint(const Face& face, const std::vector<ShadedVertex>& vertices)
{
	auto v1 = vertices[face.indices[0]], v2 = vertices[face.indices[1]], v3 = vertices[face.indices[2]];
	auto pos = (v1.original_coords + v2.original_coords + v3.original_coords) / 3;
	auto n = (v1.normal + v2.normal + v3.normal) / 3;
	color = lighting_model.GetColor(material, pos, n);
	LightlessFragmentShader::Paint(face, vertices);
}

GouraudFragmentShader::GouraudFragmentShader(int width, int height, std::vector<unsigned char>& pixel_data, const LightingModel& lighting_model) : LightlessFragmentShader(width, height, pixel_data), lighting_model(lighting_model)
{
}

void GouraudFragmentShader::Paint(const Mesh& mesh, const std::vector<ShadedVertex>& vertices)
{
	material = mesh.material;
	LightlessFragmentShader::Paint(mesh, vertices);
}

void GouraudFragmentShader::Paint(const Face& face, const std::vector<ShadedVertex>& vertices)
{
	std::vector<ShadedVertex> shaded_vertices;
	for (auto index : face.indices)
	{
		shaded_vertices.push_back(vertices[index]);
	}
	std::sort(shaded_vertices.begin(), shaded_vertices.end(), [](const ShadedVertex& v1, const ShadedVertex& v2) -> bool { return v1.ScreenY() >= v2.ScreenY(); });
	interpolation_matrix <<
		lighting_model.GetColor(material, shaded_vertices[0].original_coords, shaded_vertices[0].normal),
		lighting_model.GetColor(material, shaded_vertices[1].original_coords, shaded_vertices[1].normal),
		lighting_model.GetColor(material, shaded_vertices[2].original_coords, shaded_vertices[2].normal);
	interpolation_matrix.transposeInPlace();
	LightlessFragmentShader::Paint(face, vertices);
}

void GouraudFragmentShader::DrawScanline(Eigen::Vector3i v1, Eigen::Vector3i v2, Eigen::Vector3i v3, const Eigen::Vector3f& z_coords, float x1, float x2, int y)
{
	for (int x = fmax(0, x1); x < fmin(x2, width); ++x)
	{
		if (y < 0 || y >= height) break;
		int offset = width * y + x;
		auto barycentric = GetBarycentricCoordinates(v1, v2, v3, x, y);
		int z = barycentric * z_coords;
		if (z >= depth_buffer[offset]) continue;
		depth_buffer[offset] = z;
		color = (barycentric * interpolation_matrix).unaryExpr(clamp);
		pixel_data[4 * offset + 0] = color.x() * 255;
		pixel_data[4 * offset + 1] = color.y() * 255;
		pixel_data[4 * offset + 2] = color.z() * 255;
		pixel_data[4 * offset + 3] = SDL_ALPHA_OPAQUE;
	}
}

PhongFragmentShader::PhongFragmentShader(int width, int height, std::vector<unsigned char>& pixel_data, const LightingModel& lighting_model) : LightlessFragmentShader(width, height, pixel_data), lighting_model(lighting_model)
{
}

void PhongFragmentShader::Paint(const Mesh& mesh, const std::vector<ShadedVertex>& vertices)
{
	material = mesh.material;
	LightlessFragmentShader::Paint(mesh, vertices);
}

void PhongFragmentShader::Paint(const Face& face, const std::vector<ShadedVertex>& vertices)
{
	std::vector<ShadedVertex> shaded_vertices;
	for (auto index : face.indices)
	{
		shaded_vertices.push_back(vertices[index]);
	}
	std::sort(shaded_vertices.begin(), shaded_vertices.end(), [](const ShadedVertex& v1, const ShadedVertex& v2) -> bool { return v1.ScreenY() >= v2.ScreenY(); });
	position_interp_matrix <<
		shaded_vertices[0].original_coords, shaded_vertices[1].original_coords, shaded_vertices[2].original_coords;
	normal_interp_matrix <<
		shaded_vertices[0].normal, shaded_vertices[1].normal, shaded_vertices[2].normal;
	position_interp_matrix.transposeInPlace();
	normal_interp_matrix.transposeInPlace();
	LightlessFragmentShader::Paint(face, vertices);
}

void PhongFragmentShader::DrawScanline(Eigen::Vector3i v1, Eigen::Vector3i v2, Eigen::Vector3i v3, const Eigen::Vector3f& z_coords, float x1, float x2, int y)
{
	for (int x = fmax(0, x1); x < fmin(x2, width); ++x)
	{
		if (y < 0 || y >= height) break;
		int offset = width * y + x;
		auto barycentric = GetBarycentricCoordinates(v1, v2, v3, x, y);
		int z = barycentric * z_coords;
		if (z > depth_buffer[offset]) continue;
		barycentric = barycentric.unaryExpr(clamp);
		depth_buffer[offset] = z;
		auto position = barycentric * position_interp_matrix;
		auto normal = (barycentric * normal_interp_matrix).normalized();
		color = lighting_model.GetColor(material, position, normal);
		pixel_data[4 * offset + 0] = color.x() * 255;
		pixel_data[4 * offset + 1] = color.y() * 255;
		pixel_data[4 * offset + 2] = color.z() * 255;
		pixel_data[4 * offset + 3] = SDL_ALPHA_OPAQUE;
	}
}

void ClickMapFragmentShader::Paint(const Mesh& mesh, const std::vector<ShadedVertex>& vertices)
{
	LightlessFragmentShader::Paint(mesh, vertices);
	counter++;
}

void ClickMapFragmentShader::DrawScanline(Eigen::Vector3i v1, Eigen::Vector3i v2, Eigen::Vector3i v3, const Eigen::Vector3f& z_coords, float x1, float x2, int y)
{
	for (int x = fmax(0, x1); x < fmin(x2, width); ++x)
	{
		if (y < 0 || y >= height) break;
		int offset = width * y + x;
		int z = GetBarycentricCoordinates(v1, v2, v3, x, y) * z_coords;
		if (z >= depth_buffer[offset]) continue;
		depth_buffer[offset] = z;
		pixel_data[offset] = counter;
	}
}
