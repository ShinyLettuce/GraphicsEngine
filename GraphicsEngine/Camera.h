#pragma once
#include "BufferData.h"
#include "Vector.h"

#include <wrl/client.h>


struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;

using Microsoft::WRL::ComPtr;

class Camera
{
public:
	bool Init(ID3D11Device* aDevice ,float aFarClip, float aNearClip, float aFOVinDeg, float aAspect);

	void Bind(ID3D11DeviceContext* aDeviceContext);
	
	void SetRotation(Vector3<float> aRotation);
	Vector3<float> GetRotation() { return myRotation; }

	void SetPosition3(Vector3<float> aPosition);
	Vector3<float> GetPosition() { return myPosition; }
private:

	float myFarClip = 1000.f;
	float myNearClip = 0.1f;
	float myYfov = 90 * 3.1415927f / 180.f;

	Vector3<float> myRotation;
	Vector3<float> myPosition;

	Matrix4x4<float> myProjectionMatrix;

	ComPtr<ID3D11Buffer> myPerCameraBuffer;

};