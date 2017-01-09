#include "light.h"

Light::Light(const aiLight& light, const aiNode& node)
{
	inner_cone = light.mAngleInnerCone;
	outer_cone = light.mAngleOuterCone;

	att_constant = light.mAttenuationConstant;
	att_linear = light.mAttenuationLinear;
	att_quadratic = light.mAttenuationQuadratic;

	aiVector3D pos;
	aiQuaternion rot;
	node.mTransformation.DecomposeNoScaling(rot, pos);

	position = Eigen::Vector3f(pos.x, pos.y, pos.z);
	direction = Eigen::Vector3f(light.mDirection.x, light.mDirection.y, light.mDirection.z);

	ambient = Eigen::Vector3f(light.mColorAmbient.r, light.mColorAmbient.g, light.mColorAmbient.b);
	diffuse = Eigen::Vector3f(light.mColorDiffuse.r, light.mColorDiffuse.g, light.mColorDiffuse.b);
	specular = Eigen::Vector3f(light.mColorSpecular.r, light.mColorSpecular.g, light.mColorSpecular.b);
}

float Light::GetIntensity(Eigen::Vector3f point) const
{
	auto dist = (position - point).norm();
	return 1 / (att_constant + dist * att_linear + dist * dist * att_quadratic);
}
