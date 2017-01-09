#pragma once
#include <Assimp/light.h>
#include <Assimp/scene.h>
#include <Eigen/Core>

class Light
{
public:
	Light(const aiLight& light, const aiNode& node);

	float GetIntensity(Eigen::Vector3f point) const;
	Eigen::Vector3f ambient, diffuse, specular;
private: 
	float inner_cone, outer_cone;
	float att_constant, att_linear, att_quadratic;
	Eigen::Vector3f direction;
	Eigen::Vector3f position;
};
