#pragma once
#include "mesh.h"
#include "camera.h"
#include <iterator>

class ShadedVertex
{
public:
	ShadedVertex() {}
	ShadedVertex(Eigen::Vector4f original, Eigen::Vector4f screen, Eigen::Vector3f normal);
	float ScreenX() const;
	float ScreenY() const;
	float ScreenZ() const;
	Eigen::Vector3f original_coords;
	Eigen::Vector3f normal;
private:
	Eigen::Vector4f screen_coords;
};

class VertexShader
{
public:
	virtual ~VertexShader() = default;
	virtual void ProcessMesh(const Mesh& mesh) = 0;
	static void TransformCoords(const Mesh& mesh, const Camera& camera, std::back_insert_iterator<std::vector<ShadedVertex>>& iterator);
};
