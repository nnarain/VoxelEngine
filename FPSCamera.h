
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

		FPSCamera();
		FPSCamera(const sgl::Vector3& position);

		void update(float delta);

		void updateLookDirection(float x, float y, float delta);

		sgl::Vector3& getPosition(void);
		sgl::Vector3& getDirection(void);

		sgl::Matrix4& getView(void);
		sgl::Matrix4& getProjection(void);

		sgl::Frustum& getFrustum(void);

		void incrementFOV(float inc);

		sgl::Vector3& getVerticalVelocity(void);
		void setVerticalVelocity(float v);

	public:
		sgl::Vector3 position;
		sgl::Vector3 direction;
		sgl::Vector3 right;

	private:

		sgl::Matrix4 _view;
		sgl::Matrix4 _proj;

		sgl::Frustum _frustum;

		float _fov;

		float _lookAngleH;
		float _lookAngleV;

	private:
		void calculateView();
		void updatePosition(float delta);

	};
}

#endif
