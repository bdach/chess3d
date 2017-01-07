#include <algorithm>
#include <SDL_pixels.h>
#include "f_shader.h"
#include "v_shader.h"
#include <ctime>
#include <Eigen/Dense>

#define DEBUG

FragmentShader::FragmentShader(int _width, int _height, std::vector<unsigned char>& _pixel_data) : pixel_data(_pixel_data), depth_buffer(_width * _height)
{
	r = g = b = 0xff;
	width = _width;
	height = _height;

	for (auto i = 0; i < _width * _height; ++i)
	{
		depth_buffer[i] = INT_MAX - 1;
	}
}

void FragmentShader::Paint(const Mesh& mesh, const std::vector<ShadedVertex>& vertices)
{
	r = mesh.r;
	g = mesh.g;
	b = mesh.b;
	for (auto face : mesh.faces)
	{
		Paint(face, vertices);
	}
}

void FragmentShader::Paint(const Face& face, const std::vector<ShadedVertex>& vertices)
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

Eigen::Vector3i FragmentShader::TransformCoords(const ShadedVertex& vertex) const
{
	int z = (vertex.z() >= -1 && vertex.z() <= 1) ? (vertex.z() + 1) / 2 * INT_MAX - 1 : INT_MAX;
	return Eigen::Vector3i((vertex.x() + 1) / 2 * width, (vertex.y() - 1) / 2 * -height, z);
}

void FragmentShader::FillBottomFlatTriangle(Eigen::Vector3i v1, Eigen::Vector3i v2, Eigen::Vector3i v3, const Eigen::Vector3f& z_coords)
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

void FragmentShader::FillTopFlatTriangle(Eigen::Vector3i v1, Eigen::Vector3i v2, Eigen::Vector3i v3, const Eigen::Vector3f& z_coords)
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

void FragmentShader::DrawScanline(Eigen::Vector3i v1, Eigen::Vector3i v2, Eigen::Vector3i v3, const Eigen::Vector3f& z_coords, float x1, float x2, int y)
{
	for (int x = fmax(0, x1); x <= fmin(x2, width); ++x)
	{
		if (y < 0 || y >= height) break;
		int offset = width * y + x;
		int z = GetBarycentricCoordinates(v1, v2, v3, x, y) * z_coords;
		if (z >= depth_buffer[offset]) continue;
		depth_buffer[offset] = z;
		pixel_data[4 * offset + 0] = r;
		pixel_data[4 * offset + 1] = g;
		pixel_data[4 * offset + 2] = b;
		pixel_data[4 * offset + 3] = SDL_ALPHA_OPAQUE;
	}
}

Eigen::RowVector3f FragmentShader::GetBarycentricCoordinates(const Eigen::Vector3i& v1, const Eigen::Vector3i& v2, const Eigen::Vector3i& v3, int x, int y)
{
	Eigen::Matrix3f A;
	A <<
		v1.x(), v2.x(), v3.x(),
		v1.y(), v2.y(), v3.y(),
		1, 1, 1;
	Eigen::Vector3f b(x, y, 1);
	Eigen::Vector3f result = A.colPivHouseholderQr().solve(b);
	return result.transpose();
}