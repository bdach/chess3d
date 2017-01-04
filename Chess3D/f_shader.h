#pragma once
#include "mesh.h"
#include "v_shader.h"

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

class FragmentShader
{
public:
	FragmentShader(int width, int height, std::vector<unsigned char>& pixel_data);
	void Paint(const Face& face, const std::vector<ShadedVertex>& vertices);
private:
	std::vector<unsigned char>& pixel_data;
	int width, height;

	Eigen::Vector2i TransformCoords(const ShadedVertex& vertex) const;
	void FillBottomFlatTriangle(Eigen::Vector2i v1, Eigen::Vector2i v2, Eigen::Vector2i v3) const;
	void FillTopFlatTriangle(Eigen::Vector2i v1, Eigen::Vector2i v2, Eigen::Vector2i v3) const;
};
