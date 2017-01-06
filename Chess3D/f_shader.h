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
	std::vector<int> depth_buffer;
	int width, height;

	unsigned char r, g, b; // TODO remove later

	Eigen::Vector3i TransformCoords(const ShadedVertex& vertex) const;
	void FillBottomFlatTriangle(Eigen::Vector3i v1, Eigen::Vector3i v2, Eigen::Vector3i v3, const Eigen::Vector3f& z_coords);
	void FillTopFlatTriangle(Eigen::Vector3i v1, Eigen::Vector3i v2, Eigen::Vector3i v3, const Eigen::Vector3f& z_coords);

	static Eigen::RowVector3f GetBarycentricCoordinates(const Eigen::Vector3i& v1, const Eigen::Vector3i& v2, const Eigen::Vector3i& v3, int x, int y);
};
