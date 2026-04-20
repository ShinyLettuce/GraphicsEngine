#pragma once
#include "BufferData.h"
#include "Vector.h"

class Camera
{
public:
	void Init(float aFarClip, float aNearClip, float aFOV, float aAspect);
	
	void SetPosition3(Vector3 aPosition);
	Vector3 GetPosition() const { return Vector3{ myProjectionMatrix.worldToClipMatrix[3], myProjectionMatrix.worldToClipMatrix[7], myProjectionMatrix.worldToClipMatrix[11] }; }
private:

	float myFarClip = 1000.f;
	float myNearClip = 0.1f;
	float myYfov = 90 * 3.1415927f / 180.f;

	BufferData::FrameBufferData myProjectionMatrix;
};