#pragma once
#include "mesh.h"
#include "camera.h"
#include <iterator>

class ShadedVertex
{
public:
	ShadedVertex() {}
	explicit ShadedVertex(Eigen::Vector4f coords);
	float x() const;
	float y() const;
	float z() const;
private:
	Eigen::Vector4f coords;
};

class VertexShader
{
public:
	virtual ~VertexShader() = default;
	virtual void ProcessMesh(const Mesh& mesh) = 0;
	static void TransformCoords(const Mesh& mesh, const Camera& camera, std::back_insert_iterator<std::vector<ShadedVertex>>& iterator);
};
