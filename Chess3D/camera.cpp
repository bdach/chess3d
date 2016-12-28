#include "camera.h"
#include <Eigen/Geometry>
#include <math.h>
#include <SDL_stdinc.h>

Camera::Camera(Eigen::Vector3f _eye, float _aspect_ratio, float _fov, float _near, float _far)
{
	eye = _eye;
	up = Eigen::Vector3f(0, 1, 0);
	aspect_ratio = _aspect_ratio;
	fov = _fov;
	near = _near;
	far = _far;

	projection_matrix = GenerateProjectionMatrix();
	LookAt(Eigen::Vector3f(0, 0, 0));
}

Camera::Camera(Eigen::Vector3f _eye) : Camera(_eye, 3.0f / 4.0f, M_PI / 2, 0.1f, 2.0f)
{
}

Eigen::Matrix4f Camera::LookAt(Eigen::Vector3f _target)
{
	look_at = _target;

	Eigen::Vector3f z_axis = eye - _target;
	z_axis.normalize();
	Eigen::Vector3f x_axis = up.cross(z_axis);
	x_axis.normalize();
	Eigen::Vector3f y_axis = z_axis.cross(x_axis);
	y_axis.normalize();

	Eigen::Matrix4f inverse;
	inverse <<
		x_axis.x(),	y_axis.x(),	z_axis.x(),	eye.x(),
		x_axis.y(),	y_axis.y(),	z_axis.y(), eye.y(),
		x_axis.z(),	y_axis.z(),	z_axis.z(), eye.z(),
		0,			0,			0,			1;

	view_matrix = inverse.inverse();
	return view_matrix;
}

Eigen::Matrix4f Camera::GetProjectionMatrix() const
{
	return projection_matrix;
}

Eigen::Matrix4f Camera::GetViewMatrix() const
{
	return view_matrix;
}

Eigen::Matrix4f Camera::GenerateProjectionMatrix() const
{
	// focal length
	auto e = 1 / tan(fov / 2);

	Eigen::Matrix4f projection;
	projection <<
		e,	0,					0,								0,
		0,	e / aspect_ratio,	0,								0,
		0,	0,					-(far + near) / (far - near),	-(2 * far * near) / (far - near),
		0,	0,					-1,								0;
	return projection;
}
