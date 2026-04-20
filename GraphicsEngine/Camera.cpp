#include "Camera.h"
#include <cmath>

void Camera::Init(float aFarClip, float aNearClip, float aFOVinDeg, float aAspect)
{
	myFarClip = aFarClip;
	myNearClip = aNearClip;
	myYfov = aFOVinDeg * 3.1415927f / 180.f;

	float zoomY = 1.f / tan(myYfov * 0.5f);
	float zoomX = zoomY * (aAspect);

	myProjectionMatrix = {
		zoomX, 0.f, 0.f, 0.f,
		0.f, zoomY, 0.f, 0.f,
		0.f, 0.f, (myFarClip) / (myFarClip - myNearClip), 0.f,
		0.f, 0.f, (-myNearClip * myFarClip) / (myFarClip - myNearClip), 0.f
	};
}

void Camera::SetPosition3(Vector3 aPosition)
{
	myProjectionMatrix.worldToClipMatrix[3] = aPosition.x;
	myProjectionMatrix.worldToClipMatrix[7] = aPosition.y;
	myProjectionMatrix.worldToClipMatrix[11] = aPosition.z;
}
