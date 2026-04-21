#pragma once
#include "Matrix4x4.h"
#include "Matrix3x3.h"
#include "Vector3.h"

namespace Buffer
{
	struct alignas(16) PerCameraBuffer
	{
		Matrix4x4<float> worldToClipMatrix;
		Vector3<float> eyePosition;
	};

	struct alignas(16) PerFrameBuffer
	{
		float time;
	};

	struct alignas(16) PerObjectBufferData
	{
		Matrix4x4<float> modelToWorldMatrix;
	};
}