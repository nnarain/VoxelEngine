

#include "FPSCamera.h"

#include <SGL/Math/MathUtil.h>
#include <SGL/Util/Context.h>

#include <iostream>

#define SPEED 0.5f
#define MOVE_SPEED 20

using namespace engine;
using namespace sgl;

FPSCamera::FPSCamera() : FPSCamera(Vector3(0, 0, 0))
{
}

FPSCamera::FPSCamera(const sgl::Vector3& position) :
	position(position),
	_lookAngleH(107),
	_lookAngleV(75),
	_fov(45)
{
}

void FPSCamera::update()
{
	// update the player directions
	calculateView();

	// update the player projection

	float ratio = Context::getScreenAspectRatio();
	_proj.perspective(_fov, ratio, 0.1f, 100.0f);

	// update the view frustum
	_frustum.construct(_fov, ratio, 0.1f, 100.0f, position, _view);
}

void FPSCamera::updateLookDirection(float x, float y, float delta)
{
	float viewportWidth = Context::getViewPortWidth();
	float viewportHeight = Context::getViewPortHeight();

	_lookAngleH += SPEED * ((viewportWidth / 2) - x) * delta;
	_lookAngleV += SPEED * ((viewportHeight / 2) - y) * delta;
}

void FPSCamera::calculateView()
{
	// forward
	direction.x = cos(_lookAngleV) * sin(_lookAngleH);
	direction.y = sin(_lookAngleV);
	direction.z = cos(_lookAngleV) * cos(_lookAngleH);

	// right
	right.x = sin(_lookAngleH - 3.14f / 2.0f);
	right.y = 0;
	right.z = cos(_lookAngleH - 3.14f / 2.0f);

	// up
	Vector3 up;
	up.set(right).cross(direction);

	//
	_view.lookAt(position, position + direction, up);
}

sgl::Vector3& FPSCamera::getPosition(void)
{
	return position;
}

sgl::Vector3& FPSCamera::getDirection(void)
{
	return direction;
}

sgl::Matrix4& FPSCamera::getView(void)
{
	return _view;
}

sgl::Matrix4& FPSCamera::getProjection(void)
{
	return _proj;
}

sgl::Frustum& FPSCamera::getFrustum(void)
{
	return _frustum;
}

void FPSCamera::incrementFOV(float inc)
{
	_fov += inc;
}
