#include "Camera.h"
#include <cmath>

#include <d3d11.h>


bool Camera::Init(ID3D11Device* aDevice, float aFarClip, float aNearClip, float aFOVinDeg, float aAspect)
{
	myFarClip = aFarClip;
	myNearClip = aNearClip;
	myYfov = aFOVinDeg * 3.1415927f / 180.f;

	float zoomY = 1.f / tan(myYfov * 0.5f);
	float zoomX = zoomY * (aAspect);

	myProjectionMatrix = Matrix4x4<float> {
		zoomX, 0.f, 0.f, 0.f,
		0.f, zoomY, 0.f, 0.f,
		0.f, 0.f, (myFarClip) / (myFarClip - myNearClip), 1.f,
		0.f, 0.f, (-myNearClip * myFarClip) / (myFarClip - myNearClip), 0.f
	};


	D3D11_BUFFER_DESC vertexBufferDesc{ 0 };
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.ByteWidth = sizeof(Buffer::PerCameraBuffer);
	HRESULT result = aDevice->CreateBuffer(&vertexBufferDesc, nullptr, &myPerCameraBuffer);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Camera::Bind(ID3D11DeviceContext* aDeviceContext)
{
	Matrix4x4<float> transform = {
		Matrix4x4<float>::CreateRotationAroundX(myRotation.x) *
		Matrix4x4<float>::CreateRotationAroundY(myRotation.y) *
		Matrix4x4<float>
		{
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			myPosition.x, myPosition.y, myPosition.z, 1,
		}
	};

	Buffer::PerCameraBuffer perCameraBuffer;
	perCameraBuffer.worldToClipMatrix = transform.GetFastInverse() * myProjectionMatrix;
	perCameraBuffer.eyePosition = myPosition;

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	aDeviceContext->Map(myPerCameraBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &perCameraBuffer, sizeof(Buffer::PerCameraBuffer));
	aDeviceContext->Unmap(myPerCameraBuffer.Get(), 0);
	aDeviceContext->VSSetConstantBuffers(0, 1, myPerCameraBuffer.GetAddressOf());
	aDeviceContext->PSSetConstantBuffers(0, 1, myPerCameraBuffer.GetAddressOf());


}

void Camera::SetPosition3(Vector3<float> aPosition)
{
	myPosition = aPosition;
}

void Camera::SetRotation(Vector3<float> aRotation)
{
	myRotation = aRotation;
}
