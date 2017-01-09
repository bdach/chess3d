#pragma once
#include <Eigen/Core>
#include <Assimp/camera.h>

class Camera
{
public:
	Camera(Eigen::Vector3f _eye, float _aspect_ratio, float _fov, float _near, float _far); // wherever you are
	explicit Camera(Eigen::Vector3f _eye);
	explicit Camera(const aiCamera& camera);

	Eigen::Matrix4f LookAt(Eigen::Vector3f _target);
	Eigen::Matrix4f GetProjectionMatrix() const;
	Eigen::Matrix4f GetViewMatrix() const;

	Eigen::Vector3f eye;
private:
	float aspect_ratio, fov, near, far;

	Eigen::Vector3f look_at;
	Eigen::Vector3f up;

	Eigen::Matrix4f view_matrix;
	Eigen::Matrix4f projection_matrix;

	Eigen::Matrix4f GenerateProjectionMatrix() const;
};
