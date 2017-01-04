#include <algorithm>
#include <SDL_pixels.h>
#include "f_shader.h"
#include "v_shader.h"

#define DEBUG

FragmentShader::FragmentShader(int _width, int _height, std::vector<unsigned char>& _pixel_data) : pixel_data(_pixel_data)
{
	width = _width;
	height = _height;
}

void FragmentShader::Paint(const Face& face, const std::vector<ShadedVertex>& vertices)
{
	std::vector<Eigen::Vector2i> face_vertices;
	for (auto index : face.indices)
	{
		face_vertices.push_back(TransformCoords(vertices[index]));
	}
#ifdef DEBUG
	assert(face.indices.size() == 3);
#endif
	std::sort(face_vertices.begin(), face_vertices.end(), [](const Eigen::Vector2i& v1, const Eigen::Vector2i& v2) -> bool { return v1.y() < v2.y(); });

	FillBottomFlatTriangle(face_vertices[0], face_vertices[1], face_vertices[2]);
	FillTopFlatTriangle(face_vertices[0], face_vertices[1], face_vertices[2]);

	//FillBottomFlatTriangle(Eigen::Vector2i(50, 10), Eigen::Vector2i(300, 200), Eigen::Vector2i(500, 500));
	//FillTopFlatTriangle(Eigen::Vector2i(50, 10), Eigen::Vector2i(300, 200), Eigen::Vector2i(500, 500));
}

Eigen::Vector2i FragmentShader::TransformCoords(const ShadedVertex& vertex) const
{
	return Eigen::Vector2i((vertex.x() + 1) / 2 * width, (vertex.y() - 1) / 2 * -height);
}

void FragmentShader::FillBottomFlatTriangle(Eigen::Vector2i v1, Eigen::Vector2i v2, Eigen::Vector2i v3) const
{
	int y_max = v2.y();
	float m1 = static_cast<float>(v2.x() - v1.x()) / (v2.y() - v1.y());
	float m2 = static_cast<float>(v3.x() - v1.x()) / (v3.y() - v1.y());
	if (m1 > m2) std::swap(m1, m2);

	float x1 = v1.x(), x2 = v1.x();

	for (int y = v1.y(); y <= y_max; ++y)
	{
		for (int x = x1; x <= x2; ++x)
		{
			int offset = 4 * (width * y + x);
			pixel_data[offset + 0] = pixel_data[offset + 1] = pixel_data[offset + 2] = 0xff;
			pixel_data[offset + 3] = SDL_ALPHA_OPAQUE;
		}
		x1 += m1;
		x2 += m2;
	}
}


void FragmentShader::FillTopFlatTriangle(Eigen::Vector2i v1, Eigen::Vector2i v2, Eigen::Vector2i v3) const
{
	int y_min = v2.y();
	//if (v1.x() > v2.x()) std::swap(v1, v2);
	float m1 = static_cast<float>(v1.x() - v3.x()) / (v1.y() - v3.y());
	float m2 = static_cast<float>(v2.x() - v3.x()) / (v2.y() - v3.y());
	if (m1 < m2) std::swap(m1, m2);

	float x1 = v3.x(), x2 = v3.x();

	for (int y = v3.y(); y > y_min; --y)
	{
		for (int x = x1; x <= x2; ++x)
		{
			int offset = 4 * (width * y + x);
			pixel_data[offset + 0] = pixel_data[offset + 1] = pixel_data[offset + 2] = 0xff;
			pixel_data[offset + 3] = SDL_ALPHA_OPAQUE;
		}
		x1 -= m1;
		x2 -= m2;
	}
}