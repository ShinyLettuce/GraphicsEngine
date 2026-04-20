#pragma once
#include "BufferData.h"
#include "Vector.h"

class Camera
{
public:
	void Init(float aFarClip, float aNearClip, float aFOVinDeg, float aAspect);
	
	void SetPosition3(Vector3<float> aPosition);
	Vector3<float> GetPosition() const { return Vector3<float>{ myProjectionMatrix.worldToClipMatrix[12], myProjectionMatrix.worldToClipMatrix[13], myProjectionMatrix.worldToClipMatrix[14] }; }

	BufferData::FrameBufferData GetFrameBufferData() { return myProjectionMatrix; }
private:

	float myFarClip = 1000.f;
	float myNearClip = 0.1f;
	float myYfov = 90 * 3.1415927f / 180.f;

	BufferData::FrameBufferData myProjectionMatrix;
};