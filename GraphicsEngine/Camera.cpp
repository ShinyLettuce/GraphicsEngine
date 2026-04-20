#include "Camera.h"
#include <cmath>

void Camera::Init(float aFarClip, float aNearClip, float aFOVinDeg, float aAspect)
{
	myFarClip = aFarClip;
	myNearClip = aNearClip;
	myYfov = aFOVinDeg * 3.1415927f / 180.f;

	float zoomY = 1.f / tan(myYfov * 0.5f);
	float zoomX = zoomY * (aAspect);

	myProjectionMatrix.worldToClipMatrix = Matrix4x4<float> {
		zoomX, 0.f, 0.f, 0.f,
		0.f, zoomY, 0.f, 0.f,
		0.f, 0.f, (myFarClip) / (myFarClip - myNearClip), 1.f,
		0.f, 0.f, (-myNearClip * myFarClip) / (myFarClip - myNearClip), 0.f
	};
}

void Camera::SetPosition3(Vector3<float> aPosition)
{
	myProjectionMatrix.worldToClipMatrix(4,1) = aPosition.x;
	myProjectionMatrix.worldToClipMatrix(4,2) = aPosition.y;
	myProjectionMatrix.worldToClipMatrix(4,3) = aPosition.z;
}
