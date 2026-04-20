#pragma once
#include "BufferData.h"
#include "Vector.h"

class Camera
{
public:
	void Init(float aFarClip, float aNearClip, float aFOVinDeg, float aAspect);
	
	void SetRotation(Vector3<float> aRotation);
	Vector3<float> GetRotation() { return myRotation; }

	void SetPosition3(Vector3<float> aPosition);
	Vector3<float> GetPosition() { return myPosition; }

	void SetFrameBufferData(const BufferData::FrameBufferData& aData) { myProjectionMatrix = aData; }
	BufferData::FrameBufferData GetFrameBufferData() { return myProjectionMatrix; }
private:

	float myFarClip = 1000.f;
	float myNearClip = 0.1f;
	float myYfov = 90 * 3.1415927f / 180.f;

	Vector3<float> myRotation;
	Vector3<float> myPosition;

	BufferData::FrameBufferData myProjectionMatrix;
};