

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
	_position(position),
	_lookAngleH(107),
	_lookAngleV(75),
	_fov(45),
	_xDir(Direction::NEU),
	_zDir(Direction::NEU)
{
}

void FPSCamera::update(float delta)
{
	// update the player directions
	calculateView();

	// update the player position
	updatePosition(delta);

	// update the player projection

	float ratio = Context::getScreenAspectRatio();

	_proj.perspective(_fov, ratio, 0.1f, 100.0f);

	// update the view frustum
	_frustum.construct(_fov, ratio, 0.1f, 100.0f, _position, _view);
}

void FPSCamera::updateLookDirection(float x, float y, float delta)
{
	float viewportWidth = Context::getViewPortWidth();
	float viewportHeight = Context::getViewPortHeight();

	_lookAngleH += SPEED * ((viewportWidth / 2) - x) * delta;
	_lookAngleV += SPEED * ((viewportHeight / 2) - y) * delta;
}

void FPSCamera::updatePosition(float delta)
{
	Vector3 d(_direction);
	d.y = 0;

	// update the x direction
	switch (_xDir)
	{
	case FPSCamera::Direction::NEG:
		_position = _position + (-_right * MOVE_SPEED * delta);
		break;
	case FPSCamera::Direction::POS:
		_position = _position + (_right * MOVE_SPEED * delta);
		break;

	case FPSCamera::Direction::NEU:
		break;
	}

	// update the z direction
	switch (_zDir)
	{
	case FPSCamera::Direction::NEG:
		_position = _position + (-d * MOVE_SPEED * delta);
		break;
	case FPSCamera::Direction::POS:
		_position = _position + (d * MOVE_SPEED * delta);
		break;

	case FPSCamera::Direction::NEU:
		break;
	}

	// apply vertical velocity
	_position += _verticalVelocity * delta;

	if (_position.x < 0) _position.x = 0;
	if (_position.z < 0) _position.z = 0;
}

void FPSCamera::calculateView()
{
	// forward
	_direction.x = cos(_lookAngleV) * sin(_lookAngleH);
	_direction.y = sin(_lookAngleV);
	_direction.z = cos(_lookAngleV) * cos(_lookAngleH);

	// right
	_right.x = sin(_lookAngleH - 3.14f / 2.0f);
	_right.y = 0;
	_right.z = cos(_lookAngleH - 3.14f / 2.0f);

	// up
	Vector3 up;
	up.set(_right).cross(_direction);

	//
	_view.lookAt(_position, _position + _direction, up);
}

void FPSCamera::setState(State state)
{
	_state = state;
}

sgl::Vector3& FPSCamera::getPosition(void)
{
	return _position;
}

sgl::Vector3& FPSCamera::getDirection(void)
{
	return _direction;
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

sgl::Vector3& FPSCamera::getVerticalVelocity(void)
{
	return _verticalVelocity;
}

void FPSCamera::setVerticalVelocity(float v)
{
	_verticalVelocity.y = v;
}

void FPSCamera::setXDirection(Direction d)
{
	_xDir = d;
}

void FPSCamera::setZDirection(Direction d)
{
	_zDir = d;
}

void FPSCamera::incrementFOV(float inc)
{
	_fov += inc;
}
