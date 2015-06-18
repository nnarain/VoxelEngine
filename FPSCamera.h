
#ifndef FPSCAMERA_H
#define FPSCAMERA_H

#include <SGL/Util/PerspectiveCamera.h>

#include <SGL/Math/Math.h>
#include <SGL/Math/Frustum.h>

namespace engine
{
	class FPSCamera
	{
	public:

		// State
		enum class State
		{
			STAND
		};

		enum class Direction
		{
			POS, NEG, NEU
		};

		FPSCamera();
		FPSCamera(const sgl::Vector3& position);

		void update(float delta);

		void updateLookDirection(float x, float y, float delta);

		void setState(State state);

		sgl::Vector3& getPosition(void);
		sgl::Vector3& getDirection(void);

		sgl::Matrix4& getView(void);
		sgl::Matrix4& getProjection(void);

		sgl::Frustum& getFrustum(void);

		void setXDirection(Direction d);
		void setZDirection(Direction d);

		void incrementFOV(float inc);

		sgl::Vector3& getVerticalVelocity(void);
		void setVerticalVelocity(float v);

	private:

		State _state;

		Direction _xDir;
		Direction _zDir;

		sgl::Vector3 _position;
		sgl::Vector3 _direction;
		sgl::Vector3 _right;

		sgl::Matrix4 _view;
		sgl::Matrix4 _proj;

		sgl::Vector3 _verticalVelocity;

		sgl::Frustum _frustum;

		float _fov;

		float _lookAngleH;
		float _lookAngleV;

		/*  */
		void calculateView();

		void updatePosition(float delta);

	};
}

#endif
