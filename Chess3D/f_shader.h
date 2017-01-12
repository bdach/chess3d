#pragma once
#include "mesh.h"
#include "v_shader.h"
#include "light.h"

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

template<typename Scalar>
struct CwiseClampOp {
	CwiseClampOp(const Scalar& inf, const Scalar& sup) : m_inf(inf), m_sup(sup) {}
	const Scalar operator()(const Scalar& x) const { return x<m_inf ? m_inf : (x>m_sup ? m_sup : x); }
	Scalar m_inf, m_sup;
};

class LightingModel
{
public:
	LightingModel(const std::vector<Light>& lights, const Camera& camera);
	virtual ~LightingModel() = default;
	virtual Eigen::Vector3f GetColor(const Material& material, Eigen::Vector3f pos, Eigen::Vector3f n) const = 0;
protected:
	const std::vector<Light>& lights;
	const Eigen::Array3f ambient = Eigen::Array3f(0.1, 0.1, 0.1);
	Eigen::Vector3f eye;
	const CwiseClampOp<float> clamp = CwiseClampOp<float>(0, 1);
};

class PhongLightingModel : public LightingModel
{
public:
	PhongLightingModel(const std::vector<Light>& lights, const Camera& camera) : LightingModel(lights, camera) {}
	Eigen::Vector3f GetColor(const Material& material, Eigen::Vector3f pos, Eigen::Vector3f n) const override;
};

class BlinnPhongLightingModel : public LightingModel
{
public:
	BlinnPhongLightingModel(const std::vector<Light>& lights, const Camera& camera) : LightingModel(lights, camera) {}
	Eigen::Vector3f GetColor(const Material& material, Eigen::Vector3f pos, Eigen::Vector3f n) const override;
private:
	Eigen::Vector3f h;
};

class FragmentShader
{
public:
	virtual ~FragmentShader() = default;
	FragmentShader(int width, int height, std::vector<unsigned char>& pixel_data);
	virtual void Paint(const Mesh& mesh, const std::vector<ShadedVertex>& vertices);
protected:
	std::vector<unsigned char>& pixel_data;
	std::vector<int> depth_buffer;
	int width, height;

	Eigen::Vector3f color;

	Eigen::Vector3i TransformCoords(const ShadedVertex& vertex) const;
	virtual void Paint(const Face& face, const std::vector<ShadedVertex>& vertices);

	void FillBottomFlatTriangle(Eigen::Vector3i v1, Eigen::Vector3i v2, Eigen::Vector3i v3, const Eigen::Vector3f& z_coords);
	void FillTopFlatTriangle(Eigen::Vector3i v1, Eigen::Vector3i v2, Eigen::Vector3i v3, const Eigen::Vector3f& z_coords);
	virtual void DrawScanline(Eigen::Vector3i v1, Eigen::Vector3i v2, Eigen::Vector3i v3, const Eigen::Vector3f& z_coords, float x1, float x2, int y);

	static Eigen::RowVector3f GetBarycentricCoordinates(const Eigen::Vector3i& v1, const Eigen::Vector3i& v2, const Eigen::Vector3i& v3, int x, int y);
	const static CwiseClampOp<float> clamp;
};

class FlatFragmentShader : public FragmentShader
{
public:
	FlatFragmentShader(int width, int height, std::vector<unsigned char>& pixel_data, const LightingModel& lighting_model);
	void Paint(const Mesh& mesh, const std::vector<ShadedVertex>& vertices) override;
protected:
	void Paint(const Face& face, const std::vector<ShadedVertex>& vertices) override;

	const LightingModel& lighting_model;
	Material material;
};

class GouraudFragmentShader : public FragmentShader
{
public:
	GouraudFragmentShader(int width, int height, std::vector<unsigned char>& pixel_data, const LightingModel& lighting_model);
	void Paint(const Mesh& mesh, const std::vector<ShadedVertex>& vertices) override;
protected:
	void Paint(const Face& face, const std::vector<ShadedVertex>& vertices) override;
	void DrawScanline(Eigen::Vector3i v1, Eigen::Vector3i v2, Eigen::Vector3i v3, const Eigen::Vector3f& z_coords, float x1, float x2, int y) override;

	const LightingModel& lighting_model;
	Material material;
	Eigen::Matrix3f interpolation_matrix;
};

class PhongFragmentShader : public FragmentShader
{
public:
	PhongFragmentShader(int width, int height, std::vector<unsigned char>& pixel_data, const LightingModel& lighting_model);
	void Paint(const Mesh& mesh, const std::vector<ShadedVertex>& vertices) override;
protected:
	void Paint(const Face& face, const std::vector<ShadedVertex>& vertices) override;
	void DrawScanline(Eigen::Vector3i v1, Eigen::Vector3i v2, Eigen::Vector3i v3, const Eigen::Vector3f& z_coords, float x1, float x2, int y) override;

	const LightingModel& lighting_model;
	Material material;
	Eigen::Matrix3f position_interp_matrix, normal_interp_matrix;
};

class ClickMapFragmentShader : public FragmentShader
{
public:
	ClickMapFragmentShader(int width, int height, std::vector<unsigned char>& click_map) : FragmentShader(width, height, click_map) {}
	void Paint(const Mesh& mesh, const std::vector<ShadedVertex>& vertices) override;
protected:
	void DrawScanline(Eigen::Vector3i v1, Eigen::Vector3i v2, Eigen::Vector3i v3, const Eigen::Vector3f& z_coords, float x1, float x2, int y) override;

	int counter = 0;
};
